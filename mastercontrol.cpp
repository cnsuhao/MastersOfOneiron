/* Masters of Oneiron
// Copyright (C) 2016 LucKey Productions (luckeyproductions.nl)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "world.h"
#include "oneirocam.h"
#include "platform.h"
#include "tile.h"
#include "slot.h"
#include "frop.h"
#include "grass.h"
#include "ekelplitf.h"

#include "inputmaster.h"
#include "resourcemaster.h"
#include "spawnmaster.h"

#include "mastercontrol.h"

URHO3D_DEFINE_APPLICATION_MAIN(MasterControl);

MasterControl* MasterControl::instance_ = NULL;

MasterControl* MasterControl::GetInstance()
{
    return MasterControl::instance_;
}

MasterControl::MasterControl(Context *context):
    Application(context),
    platformMap_{},
    paused_{false}
{
    instance_ = this;
}

void MasterControl::Setup()
{
    SetRandomSeed(GetSubsystem<Time>()->GetSystemTime());
    // Modify engine startup parameters.
    //Set custom window title and icon.
    engineParameters_[EP_WINDOW_TITLE] = "Masters of Oneiron";
    engineParameters_[EP_WINDOW_ICON] = "icon.png";
    engineParameters_[EP_LOG_NAME] = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs")+"Oneiron.log";
    engineParameters_[EP_RESOURCE_PATHS] = "Data;CoreData;Resources";

//    engineParameters_["FullScreen"] = true;
//    engineParameters_["Headless"] = false;
//    engineParameters_["WindowWidth"] = 960;
//    engineParameters_["WindowHeight"] = 540;
}
void MasterControl::Start()
{
    World::RegisterObject(context_);
    OneiroCam::RegisterObject(context_);
    Ekelplitf::RegisterObject(context_);
    Tile::RegisterObject(context_);
    Slot::RegisterObject(context_);
    Frop::RegisterObject(context_);
    Grass::RegisterObject(context_);
    Platform::RegisterObject(context_);

    context_->RegisterSubsystem(this);
    context_->RegisterSubsystem(new InputMaster(context_));
    context_->RegisterSubsystem(new SpawnMaster(context_));
    context_->RegisterSubsystem(new ResourceMaster(context_));

    // Get default style
    defaultStyle_ = CACHE->GetResource<XMLFile>("UI/DefaultStyle.xml");
    SetWindowTitleAndIcon();
    //Create console and debug HUD.
    CreateConsoleAndDebugHud();
    //Create the scene content
    CreateScene();
    //Create the UI content
    CreateUI();
    //Hook up to the frame update and render post-update events
    SubscribeToEvents();

    //Sound* music = cache_->GetResource<Sound>("Resources/Music/Macroform_-_Compassion.ogg"); //Main menu
    //Sound* music = cache_->GetResource<Sound>("Resources/Music/Macroform_-_Dreaming.ogg");
    Sound* music{ CACHE->GetResource<Sound>("Resources/Music/Macroform_-_Root.ogg") };
    music->SetLooped(true);
    Node* musicNode{world.scene->CreateChild("Music")};
    SoundSource* musicSource{musicNode->CreateComponent<SoundSource>()};
    musicSource->SetSoundType(SOUND_MUSIC);
//    musicSource->Play(music);
}
void MasterControl::Stop()
{
    engine_->DumpResources(true);
}

void MasterControl::SubscribeToEvents()
{
    //Subscribe scene update event.
    SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(MasterControl, HandleSceneUpdate));
    //Subscribe HandleUpdate() function for processing update events
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(MasterControl, HandleUpdate));
    //Subscribe scene update event.
    SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(MasterControl, HandleSceneUpdate));

    //Render Debug Geometry
//    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(MasterControl, HandlePostRenderUpdate));
}

void MasterControl::SetWindowTitleAndIcon()
{
    //Create console
    Console* console{engine_->CreateConsole()};
    console->SetDefaultStyle(defaultStyle_);
    console->GetBackground()->SetOpacity(0.0f);

    //Create debug HUD
    DebugHud* debugHud{engine_->CreateDebugHud()};
    debugHud->SetDefaultStyle(defaultStyle_);
}

void MasterControl::CreateConsoleAndDebugHud()
{
    // Create console
    Console* console{engine_->CreateConsole()};
    console->SetDefaultStyle(defaultStyle_);
    console->GetBackground()->SetOpacity(0.8f);

    // Create debug HUD.
    DebugHud* debugHud{engine_->CreateDebugHud()};
    debugHud->SetDefaultStyle(defaultStyle_);
}

void MasterControl::CreateUI()
{
    ResourceCache* cache{ GetSubsystem<ResourceCache>() };
    UI* ui{ GetSubsystem<UI>() };

    //Create a Cursor UI element because we want to be able to hide and show it at will. When hidden, the mouse cursor will control the camera
    world.cursor.uiCursor = new Cursor(context_);
    world.cursor.uiCursor->SetVisible(false);
    ui->SetCursor(world.cursor.uiCursor);

    //Set starting position of the cursor at the rendering window center
    world.cursor.uiCursor->SetPosition(GRAPHICS->GetWidth() / 2, GRAPHICS->GetHeight() / 2);

    //Construct new Text object, set string to display and font to use
    Text* instructionText{ui->GetRoot()->CreateChild<Text>()};
    instructionText->SetText("Masters of Oneiron");
    instructionText->SetFont(cache->GetResource<Font>("Resources/Fonts/Riau.ttf"), 32);
    instructionText->SetColor(Color(0.023f, 1.0f, 0.95f, 0.75f));
    //The text has multiple rows. Center them in relation to each other
    instructionText->SetHorizontalAlignment(HA_CENTER);
    instructionText->SetVerticalAlignment(VA_CENTER);
    instructionText->SetPosition(0, ui->GetRoot()->GetHeight()/2.1);
}

void MasterControl::CreateScene()
{
    world.scene = new Scene(context_);


    //Create octree, use default volume (-1000, -1000, -1000) to (1000,1000,1000)
    world.scene->CreateComponent<Octree>();

    World* world2 = world.scene->CreateComponent<World>();
//    World* world2{ SPAWN->Create<World>() };

    PhysicsWorld* physicsWorld{ world.scene->CreateComponent<PhysicsWorld>()};
    physicsWorld->SetGravity(Vector3::ZERO);


    world.scene->CreateComponent<DebugRenderer>();

    //Create cursor
    world.cursor.sceneCursor = world.scene->CreateChild("Cursor");
    world.cursor.sceneCursor->SetPosition(Vector3(0.0f,0.0f,0.0f));
    StaticModel* cursorObject{world.cursor.sceneCursor->CreateComponent<StaticModel>()};
    cursorObject->SetModel(CACHE->GetResource<Model>("Resources/Models/Kekelplithf.mdl"));
    cursorObject->SetMaterial(CACHE->GetResource<Material>("Resources/Materials/Glow.xml"));

    //Create an Invisible plane for mouse raycasting
//    world.voidNode = world.scene->CreateChild("Void");
    //Location is set in update since the plane moves with the camera.
//    world.voidNode->SetScale(Vector3(1000.0f, 1.0f, 1000.0f));
//    StaticModel* planeObject{world.voidNode->CreateComponent<StaticModel>()};
//    planeObject->SetModel(RESOURCE->GetModel("Plane"));
//    planeObject->SetMaterial(RESOURCE->GetMaterial("Invisible"));

    //Create background
    /*for (int i{-2}; i <= 2; ++i){
        for (int j{-2}; j <= 2; ++j){
            world.backgroundNode = world.scene->CreateChild("BackPlane");
            world.backgroundNode->SetScale(Vector3(512.0f, 1.0f, 512.0f));
            world.backgroundNode->SetPosition(Vector3(512.0f * i, -200.0f, 512.0f * j));
            StaticModel* backgroundObject{world.backgroundNode->CreateComponent<StaticModel>()};
            backgroundObject->SetModel(cache_->GetResource<Model>("Models/Plane.mdl"));
            backgroundObject->SetMaterial(cache_->GetResource<Material>("Resources/Materials/DreamSky.xml"));
        }
    }*/

    world.sunNode = world.scene->CreateChild("Core");
    world.sunNode->SetScale(WORLD_RADIUS * 0.1f);
    StaticModel* sunModel{ world.sunNode->CreateComponent<StaticModel>() };
    sunModel->SetModel(RESOURCE->GetModel("Core"));
    sunModel->SetMaterial(RESOURCE->GetMaterial("Core"));

    Light* sunLight{world.sunNode->CreateComponent<Light>()};
    sunLight->SetLightType(LIGHT_POINT);
    sunLight->SetBrightness(1.666f);
    sunLight->SetRange(WORLD_RADIUS * 1.5f);
    sunLight->SetColor(Color(0.9f, 0.95f, 1.0f));
//    sunLight->SetCastShadows(true);
//    sunLight->SetShadowBias(BiasParameters(0.0000023f, 0.23f));
//    sunLight->SetShadowResolution(1.0f);
//    sunLight->SetShadowCascade(CascadeParameters(4.0f, 16.0f, 64.0f, 0.5f, 1.0f));

    world.sunNode->CreateComponent<RigidBody>();

//    Node* bubbleNode{ world.scene->CreateChild("Bubble") };
//    bubbleNode->SetScale(WORLD_RADIUS);
//    StaticModel* bubble{bubbleNode->CreateComponent<StaticModel>()};
//    bubble->SetModel(RESOURCE->GetModel("Bubble"));
//    bubble->SetMaterial(RESOURCE->GetMaterial("Bubble"));

//    Node* lightNode2{world.scene->CreateChild("AmbientLight")};
//    lightNode2->SetDirection(Vector3(0.0f, 1.0f, 0.0f));
//    Light* light2{lightNode2->CreateComponent<Light>()};
//    light2->SetLightType(LIGHT_DIRECTIONAL);
//    light2->SetBrightness(0.23f);
//    light2->SetColor(Color(1.0f, 1.0f, 0.9f));
//    light2->SetCastShadows(true);
//    light2->SetShadowBias(BiasParameters(0.00025f, 0.5f));
//    light2->SetShadowResolution(0.666f);


    //Create camera
    world.camera = world.scene->CreateChild()->CreateComponent<OneiroCam>();

    //Add some random platforms
//    for (int p{0}; p < 23; ++p){
    for (Vector3 v : world2->GetRhombicTriacontahedricPoints()) {
        SPAWN->Create<Platform>()->Set(v);
    }
    for (Vector3 v : world2->GetRhombicCenters()) {
        SPAWN->Create<Platform>()->Set(v);
    }
}

void MasterControl::HandleUpdate(StringHash eventType, VariantMap &eventData)
{

}

void MasterControl::HandleSceneUpdate(StringHash eventType, VariantMap &eventData)
{
    float t{eventData[Update::P_TIMESTEP].GetFloat()};
//    world.voidNode->SetPosition((2.0f*Vector3::DOWN) + (world.camera->GetWorldPosition()*Vector3(1.0f,0.0f,1.0f)));
    UpdateCursor(t);

}

void MasterControl::UpdateCursor(float timeStep)
{
    world.cursor.sceneCursor->Rotate(Quaternion(0.0f, 100.0f * timeStep, 0.0f));
    world.cursor.sceneCursor->SetScale((world.cursor.sceneCursor->GetWorldPosition() - world.camera->GetWorldPosition()).Length() * 0.0023f);
    if (CursorRayCast(5.0f * WORLD_RADIUS, world.cursor.hitResults)) {
        for (int i{0}; i < world.cursor.hitResults.Size(); ++i) {
            if (world.cursor.hitResults[i].node_->GetName() == "Bubble") {
                world.cursor.sceneCursor->SetWorldPosition(world.cursor.hitResults[i].position_);
                return;
            }
        }
    }
}

bool MasterControl::CursorRayCast(float maxDistance, PODVector<RayQueryResult> &hitResults)
{
    Ray cameraRay{ world.camera->camera_->GetScreenRay(0.5f,0.5f) };
    RayOctreeQuery query{ hitResults, cameraRay, RAY_TRIANGLE, maxDistance, DRAWABLE_GEOMETRY };

    world.scene->GetComponent<Octree>()->Raycast(query);

    if (hitResults.Size())
        return true;
    else
        return false;
}

void MasterControl::Exit()
{
    engine_->Exit();
}

void MasterControl::HandlePostRenderUpdate(StringHash eventType, VariantMap &eventData)
{
    world.scene->GetComponent<PhysicsWorld>()->DrawDebugGeometry(true);
}

float MasterControl::Sine(const float freq, const float min, const float max, const float shift)
{
    float phase{ freq * world.scene->GetElapsedTime() + shift };
    float add{ 0.5f * (min + max) };
    return LucKey::Sine(phase) * 0.5f * (max - min) + add;
}
float MasterControl::Cosine(const float freq, const float min, const float max, const float shift)
{
    float phase{ freq * world.scene->GetElapsedTime() + shift };
    float add{ 0.5f * (min + max) };
    return LucKey::Cosine(phase) * 0.5f * (max - min) + add;
}

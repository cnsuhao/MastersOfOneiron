/* Masters of Oneiron
// Copyright (C) 2015 LucKey Productions (luckeyproductions.nl)
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

#include "kekelplithf.h"
#include "platform.h"
#include "oneirocam.h"
#include "inputmaster.h"

#include "mastercontrol.h"

URHO3D_DEFINE_APPLICATION_MAIN(MasterControl);

MasterControl::MasterControl(Context *context):
    Application(context),
    paused_(false)
{
}


void MasterControl::Setup()
{
    // Modify engine startup parameters.
    //Set custom window title and icon.
    engineParameters_["WindowTitle"] = "Masters of Oneiron";
    engineParameters_["LogName"] = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs")+"Oneiron.log";
//    engineParameters_["FullScreen"] = true;
//    engineParameters_["Headless"] = false;
//    engineParameters_["WindowWidth"] = 960;
//    engineParameters_["WindowHeight"] = 540;
}
void MasterControl::Start()
{
    new InputMaster(context_, this);
    cache_ = GetSubsystem<ResourceCache>();
    graphics_ = GetSubsystem<Graphics>();
    renderer_ = GetSubsystem<Renderer>();

    // Get default style
    defaultStyle_ = cache_->GetResource<XMLFile>("UI/DefaultStyle.xml");
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
    Sound* music = cache_->GetResource<Sound>("Resources/Music/Macroform_-_Root.ogg"); //Battle
    music->SetLooped(true);
    Node* musicNode = world.scene->CreateChild("Music");
    SoundSource* musicSource = musicNode->CreateComponent<SoundSource>();
    musicSource->SetSoundType(SOUND_MUSIC);
    musicSource->Play(music);
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
}

void MasterControl::SetWindowTitleAndIcon()
{
    //Create console
    Console* console = engine_->CreateConsole();
    console->SetDefaultStyle(defaultStyle_);
    console->GetBackground()->SetOpacity(0.0f);

    //Create debug HUD
    DebugHud* debugHud = engine_->CreateDebugHud();
    debugHud->SetDefaultStyle(defaultStyle_);
}

void MasterControl::CreateConsoleAndDebugHud()
{
    // Create console
    Console* console = engine_->CreateConsole();
    console->SetDefaultStyle(defaultStyle_);
    console->GetBackground()->SetOpacity(0.8f);

    // Create debug HUD.
    DebugHud* debugHud = engine_->CreateDebugHud();
    debugHud->SetDefaultStyle(defaultStyle_);
}

void MasterControl::CreateUI()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    UI* ui = GetSubsystem<UI>();

    //Create a Cursor UI element because we want to be able to hide and show it at will. When hidden, the mouse cursor will control the camera
    world.cursor.uiCursor = new Cursor(context_);
    world.cursor.uiCursor->SetVisible(false);
    ui->SetCursor(world.cursor.uiCursor);

    //Set starting position of the cursor at the rendering window center
    world.cursor.uiCursor->SetPosition(graphics_->GetWidth()/2, graphics_->GetHeight()/2);

    //Construct new Text object, set string to display and font to use
    Text* instructionText = ui->GetRoot()->CreateChild<Text>();
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

    PhysicsWorld* physicsWorld = world.scene->CreateComponent<PhysicsWorld>();
    physicsWorld->SetGravity(Vector3::ZERO);
    world.scene->CreateComponent<DebugRenderer>();

    //Create cursor
    world.cursor.sceneCursor = world.scene->CreateChild("Cursor");
    world.cursor.sceneCursor->SetPosition(Vector3(0.0f,0.0f,0.0f));
    StaticModel* cursorObject = world.cursor.sceneCursor->CreateComponent<StaticModel>();
    cursorObject->SetModel(cache_->GetResource<Model>("Resources/Models/Kekelplithf.mdl"));
    cursorObject->SetMaterial(cache_->GetResource<Material>("Resources/Materials/Glow.xml"));

    //Create an Invisible plane for mouse raycasting
    world.voidNode = world.scene->CreateChild("Void");
    //Location is set in update since the plane moves with the camera.
    world.voidNode->SetScale(Vector3(1000.0f, 1.0f, 1000.0f));
    StaticModel* planeObject = world.voidNode->CreateComponent<StaticModel>();
    planeObject->SetModel(cache_->GetResource<Model>("Models/Plane.mdl"));
    planeObject->SetMaterial(cache_->GetResource<Material>("Resources/Materials/Invisible.xml"));

    //Create background
    for (int i = -2; i <= 2; i++){
        for (int j = -2; j <= 2; j++){
            world.backgroundNode = world.scene->CreateChild("BackPlane");
            world.backgroundNode->SetScale(Vector3(512.0f, 1.0f, 512.0f));
            world.backgroundNode->SetPosition(Vector3(512.0f*i, -200.0f, 512.0f*j));
            StaticModel* backgroundObject = world.backgroundNode->CreateComponent<StaticModel>();
            backgroundObject->SetModel(cache_->GetResource<Model>("Models/Plane.mdl"));
            backgroundObject->SetMaterial(cache_->GetResource<Material>("Resources/Materials/DreamSky.xml"));
        }
    }

    world.sunLightNode = world.scene->CreateChild("DirectionalLight");
    world.sunLightNode->SetDirection(Vector3(0.0f, -1.0f, 0.0f));
    world.sunLight = world.sunLightNode->CreateComponent<Light>();
    world.sunLight->SetLightType(LIGHT_DIRECTIONAL);
    world.sunLight->SetBrightness(1.0f);
    world.sunLight->SetColor(Color(1.0f, 0.8f, 0.7f));
    world.sunLight->SetCastShadows(true);
    world.sunLight->SetShadowBias(BiasParameters(0.0000023f, 0.23f));
    world.sunLight->SetShadowResolution(2.0f);
    world.sunLight->SetShadowCascade(CascadeParameters(4.0f, 16.0f, 64.0f, 0.5f, 1.0f));

    Node* lightNode2 = world.scene->CreateChild("DirectionalLight");
    lightNode2->SetDirection(Vector3(0.0f, 1.0f, 0.0f));
    Light* light2 = lightNode2->CreateComponent<Light>();
    light2->SetLightType(LIGHT_DIRECTIONAL);
    light2->SetBrightness(0.23f);
    light2->SetColor(Color(1.0f, 1.0f, 0.9f));
    light2->SetCastShadows(true);
    light2->SetShadowBias(BiasParameters(0.00025f, 0.5f));
    light2->SetShadowResolution(0.666f);


    //Create camera
    world.camera = new OneiroCam(context_, this);

    //Add some random platforms
    for (int p = 5; p <= 23; ++p){
        new Platform(context_, Quaternion(Random(60.0f)+72.0f*(p%5), Vector3::UP) * Vector3::FORWARD * p * 5.0f, this, true);
    }
}

void MasterControl::HandleUpdate(StringHash eventType, VariantMap &eventData)
{

}

void MasterControl::HandleSceneUpdate(StringHash eventType, VariantMap &eventData)
{
    using namespace Update;
    double timeStep = eventData[P_TIMESTEP].GetFloat();
    world.voidNode->SetPosition((2.0f*Vector3::DOWN) + (world.camera->GetWorldPosition()*Vector3(1.0f,0.0f,1.0f)));
    UpdateCursor(timeStep);

//    world.sunLightNode->SetDirection(Vector3(sin(world.scene->GetElapsedTime() * 0.23f),
//                                             -5.0f,
//                                             sin(world.scene->GetElapsedTime() * 0.42f)));
}

void MasterControl::UpdateCursor(double timeStep)
{
    world.cursor.sceneCursor->Rotate(Quaternion(0.0f,100.0f*timeStep,0.0f));
    world.cursor.sceneCursor->SetScale((world.cursor.sceneCursor->GetWorldPosition() - world.camera->GetWorldPosition()).Length()*0.0023f);
    if (CursorRayCast(250.0f, world.cursor.hitResults))
    {
        for (int i = 0; i < world.cursor.hitResults.Size(); i++)
        {
            if (world.cursor.hitResults[i].node_->GetNameHash() == N_VOID)
            {
                Vector3 camHitDifference = world.camera->translationNode_->GetWorldPosition() - world.cursor.hitResults[i].position_;
                camHitDifference /= world.camera->translationNode_->GetWorldPosition().y_ - world.voidNode->GetPosition().y_;
                camHitDifference *= world.camera->translationNode_->GetWorldPosition().y_;
                world.cursor.sceneCursor->SetWorldPosition(world.camera->translationNode_->GetWorldPosition()-camHitDifference);
            }
        }
    }
}

bool MasterControl::CursorRayCast(double maxDistance, PODVector<RayQueryResult> &hitResults)
{
    Ray cameraRay = world.camera->camera_->GetScreenRay(0.5f,0.5f);
    RayOctreeQuery query(hitResults, cameraRay, RAY_TRIANGLE, maxDistance, DRAWABLE_GEOMETRY);
    world.scene->GetComponent<Octree>()->Raycast(query);
    if (hitResults.Size()) return true;
    else return false;
}

void MasterControl::Exit()
{
    engine_->Exit();
}

void MasterControl::HandlePostRenderUpdate(StringHash eventType, VariantMap &eventData)
{
    //world.scene->GetComponent<PhysicsWorld>()->DrawDebugGeometry(true);
}






































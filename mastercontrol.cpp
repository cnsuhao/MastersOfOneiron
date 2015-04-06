//
// Copyright (c) 2008-2015 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <Urho3D/Urho3D.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/RenderPath.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Resource/Resource.h>

#include <Urho3D/IO/Log.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/OctreeQuery.h>

#include "mastercontrol.h"
#include "imp.h"
#include "platform.h"

DEFINE_APPLICATION_MAIN(MasterControl);

MasterControl::MasterControl(Context *context):
    Application(context),
    paused_(false)
{
}


void MasterControl::Setup()
{
    // Modify engine startup parameters.
    //Set custom window title and icon.
    engineParameters_["WindowTitle"] = GetTypeName();
    engineParameters_["LogName"] = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs")+GetTypeName()+".log";
    engineParameters_["FullScreen"] = true;
    engineParameters_["Headless"] = false;
    engineParameters_["WindowWidth"] = 1280;
    engineParameters_["WindowHeight"] = 720;
}
void MasterControl::Start()
{
    cache_ = GetSubsystem<ResourceCache>();
    input_ = GetSubsystem<Input>();
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
    //Setup the viewport for displaying the scene
    SetupViewport();
    //Hook up to the frame update and render post-update events
    SubscribeToEvents();
}
void MasterControl::Stop()
{
    engine_->DumpResources(true);
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

    //Create a Cursor UI element because we want to be able to hide and show it at will. When hidden, the mouse cursor will control the camera, and when visible it will point the raycast target

    world.cursor.uiCursor_ = new Cursor(context_);
    world.cursor.uiCursor_->SetVisible(false);
    ui->SetCursor(world.cursor.uiCursor_);

    //Set starting position of the cursor at the rendering window center
    //Graphics* graphics = GetSubsystem<Graphics>();
    world.cursor.uiCursor_->SetPosition(graphics_->GetWidth()/2, graphics_->GetHeight()/2);

    //Construct new Text object, set string to display and font to use
    Text* instructionText = ui->GetRoot()->CreateChild<Text>();
    instructionText->SetText(
                "Masters of Oneiron"
                );
    instructionText->SetFont(cache->GetResource<Font>("Resources/Fonts/Riau.ttf"), 64);
    //The text has multiple rows. Center them in relation to each other
    instructionText->SetHorizontalAlignment(HA_CENTER);
    instructionText->SetVerticalAlignment(VA_CENTER);
    instructionText->SetPosition(0, ui->GetRoot()->GetHeight()/2.3);
}

void MasterControl::CreateScene()
{
//    ResourceCache* cache_ = GetSubsystem<ResourceCache>();

    world.scene_ = new Scene(context_);

    //Create octree, use default volume (-1000, -1000, -1000) to (1000,1000,1000)
    //Also create a DebugRenderer component so that we can draw debug geometry
    world.scene_->CreateComponent<Octree>();
    PhysicsWorld* physicsWorld = world.scene_->CreateComponent<PhysicsWorld>();
    physicsWorld->SetGravity(Vector3::ZERO);
    //world.scene_->CreateComponent<DebugRenderer>();

    //Create cursor
    world.cursor.sceneCursor_ = world.scene_->CreateChild("Cursor");
    world.cursor.sceneCursor_->SetPosition(Vector3(0.0f,0.0f,0.0f));
    StaticModel* cursorObject = world.cursor.sceneCursor_->CreateComponent<StaticModel>();
    cursorObject->SetModel(cache_->GetResource<Model>("Resources/Models/Cursor.mdl"));
    cursorObject->SetMaterial(cache_->GetResource<Material>("Resources/Materials/glow.xml"));

    //Create scene node & StaticModel component for showing a static plane
    world.mouseHitPlaneNode_ = world.scene_->CreateChild("HitPlane");
    world.mouseHitPlaneNode_->SetScale(Vector3(100.0f, 1.0f, 100.0f));
    StaticModel* planeObject = world.mouseHitPlaneNode_->CreateComponent<StaticModel>();
    planeObject->SetModel(cache_->GetResource<Model>("Models/Plane.mdl"));
    planeObject->SetMaterial(cache_->GetResource<Material>("Resources/Materials/invisible.xml"));

    //Create background
    world.backgroundNode_ = world.scene_->CreateChild("BackPlane");
    world.backgroundNode_->SetScale(Vector3(200.0f, 1.0f, 200.0f));
    world.backgroundNode_->SetPosition(0.0f, -50.0f, 0.0f);
    StaticModel* backgroundObject = world.backgroundNode_->CreateComponent<StaticModel>();
    backgroundObject->SetModel(cache_->GetResource<Model>("Models/Plane.mdl"));
    backgroundObject->SetMaterial(cache_->GetResource<Material>("Resources/Materials/dreamsky.xml"));
    //Create a Zone component for ambient lighting & fog control
    /*Node* zoneNode = world.scene_->CreateChild("Zone");
    Zone* zone = zoneNode->CreateComponent<Zone>();
    zone->SetBoundingBox(BoundingBox(Vector3(-1000.0f, -10.0f, -1000.0f),Vector3(1000.0f, 20.0f, 1000.0f)));
    zone->SetAmbientColor(Color(0.15f, 0.15f, 0.15f));
    zone->SetFogColor(Color(0.2f, 0.1f, 0.3f));
    zone->SetFogStart(100.0f);
    zone->SetFogEnd(110.0f);*/

    //Create a directional light to the world. Enable cascaded shadows on it
    Node* lightNode = world.scene_->CreateChild("DirectionalLight");
    lightNode->SetDirection(Vector3(0.6f, -1.0f, 0.8f));
    Light* light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetBrightness(1.5f);
    light->SetColor(Color(1.0f,0.9f,0.95f));
    light->SetCastShadows(true);
    light->SetShadowBias(BiasParameters(0.00025f, 0.5f));

    //Set cascade splits at 10, 50, 200 world unitys, fade shadows at 80% of maximum shadow distance
    light->SetShadowCascade(CascadeParameters(10.0f, 50.0f, 200.0f, 0.0f, 0.8f));

    //Create some mushrooms
    const unsigned NUM_BLOCKS = 1;
    for (unsigned i = 0; i < NUM_BLOCKS ; ++i)
        CreatePlatform(Vector3(Random(22.0f) - 11.0f, 0.0f, Random(22.0f) - 11.0f));

    //Create randomly sized boxes. If boxes are big enough make them occluders
    /*const unsigned NUM_BOXES = 20;
    for (unsigned i = 0; i <NUM_BOXES; ++i)
    {
        Node* boxNode = world.scene_->CreateChild("Box");
        float size = 1.0f + Random(10.0f);
        boxNode->SetPosition(Vector3(Random(80.0f) - 40.0f, size * 0.5f, Random(80.0f) - 40.0f));
        boxNode->SetScale(size);
        StaticModel* boxObject = boxNode->CreateComponent<StaticModel>();
        boxObject->SetModel(cache->GetResource<Model>("Resources/Models/Cube.mdl"));
        boxObject->SetMaterial(cache->GetResource<Material>("Materials/StoneTiled.xml"));
        boxObject->SetCastShadows(true);
        if (size >= 3.0f)
            boxObject->SetOccluder(true);
    }*/
    //Spawn Imps
    for (int i = 0; i < 10; i++) new Imp(context_, this);

    //Create the camera. Limit far clip distance to match the fog
    world.camera.translationNode_ = world.scene_->CreateChild("CamTrans");
    world.camera.rotationNode_ = world.camera.translationNode_->CreateChild("CamRot");
    world.camera.camera_ = world.camera.rotationNode_->CreateComponent<Camera>();
    world.camera.camera_->SetFarClip(1024.0f);
    //Set an initial position for the camera scene node above the plane
    world.camera.translationNode_->SetPosition(Vector3(0.0f, 3.0f, 0.0f));
    world.camera.rigidBody_ = world.camera.translationNode_->CreateComponent<RigidBody>();
    world.camera.rigidBody_->SetAngularDamping(10.0f);
    CollisionShape* collisionShape = world.camera.translationNode_->CreateComponent<CollisionShape>();
    collisionShape->SetSphere(0.1f);
    world.camera.rigidBody_->SetMass(1.0f);
}

void MasterControl::SetupViewport()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();

    //Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
    SharedPtr<Viewport> viewport(new Viewport(context_, world.scene_, world.camera.camera_));
    world.camera.viewport_ = viewport;

    //Add anti-asliasing
    world.camera.effectRenderPath = world.camera.viewport_->GetRenderPath()->Clone();
    world.camera.effectRenderPath->Append(cache->GetResource<XMLFile>("PostProcess/FXAA2.xml"));
    world.camera.effectRenderPath->SetEnabled("FXAA2", true);

    world.camera.viewport_->SetRenderPath(world.camera.effectRenderPath);
    renderer->SetViewport(0, viewport);
}
void MasterControl::MoveCamera(float timeStep)
{
    //Movement speed as world units per second
    const float MOVE_SPEED = 2000.0f;
    //Mouse sensitivity as degrees per pixel
    const float MOUSE_SENSITIVITY = 0.1f;

    //Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees. Only move the camera when the cursor is hidden.
    /*if (!ui->GetCursor()->IsVisible())
    {*/
    Input* input = GetSubsystem<Input>();
    IntVector2 mouseMove = input->GetMouseMove();
        world.camera.yawDelta_ = 0.5f*(world.camera.yawDelta_ + MOUSE_SENSITIVITY * mouseMove.x_);
        world.camera.pitchDelta_ = 0.5f*(world.camera.pitchDelta_ + MOUSE_SENSITIVITY * mouseMove.y_);
        world.camera.yaw_ += world.camera.yawDelta_;
        world.camera.pitch_ += world.camera.pitchDelta_;
        world.camera.pitch_ = Clamp(world.camera.pitch_, -90.0f, 90.0f);

        //Construct new orientation for the camera scene node from yaw and pitch. Roll is fixed to zero
        world.camera.rotationNode_->SetRotation(Quaternion(world.camera.pitch_, world.camera.yaw_, 0.0f));

    /*}*/

    //Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
    Vector3 camForce = Vector3::ZERO;
    if (input->GetKeyDown('W')) camForce += Vector3::Scale(world.camera.rotationNode_->GetDirection(),Vector3(1.0f,0.0f,1.0f)).Normalized();
    if (input->GetKeyDown('S')) camForce += Vector3::Scale(world.camera.rotationNode_->GetDirection(),Vector3(-1.0f,0.0f,-1.0f)).Normalized();
    if (input->GetKeyDown('D')) camForce += Vector3::Scale(world.camera.rotationNode_->GetRight(),Vector3(1.0f,0.0f,1.0f)).Normalized();
    if (input->GetKeyDown('A')) camForce += Vector3::Scale(world.camera.rotationNode_->GetRight(),Vector3(-1.0f,0.0f,-1.0f)).Normalized();
    if (input->GetKeyDown('E')) camForce += Vector3::UP;
    if (input->GetKeyDown('Q')) camForce += Vector3::DOWN;
    camForce = camForce.Normalized() * MOVE_SPEED * timeStep;

    world.camera.rigidBody_->ApplyForce(camForce - (2.0f*world.camera.rigidBody_->GetLinearVelocity()));
}

void MasterControl::HandleKeyDown(StringHash eventType, VariantMap &eventData)
{
    using namespace KeyDown;

    int key = eventData[P_KEY].GetInt();

    //Close console (if open) or exit when ESC is pressed
    if (key == KEY_ESC)
    {
        /*Console* console = GetSubsystem<Console>();
        if (console->IsVisible())
            console->SetVisible(false);
        else*/
            engine_->Exit();
    }
    //Toggle console with F1
    /*else if (key == KEY_F1)
        GetSubsystem<Console>()->Toggle();
    //Toggle debug HUD with F2
    else if (key == KEY_F2)
        GetSubsystem<DebugHud>()->ToggleAll();
    //Rendering quality controls, only when UI has no focused element
    else if (!GetSubsystem<UI>()->GetFocusElement())
    {
        //Texture quality
        /*else*//* if (key == '1')
    {
            int quality = renderer_->GetTextureQuality();
            ++quality;
            if (quality>QUALITY_HIGH)
                quality = QUALITY_LOW;
            renderer_->SetTextureQuality(quality);
        }

        //Material quality
        else if (key == '2')
        {
            int quality = renderer_->GetMaterialQuality();
            ++quality;
            if (quality>QUALITY_HIGH)
                quality = QUALITY_LOW;
            renderer_->SetMaterialQuality(quality);
        }

        //Specular lighting
        else if (key == '3')
            renderer_->SetSpecularLighting(!renderer_->GetSpecularLighting());
        //Shadow rendering
        else if (key == '4')
            renderer_->SetDrawShadows(!renderer_->GetDrawShadows());
        //Shadow map resolution
        else if (key == '5')
        {
            int shadowMapSize = renderer_->GetShadowMapSize();
            shadowMapSize *= 2;
            if (shadowMapSize > 4096)
                shadowMapSize = 512;
            renderer_->SetShadowMapSize(shadowMapSize);
        }
        //Shadow depth and filtering quality
        else if (key == '6')
        {
            int quality = renderer_->GetShadowQuality();
            ++quality;
            if (quality > SHADOWQUALITY_HIGH_24BIT)
                quality = SHADOWQUALITY_LOW_16BIT;
            renderer_->SetShadowQuality(quality);
        }
        //Occlusion culling
        else if (key == '7')
        {
            bool occlusion = renderer_->GetMaxOccluderTriangles() > 0;
            occlusion = !occlusion;
            renderer_->SetMaxOccluderTriangles(occlusion ? 5000 : 0);
        }
        //Instancing
        else if (key == '8')
            renderer_->SetDynamicInstancing(!renderer_->GetDynamicInstancing());
        //Take screenshot
  */else if (key == '9')
    {
        Graphics* graphics = GetSubsystem<Graphics>();
        Image screenshot(context_);
        graphics->TakeScreenShot(screenshot);
        //Here we save in the Data folder with date and time appended
        String fileName = GetSubsystem<FileSystem>()->GetProgramDir() + "Screenshots/Screenshot_" + Time::GetTimeStamp().Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_')+".png";
        Log::Write(1, fileName);
        screenshot.SavePNG(fileName);
    }
}
void MasterControl::HandleUpdate(StringHash eventType, VariantMap &eventData)
{

}
void MasterControl::HandleSceneUpdate(StringHash eventType, VariantMap &eventData)
{
    using namespace Update;

    //Take the frame time step, which is stored as a float
    float timeStep = eventData[P_TIMESTEP].GetFloat();
    //Move the camera, scale movement with time step

    //world.camera.camLight_->SetFov((2.0f + sinf(10.0f*world.scene_->GetElapsedTime()))/10.0f);
    UpdateCursor(timeStep);
    MoveCamera(timeStep);
}
/*void Urho3DTemplate::HandlePostRenderUpdate(StringHash eventType, VariantMap &eventData)
{
}*/

void MasterControl::SubscribeToEvents()
{
    //Subscribe key down event.
    SubscribeToEvent(E_KEYDOWN, HANDLER(MasterControl, HandleKeyDown));
    //Subscribe scene update event.
    SubscribeToEvent(E_SCENEUPDATE, HANDLER(MasterControl, HandleSceneUpdate));
    //Subscribe HandleUpdate() function for processing update events
    SubscribeToEvent(E_UPDATE, HANDLER(MasterControl, HandleUpdate));
    //Subscribe key down event.
    SubscribeToEvent(E_KEYDOWN, HANDLER(MasterControl, HandleKeyDown));
    //Subscribe scene update event.
    SubscribeToEvent(E_SCENEUPDATE, HANDLER(MasterControl, HandleSceneUpdate));
    SubscribeToEvent(E_MOUSEBUTTONDOWN, HANDLER(MasterControl, HandleMouseDown));
    //Subscribe HandlePostRenderUpdate() function for processing the post-render update event, during which we request debug geometry
    //SubscribeToEvent(E_POSTRENDERUPDATE, HANDLER(Urho3DTemplate, HandlePostRenderUpdate));
}



void MasterControl::HandleMouseDown(StringHash eventType, VariantMap &eventData)
{
    using namespace MouseButtonDown;
    int key = eventData[P_BUTTON].GetInt();
    if (key == MOUSEB_LEFT){
        CreatePlatform(world.cursor.sceneCursor_->GetPosition());
    }
}

void MasterControl::CreatePlatform(const Vector3 &pos)
{
    IntVector2 flooredPos = IntVector2(round(pos.x_), round(pos.z_));

    /*ResourceCache* cache = GetSubsystem<ResourceCache>();

    Node* blockNode = world.scene_->CreateChild("Block");
    blockNode->SetPosition(flooredPos);
    //blockNode->SetRotation(Quaternion(0.0f, Random(360.0f), 0.0f));
    //blockNode->SetScale(1.0f + Random(0.5f));
    StaticModel* blockObject = blockNode->CreateComponent<StaticModel>();
    blockObject->SetModel(cache->GetResource<Model>("Resources/Models/Cube.mdl"));
    blockObject->SetMaterial(cache->GetResource<Material>("Materials/StoneTiled.xml"));
    blockObject->SetCastShadows(true);

    return blockNode;*/
    //new Tile(context_, flooredPos, this);
    new Platform(context_, pos, this);
}
void MasterControl::UpdateCursor(float timeStep)
{
    world.cursor.sceneCursor_->Rotate(Quaternion(0.0f,timeStep,0.0f));
    HitInfo hitResult;
    if (RayCast(250.0f, hitResult))
    {
        world.cursor.sceneCursor_->SetPosition(Vector3(hitResult.position_.x_, 0.0f, hitResult.position_.z_));
    }

}

bool MasterControl::RayCast(float maxDistance, HitInfo &hitResult)
{
    //UI* ui = GetSubsystem<UI>();
    //IntVector2 cursorPos = ui->GetCursorPosition();
    //Graphics* graphics = GetSubsystem<Graphics>();
    Ray cameraRay = world.camera.camera_->GetScreenRay(0.5f,0.5f);
    //Pick only geometry objects, not eg zones or lights, only get the first (closest) hit
    PODVector<RayQueryResult> hitResults;
    RayOctreeQuery query(hitResults, cameraRay, RAY_TRIANGLE, maxDistance, DRAWABLE_GEOMETRY);
    world.scene_->GetComponent<Octree>()->Raycast(query);
    if (hitResults.Size())
    {
        for (int i = 0; i < hitResults.Size(); i++)
        {
            if (hitResults[i].node_->GetName()=="HitPlane")
            {
                hitResult.hitNode_ = hitResults[0].node_;
                hitResult.hitNormal_ = hitResults[0].normal_;
                hitResult.position_ = hitResults[0].position_;
                hitResult.drawable_ = hitResults[0].drawable_;
                return true;
            }
        }
    }

    return false;
}






































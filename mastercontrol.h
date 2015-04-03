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

#pragma once

#include "common.h"
#include <Urho3D/Urho3D.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Math/Plane.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource.h>
#include <Urho3D/Input/Input.h>


namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

typedef struct GameWorld
{
    Node* cursor_;
    struct
    {
        SharedPtr<Node> translationNode_;
        SharedPtr<Node> rotationNode_;
        SharedPtr<Camera> camera_;
        SharedPtr<Light>  camLight_;
        float yaw_ = 0.0f;
        float pitch_ = 0.0f;
        SharedPtr<Viewport> viewport_;
        SharedPtr<RenderPath> effectRenderPath;
    } camera;

    /// Scene.
    SharedPtr<Scene> scene_;
} GameWorld;

typedef struct HitInfo
{
    Vector3 position_;
    Vector3 hitNormal_;
    Node* hitNode_;
    Drawable* drawable_;
} HitInfo;

class MasterControl : public Application
{
    /// Enable type information.
    OBJECT(MasterControl);

public:
    /// Constructor.
    MasterControl(Context* context);

    /// Setup before engine initialization. Modifies the engine paramaters.
    virtual void Setup();
    /// Setup after engine initialization.
    virtual void Start();
    /// Cleanup after the main loop. Called by Application.
    virtual void Stop();

    GameWorld world;
protected:
    /*SharedPtr<ResourceCache> cache_;
    SharedPtr<Input> input_;
    SharedPtr<UI> ui_;
    SharedPtr<Graphics> graphics_;
    SharedPtr<Renderer> renderer_;

    XMLFile* defaultStyle_;
*/
private:
    /// Set custom window title and icon
    void SetWindowTitleAndIcon();
    /// Create console and debug HUD
    void CreateConsoleAndDebugHud();

    /// Construct the scene content.
    void CreateScene();
    /// Construct user interface elements.
    void CreateUI();
    /// Set up a viewport for displaying the scene.
    void SetupViewport();
    /// Subscribe to application-wide logic update and post-render update events.
    void SubscribeToEvents();
    /// Read input and moves the camera.
    void MoveCamera(float timeStep);

    /// Handle key down event to provess key controls.
    void HandleKeyDown(StringHash eventType, VariantMap& eventData);
    /// Add or remove object.
    void HandleMouseDown(StringHash eventType, VariantMap &eventData);
    /// Handle scene update event to control camera's pitch and yaw.
    void HandleSceneUpdate(StringHash eventType, VariantMap& eventData);
    /// Handle the logic update event.
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    /// Handle the post-render update event.
    void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData);

    /// Create a mushroom object at position.
    Node* CreateBlock(const Vector3& pos);
    void UpdateCursor();
    /// Utility function to raycast to the cursor position. Return true if hit.
    bool RayCast(float maxDistance, HitInfo& hitResult);

    ///Imp scene node.
    SharedPtr<Node> impNode_;

    /// Pause flag
    bool paused_;


};
































#pragma once

#include "mastercontrol.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
class Viewport;
class RenderPath;
class Camera;
}

using namespace Urho3D;

class OneiroCam : public Object
{
    OBJECT(OneiroCam);
    friend class MasterControl;
public:
    OneiroCam(Context *context, MasterControl* masterControl);

    virtual void Start();
    virtual void Stop();

    SharedPtr<Camera> camera_;
    SharedPtr<Viewport> viewport_;
    SharedPtr<RenderPath> effectRenderPath;

    Vector3 GetPosition();
    Quaternion GetRotation();
private:
    MasterControl* masterControl_;
    void HandleSceneUpdate(StringHash eventType, VariantMap &eventData);
    SharedPtr<Node> translationNode_;
    SharedPtr<Node> rotationNode_;

    SharedPtr<RigidBody> rigidBody_;
    double yaw_ = 0.0;
    double pitch_ = 0.0;
    //double roll_ = 0.0;
    double yawDelta_ = 0.0;
    double pitchDelta_ = 0.0;
    double forceMultiplier = 1.0;
    void SetupViewport();
};
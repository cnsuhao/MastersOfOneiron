
#ifndef CONTROLLABLE_H
#define CONTROLLABLE_H

#include <Urho3D/Urho3D.h>
#include <bitset>

#include "sceneobject.h"

#define INPUTMASTER GetSubsystem<InputMaster>()

class Controllable : public SceneObject
{
    friend class InputMaster;
    URHO3D_OBJECT(Controllable, SceneObject);
public:
    Controllable(Context* context);
    virtual void OnNodeSet(Node* node);
    virtual void Update(float timeStep);

    void SetMove(Vector3 move);
    void SetAim(Vector3 aim);
    virtual void ClearControl();

    virtual void Think(float timeStep) {}

    Vector3 GetLinearVelocity() const { return rigidBody_->GetLinearVelocity(); }

    Player* GetPlayer();
protected:
    bool controlled_;
    Vector3 move_;
    Vector3 aim_;
    float thrust_;
    float maxSpeed_;
    float maxPitch_;
    float minPitch_;

    std::bitset<4> actions_;
    HashMap<int, float> actionSince_;
    HashMap<int, float> actionInterval_;

    AnimatedModel* model_;
    RigidBody* rigidBody_;
    CollisionShape* collisionShape_;
    AnimationController* animCtrl_;

    void ResetInput() { move_ = aim_ = Vector3::ZERO; actions_.reset(); }
    void SetActions(std::bitset<4> actions);
    void ClampPitch(Quaternion& rot);

    void AlignWithVelocity(float timeStep);
    void AlignWithMovement(float timeStep);

    virtual void HandleAction(int actionId);
};

#endif // CONTROLLABLE_H


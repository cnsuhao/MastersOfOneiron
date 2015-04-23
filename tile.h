#pragma once

#include "mastercontrol.h"
#include "platform.h"
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Core/CoreEvents.h>

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class Platform;

class Tile : public Object
{
    friend class Platform;
    OBJECT(Tile);
public:
    Tile(Context *context, const IntVector2 coords, Platform *platform);

    virtual void Start();
    virtual void Stop();


    IntVector2 coords_;
    BuildingType buildingType_ = B_EMPTY;
    double GetHealth(){return health_;}
    double ApplyDamage(double damage){health_ = Max(health_ - damage, 0.0);}
private:
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    MasterControl* masterControl_;
    Platform* platform_;
    Node* rootNode_;
    CollisionShape* collisionShape_;
    Node* elements_[TE_LENGTH];
    double health_ = 1.0;
    //A node pointer for each element:
    // 516 ^
    // 402 N
    // 837 |
    void SetBuilding(BuildingType type);
    BuildingType GetBuilding();
    void FixFringe();
};

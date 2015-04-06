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

class Tile : public Object
{
    OBJECT(Tile);
public:
    Tile(Context *context, const Pair<int, int> coords, Platform *platform);

    virtual void Start();
    virtual void Stop();
    enum TileElement {CENTER, NORTH, EAST, SOUTH, WEST, NORTHEAST, SOUTHEAST, SOUTHWEST, NORTHWEST, TILEELEMENT_LENGTH};

    Pair<int,int> coords_;
private:
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    MasterControl* masterControl_;
    Platform* platform_;
    Node* rootNode_;
    CollisionShape* collisionShape_;
    Node* elements_[TILEELEMENT_LENGTH];
    //A node pointer for each element:
    // 516 ^
    // 402 N
    // 837 |
};

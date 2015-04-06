#pragma once

//#include "tile.h"
//#include "slot.h"
#include "mastercontrol.h"
#include <Urho3D/Physics/RigidBody.h>
//#include <Urho3D/Input/Input.h>
#include <Urho3D/Core/CoreEvents.h>

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class Tile;
class Slot;

class Platform : public Object
{

    //friend class Tile;
    OBJECT(Platform);
public:
    Platform(Context *context, Vector3 position, MasterControl* masterControl);

    virtual void Start();
    virtual void Stop();
    MasterControl* masterControl_;
    Node* rootNode_;
    RigidBody* rigidBody_;

    void AddMissingSlots();
private:
    void HandleUpdate(StringHash eventType, VariantMap& eventData);

    HashMap<Pair<int, int>, SharedPtr<Tile>> tileMap_;
    HashMap<Pair<int, int>, SharedPtr<Slot>> slotMap_;
    //A node pointer for each element:
    // 516 ^
    // 402 N
    // 837 |
};

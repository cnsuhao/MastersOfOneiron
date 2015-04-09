#pragma once

#include "mastercontrol.h"
#include <Urho3D/Physics/RigidBody.h>
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

enum TileElement {TE_CENTER = 0, TE_NORTH, TE_EAST, TE_SOUTH, TE_WEST, TE_NORTHWEST, TE_NORTHEAST, TE_SOUTHEAST, TE_SOUTHWEST, TE_LENGTH};
enum CornerType {CT_NONE, CT_IN, CT_OUT, CT_TWEEN, CT_DOUBLE, CT_FILL};

class Platform : public Object
{
    OBJECT(Platform);
public:
    Platform(Context *context, Vector3 position, MasterControl* masterControl);

    virtual void Start();
    virtual void Stop();
    MasterControl* masterControl_;
    Node* rootNode_;
    RigidBody* rigidBody_;

    void AddMissingSlots();
    void FixFringe();
    bool CheckEmpty(IntVector2 coords, bool checkTiles) const;
private:
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    bool CheckEmptyNeighbour(IntVector2 coords, TileElement element, bool tileMap) const;
    IntVector2 GetNeighbourCoords(IntVector2 coords, TileElement element) const;
    CornerType PickCornerType(IntVector2 tileCoords, TileElement element);

    HashMap<IntVector2, SharedPtr<Tile>> tileMap_;
    HashMap<IntVector2, SharedPtr<Slot>> slotMap_;
    //A node pointer for each element:
    // 516 ^
    // 402 N
    // 837 |
};

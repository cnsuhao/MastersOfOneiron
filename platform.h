#pragma once

#include "mastercontrol.h"
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
enum BuildingType {B_SPACE, B_EMPTY, B_ENGINE};

class Platform : public Object
{
    OBJECT(Platform);
    friend class InputMaster;
public:
    Platform(Context *context, Vector3 position, MasterControl* masterControl);

    MasterControl* masterControl_;
    Node* rootNode_;
    RigidBody* rigidBody_;

    virtual void Start();
    virtual void Stop();
    void AddMissingSlots();
    void FixFringe();
    bool CheckEmpty(IntVector2 coords, bool checkTiles) const;
    bool CheckEmpty(Vector3 coords, bool checkTiles) const { CheckEmpty(IntVector2(round(coords.x_), round(coords.z_)), checkTiles); }
    void AddTile(IntVector2 newTileCoords);
    bool DisableSlot(IntVector2 coords);
    bool EnableSlot(IntVector2 coords);
private:
    HashMap<IntVector2, SharedPtr<Tile>> tileMap_;
    HashMap<IntVector2, SharedPtr<Slot>> slotMap_;
    HashMap<IntVector2, BuildingType> buildingMap_;

    bool selected_ = false;

    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    bool CheckEmptyNeighbour(IntVector2 coords, TileElement element, bool tileMap) const;
    IntVector2 GetNeighbourCoords(IntVector2 coords, TileElement element) const;
    BuildingType GetBuildingType(IntVector2 coords);
    BuildingType GetNeighbourType(IntVector2 coords, TileElement element);
    CornerType PickCornerType(IntVector2 tileCoords, TileElement element) const;

    void Select();
    void Deselect();
    void SetSelected(bool selected);
    bool IsSelected() const;

    void SetBuilding(IntVector2 coords, BuildingType type = B_ENGINE);
    void RemoveBuilding(IntVector2 coords) {SetBuilding(coords, B_EMPTY);}
    void UpdateCenterOfMass();
};

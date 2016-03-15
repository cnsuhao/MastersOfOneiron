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

#ifndef PLATFORM_H
#define PLATFORM_H

#include <Urho3D/Urho3D.h>

#include "mastercontrol.h"

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
    URHO3D_OBJECT(Platform, Object);
    friend class InputMaster;
public:
    Platform(Context *context, Vector3 position, MasterControl* masterControl, bool random = false);

    MasterControl* masterControl_;
    Node* rootNode_;
    RigidBody* rigidBody_;

    bool CheckEmpty(Vector3 coords, bool checkTiles) const { CheckEmpty(IntVector2(round(coords.x_), round(coords.z_)), checkTiles); }
    bool CheckEmpty(IntVector2 coords, bool checkTiles) const;
    bool CheckEmptyNeighbour(IntVector2 coords, TileElement element, bool tileMap) const;
    IntVector2 GetNeighbourCoords(IntVector2 coords, TileElement element) const;
    CornerType PickCornerType(IntVector2 tileCoords, TileElement element) const;
    BuildingType GetBuildingType(IntVector2 coords);
    BuildingType GetNeighbourType(IntVector2 coords, TileElement element);


    virtual void Start();
    virtual void Stop();
    void AddMissingSlots();
    void FixFringe();
    void FixFringe(IntVector2 coords);

    void AddTile(IntVector2 newTileCoords);
    bool DisableSlot(IntVector2 coords);
    bool EnableSlot(IntVector2 coords);
    void SetMoveTarget(Vector3 moveTarget) {moveTarget_ = moveTarget;}
    void EnableSlots();
    void DisableSlots();
private:
    HashMap<IntVector2, SharedPtr<Tile> > tileMap_;
    HashMap<IntVector2, SharedPtr<Slot> > slotMap_;
    HashMap<IntVector2, BuildingType> buildingMap_;

    bool selected_ = false;
    Vector3 moveTarget_;

    void HandleUpdate(StringHash eventType, VariantMap& eventData);



    void Select();
    void Deselect();
    void SetSelected(bool selected);
    bool IsSelected() const;

    void SetBuilding(IntVector2 coords, BuildingType type = B_ENGINE);
    void RemoveBuilding(IntVector2 coords) {SetBuilding(coords, B_EMPTY);}
    void UpdateCenterOfMass();
    void Move(double timeStep);
};

#endif // PLATFORM_H

/* Masters of Oneiron
// Copyright (C) 2016 LucKey Productions (luckeyproductions.nl)
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

#include "sceneobject.h"

#define PLATFORM_HALF_THICKNESS 0.23f

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class Tile;
class Slot;

enum TileElement {TE_NORTHEAST = 0, TE_EAST, TE_SOUTHEAST, TE_NORTHWEST, TE_WEST, TE_SOUTHWEST, TE_LENGTH};
enum Neighbour{ NB_NORTH = 0, NB_NORTHEAST, NB_SOUTHEAST, NB_SOUTH, NB_SOUTHWEST, NB_NORTHWEST, NB_LENGTH };
enum CornerType {CT_NONE, CT_OUT, CT_INA, CT_INB, CT_STRAIGHTA, CT_STRAIGHTB, CT_FILL};
enum BuildingType {B_SPACE, B_EMPTY, B_ENGINE};


class Platform : public SceneObject
{
    URHO3D_OBJECT(Platform, SceneObject);
    friend class InputMaster;
public:
    Platform(Context *context);
    static void RegisterObject(Context* context);
    virtual void OnNodeSet(Node* node);
    virtual void Set(Vector3 position);

    static int platformCount_;
    RigidBody* rigidBody_;

    bool CheckEmpty(Vector3 coords, bool checkTiles = true) const { return CheckEmpty(IntVector2(round(coords.x_), round(coords.z_)), checkTiles); }
    bool CheckEmpty(IntVector2 coords, bool checkTiles = true) const;
    bool CheckEmptyNeighbour(IntVector2 coords, Neighbour neighbour, bool checkTiles = true) const;
    static IntVector2 GetNeighbourCoords(IntVector2 coords, Neighbour element);
    CornerType PickCornerType(IntVector2 tileCoords, TileElement element) const;
    BuildingType GetBuildingType(IntVector2 coords);
    BuildingType GetNeighbourType(IntVector2 coords, Neighbour neighbour);


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

    static Vector3 CoordsToPosition(IntVector2 coords, float y = 0.0f) { return Vector3(coords.x_,
                                                                                        y,
                                                                                       (Abs(coords.x_) % 2) + coords.y_ * 2.0f);
                                                                       }
    char GetNeighbourMask(IntVector2 tileCoords, TileElement element) const;

private:
    HashMap<IntVector2, Tile*> tileMap_;
    HashMap<IntVector2, Slot*> slotMap_;
    HashMap<IntVector2, BuildingType> buildingMap_;

    bool selected_;
    Vector3 moveTarget_;

    void Update(float timeStep);



    void Select();
    void Deselect();
    void SetSelected(bool select);
    bool IsSelected() const;

    void SetBuilding(IntVector2 coords, BuildingType type = B_ENGINE);
    void RemoveBuilding(IntVector2 coords) {SetBuilding(coords, B_EMPTY);}
    void UpdateCenterOfMass();
    void Move(double timeStep);
};

#endif // PLATFORM_H

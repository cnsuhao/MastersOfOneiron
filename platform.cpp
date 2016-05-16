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

#include "platform.h"
#include "tile.h"
#include "slot.h"

namespace Urho3D {
template <> unsigned MakeHash(const IntVector2& value)
  {
    return LucKey::IntVector2ToHash(value);
  }
}

Platform::Platform(Context *context, Vector3 position, MasterControl* masterControl, bool random):
    Object(context),
    masterControl_{masterControl}
{
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Platform, HandleUpdate));
    rootNode_ = masterControl_->world.scene->CreateChild("Platform");
    masterControl_->platformMap_[rootNode_->GetID()] = WeakPtr<Platform>(this);

    rootNode_->SetPosition(position);
    SetMoveTarget(position);
    //rootNode_->SetRotation(Quaternion(0.0f, Random(360.0f), 0.0f));
    rigidBody_ = rootNode_->CreateComponent<RigidBody>();
    //rigidBody_->SetRotation(rootNode_->GetRotation());
    rigidBody_->SetMass(1.0);
    rigidBody_->SetLinearDamping(0.369);
    rigidBody_->SetAngularDamping(0.1);
    rigidBody_->SetLinearRestThreshold(0.1f);
    rigidBody_->SetAngularRestThreshold(0.01f);
    rigidBody_->SetLinearFactor(Vector3(1.0f, 0.0f, 1.0f));
    rigidBody_->SetAngularFactor(Vector3(0.0f, 1.0f, 0.0f));

    // Add base tile
    IntVector2 firstCoordPair{IntVector2(0,0)};
    tileMap_[firstCoordPair] = new Tile(context_, firstCoordPair, this);
    // Add random tiles
    if (random){
        rootNode_->Rotate(Quaternion(Random(360.0f), Vector3::UP));

        bool symmetrical{static_cast<bool>(Random(2))};
        int addedTiles{1};
        int platformSize{Random(5, 88)};

        while (addedTiles < platformSize) {
            //Pick a random exsisting tile from a list.
            Vector<IntVector2> coordsVector{tileMap_.Keys()};
            IntVector2 randomTileCoords{coordsVector[Random((int)coordsVector.Size())]};

            char startDir{static_cast<char>(Random(0,4))};
            for (int direction{startDir}; direction < startDir+3; ++direction) {
                int cycledDir{LucKey::Cycle(direction, 1, 4)};
                assert(cycledDir > 0 && cycledDir < 5);
                TileElement tileElement{static_cast<TileElement>(cycledDir)};
                if (CheckEmptyNeighbour(randomTileCoords, tileElement, true)) {
                    IntVector2 newTileCoords{GetNeighbourCoords(randomTileCoords, tileElement)};
                    AddTile(newTileCoords);
                    addedTiles++;
                    if (newTileCoords.x_ != 0 && symmetrical) {
                        newTileCoords = IntVector2(-newTileCoords.x_, newTileCoords.y_);
                        AddTile(newTileCoords);
//                        tileMap_[newTileCoords] = new Tile(context_, newTileCoords, this);
                        addedTiles++;
                    }
                }
            }
        }
    }

    //Add slots
    AddMissingSlots();
    DisableSlots();
    FixFringe();

//    rigidBody_->ApplyForce(Vector3(Random(-100.0f,100.0f), 0.0f, Random(-100.0f,100.0f)));
//    rigidBody_->ApplyTorque(Vector3(0.0f, Random(-16.0f, 16.0f), 0.0f));

}


void Platform::Start()
{
}

void Platform::Stop()
{
}

bool Platform::EnableSlot(IntVector2 coords)
{
    slotMap_[coords]->rootNode_->SetEnabled(true);
}
void Platform::EnableSlots()
{
    for (unsigned i{0}; i < slotMap_.Values().Size(); ++i) {
        if (GetBuildingType(slotMap_.Values()[i]->coords_) <= B_EMPTY)
            EnableSlot(slotMap_.Values()[i]->coords_);
    }
}

bool Platform::DisableSlot(IntVector2 coords)
{
    slotMap_[coords]->rootNode_->SetEnabled(false);
}
void Platform::DisableSlots()
{
    for (unsigned i{0}; i < slotMap_.Values().Size(); ++i) {
        DisableSlot(slotMap_.Values()[i]->coords_);
    }
}

void Platform::Select()
{
    selected_ = true;
    EnableSlots();
}

void Platform::Deselect()
{
    selected_ = false;
    DisableSlots();
}

void Platform::SetSelected(bool selected)
{
    if (selected == true) Select();
    if (selected == false) Deselect();
}

bool Platform::IsSelected() const
{
    return selected_;
}



void Platform::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    double timeStep{eventData[Update::P_TIMESTEP].GetFloat()};
    if (moveTarget_ != rootNode_->GetPosition()) Move(100.0f * timeStep);
}

void Platform::Move(double timeStep)
{
    /*Vector3 relativeMoveTarget = moveTarget_ - rootNode_->GetPosition();
    if (relativeMoveTarget.Length() > 1.0){
        rigidBody_->ApplyForce(10.0f*relativeMoveTarget.Normalized()*timeStep);
        rigidBody_->ApplyTorque(Vector3(0.0f, rootNode_->GetDirection().Angle(relativeMoveTarget)*timeStep*0.1 - pow(rigidBody_->GetAngularVelocity().y_, 3.0f), 0.0f));
    }*/
}

void Platform::AddTile(IntVector2 newTileCoords)
{
    tileMap_[newTileCoords] = new Tile(context_, newTileCoords, this);
}

void Platform::AddMissingSlots()
{
    Vector<IntVector2> tileCoords{tileMap_.Keys()};
    for (uint nthTile{0}; nthTile < tileCoords.Size(); ++nthTile){
        for (int element{0}; element <= 4; ++element){
            IntVector2 checkCoords{GetNeighbourCoords(tileCoords[nthTile], (TileElement)element)};
            if (CheckEmpty(checkCoords, false))
                slotMap_[checkCoords] = new Slot(context_, this, checkCoords);
        }
    }
}

void Platform::FixFringe()
{
    Vector<SharedPtr<Tile> > tiles{tileMap_.Values()};
    for (unsigned tile{0}; tile < tiles.Size(); ++tile) {
        tiles[tile]->FixFringe();
    }
}

void Platform::FixFringe(IntVector2 coords)
{
    for (int coordsOffset{0}; coordsOffset < TE_LENGTH; ++coordsOffset) {
        IntVector2 neighbourCoords{GetNeighbourCoords(coords, (TileElement)coordsOffset)};
        if (!CheckEmpty(neighbourCoords, true))
            tileMap_[neighbourCoords]->FixFringe();
    }
}

void Platform::SetBuilding(IntVector2 coords, BuildingType type)
{
    tileMap_[coords]->SetBuilding(type);
    FixFringe(coords);
}

bool Platform::CheckEmpty(IntVector2 coords, bool checkTiles = true) const
{
    if (checkTiles)
        return (!tileMap_.Keys().Contains(coords));
    else
        return (!slotMap_.Keys().Contains(coords));
}


bool Platform::CheckEmptyNeighbour(IntVector2 coords, TileElement element, bool checkTiles = true) const
{
    return CheckEmpty(GetNeighbourCoords(coords, element), checkTiles);
}



IntVector2 Platform::GetNeighbourCoords(IntVector2 coords, TileElement element) const
{
    IntVector2 shift{IntVector2::ZERO};
    switch (element) {
    case TE_NORTH: shift.y_ =  1; break;
    case TE_EAST:  shift.x_ =  1; break;
    case TE_SOUTH: shift.y_ = -1; break;
    case TE_WEST:  shift.x_ = -1; break;
    case TE_NORTHEAST: shift.x_ =  1; shift.y_ =  1; break;
    case TE_SOUTHEAST: shift.x_ =  1; shift.y_ = -1; break;
    case TE_SOUTHWEST: shift.x_ = -1; shift.y_ = -1; break;
    case TE_NORTHWEST: shift.x_ = -1; shift.y_ =  1; break;
    default: case TE_CENTER: break;
    }
    return coords + shift;
}

BuildingType Platform::GetBuildingType(IntVector2 coords)
{
    if (!CheckEmpty(coords)) {
        return tileMap_[coords]->buildingType_;
    } else {
        return B_SPACE;
    }
}

BuildingType Platform::GetNeighbourType(IntVector2 coords, TileElement element)
{
    return GetBuildingType(GetNeighbourCoords(coords, element));
}

CornerType Platform::PickCornerType(IntVector2 tileCoords, TileElement element) const
{
    bool emptyCheck[3]{false, false, false};
    switch (element) {
    case TE_NORTHEAST: {
        emptyCheck[0] = CheckEmptyNeighbour(tileCoords, TE_NORTH);
        emptyCheck[1] = CheckEmptyNeighbour(tileCoords, TE_NORTHEAST);
        emptyCheck[2] = CheckEmptyNeighbour(tileCoords, TE_EAST);
    }
    break;
    case TE_SOUTHEAST: {
        emptyCheck[0] = CheckEmptyNeighbour(tileCoords, TE_EAST);
        emptyCheck[1] = CheckEmptyNeighbour(tileCoords, TE_SOUTHEAST);
        emptyCheck[2] = CheckEmptyNeighbour(tileCoords, TE_SOUTH);
    }break;
    case TE_SOUTHWEST: {
        emptyCheck[0] = CheckEmptyNeighbour(tileCoords, TE_SOUTH);
        emptyCheck[1] = CheckEmptyNeighbour(tileCoords, TE_SOUTHWEST);
        emptyCheck[2] = CheckEmptyNeighbour(tileCoords, TE_WEST);
    }break;
    case TE_NORTHWEST: {
        emptyCheck[0] = CheckEmptyNeighbour(tileCoords, TE_WEST);
        emptyCheck[1] = CheckEmptyNeighbour(tileCoords, TE_NORTHWEST);
        emptyCheck[2] = CheckEmptyNeighbour(tileCoords, TE_NORTH);
    }break;
    default: break;
    }

    int neighbourMask{0};
    for (int i{2}; i >= 0; --i) {
        neighbourMask += !emptyCheck[i] << i;
    }
    switch (neighbourMask){
    case 0: return CT_OUT; break;
    case 1: return CT_TWEEN; break;
    case 2: return CT_DOUBLE; break;
    case 3: return CT_NONE; break;
    case 4: return CT_NONE; break;
    case 5: return CT_IN; break;
    case 6: return CT_NONE; break;
    case 7: return (element == TE_SOUTHEAST) ? CT_FILL : CT_NONE; break;
    default: return CT_NONE; break;
    }
}

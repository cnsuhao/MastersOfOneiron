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

#include "platform.h"
#include "tile.h"
#include "slot.h"

namespace Urho3D {
template <> unsigned MakeHash(const IntVector2& value)
  {
    return LucKey::IntVector2ToHash(value);
  }
}

void Platform::RegisterObject(Context *context)
{
    context->RegisterFactory<Platform>();
}

int Platform::platformCount_{};

Platform::Platform(Context *context):
    SceneObject(context),
    selected_{false}
{
    ++platformCount_;
}

void Platform::OnNodeSet(Node *node)
{ if (!node) return;

    MC->platformMap_[node_->GetID()] = this;


    node_->LookAt(Quaternion(Random(360.0f), node_->GetWorldPosition().Normalized()) * node_->GetWorldPosition(), Vector3::UP);

//    SetMoveTarget(position);
    //rootNode_->SetRotation(Quaternion(0.0f, Random(360.0f), 0.0f));
    rigidBody_ = node_->CreateComponent<RigidBody>();
//    rigidBody_->SetRotation(Quaternion(180.0f, Vector3::RIGHT));
    //rigidBody_->SetRotation(rootNode_->GetRotation());
    rigidBody_->SetMass(1.0f);
    rigidBody_->SetFriction(0.0f);
    rigidBody_->SetLinearDamping(0.9f);
    rigidBody_->SetAngularDamping(0.5f);
    rigidBody_->SetLinearRestThreshold(0.1f);
    rigidBody_->SetAngularRestThreshold(0.01f);
//    rigidBody_->SetLinearFactor(Vector3(1.0f, 0.0f, 1.0f));
    rigidBody_->SetAngularFactor(Vector3(0.0f, 1.0f, 0.0f));
    rigidBody_->SetUseGravity(false);


    // Add base tile
    IntVector2 firstCoordPair{ IntVector2(0,0) };
    AddTile(firstCoordPair);
    // Add random tiles
//    if (random){

        bool symmetrical{ true };//static_cast<bool>(Random(2))};
        int addedTiles{ 1 };
        int platformSize{Random(1, 42)};

        while (addedTiles < platformSize) {
            //Pick a random exsisting tile
            Vector<IntVector2> coordsVector{tileMap_.Keys()};
            IntVector2 randomTileCoords{coordsVector[Random((int)coordsVector.Size())]};

            char startDir{static_cast<char>(Random(NB_WEST,NB_LENGTH))};
            for (int direction{startDir}; direction < startDir+3; ++direction) {

                int cycledDir{LucKey::Cycle(direction, NB_NORTHWEST, NB_SOUTHWEST)};
                assert((cycledDir > NB_WEST) && (cycledDir < NB_LENGTH));
                Neighbour neighbour{ static_cast<Neighbour>(cycledDir) };
                if (CheckEmptyNeighbour(randomTileCoords, neighbour, true)) {

                    IntVector2 newTileCoords{ GetNeighbourCoords(randomTileCoords, neighbour) };
                    AddTile(newTileCoords);
                    ++addedTiles;
                    if (newTileCoords.x_ != newTileCoords.y_ && symmetrical) {

                        newTileCoords = IntVector2(newTileCoords.y_, newTileCoords.x_);
                        AddTile(newTileCoords);
                        ++addedTiles;
                    }
                }
            }
        }
//    }

    //Add slots
    AddMissingSlots();
    DisableSlots();
    FixFringe();

//    rigidBody_->ApplyForce(Vector3(Random(-100.0f,100.0f), 0.0f, Random(-100.0f,100.0f)));
//    rigidBody_->ApplyTorque(Vector3(0.0f, Random(-16.0f, 16.0f), 0.0f));

}

void Platform::Set(Vector3 position)
{
    Vector3 platformPos{ position.Normalized() * WORLD_RADIUS };
    SceneObject::Set(platformPos);
//    node_->LookAt(Vector3::ZERO, Vector3::DOWN);

    Constraint* worldConstraint{ node_->GetOrCreateComponent<Constraint>() };
    worldConstraint->SetConstraintType(CONSTRAINT_POINT);
    worldConstraint->SetOtherBody(MC->world.sunNode->GetComponent<RigidBody>());
    worldConstraint->SetPosition(-platformPos);
//    worldConstraint->SetOtherPosition(Vector3::ZERO);


    node_->Rotate(Quaternion(Random(360.0f), Vector3::UP));


}

void Platform::Start()
{
}

void Platform::Stop()
{
}

bool Platform::EnableSlot(IntVector2 coords)
{
    slotMap_[coords]->GetNode()->SetEnabled(true);
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
    slotMap_[coords]->GetNode()->SetEnabled(false);
}
void Platform::DisableSlots()
{
    for (unsigned i{0}; i < slotMap_.Values().Size(); ++i) {
        DisableSlot(slotMap_.Values()[i]->coords_);
    }
}

void Platform::Select()
{
    if (selected_)
        return;

    selected_ = true;
    EnableSlots();
}

void Platform::Deselect()
{
    if (!selected_)
        return;

    selected_ = false;
    DisableSlots();
}

void Platform::SetSelected(bool select)
{
    if (select == true) Select();
    if (select == false) Deselect();
}

bool Platform::IsSelected() const
{
    return selected_;
}



void Platform::Update(float timeStep)
{

    node_->GetComponent<Constraint>()->SetAxis(-node_->GetWorldPosition().Normalized());
//    rigidBody_->SetGravityOverride(23.0f * node_->GetWorldPosition().Length() > WORLDRADIUS
//                                   ? -node_->GetWorldPosition().Normalized()
//                                   :  node_->GetWorldPosition().Normalized());
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
    Tile* newTile{ SPAWN->Create<Tile>() };
    newTile->Set(newTileCoords, this);
    tileMap_[newTileCoords] = newTile;
}

void Platform::AddMissingSlots()
{
    Vector<IntVector2> tileCoords{tileMap_.Keys()};
    for (uint nthTile{0}; nthTile < tileCoords.Size(); ++nthTile){
        for (int neighbour{0}; neighbour < TE_LENGTH; ++neighbour){
            IntVector2 checkCoords{ GetNeighbourCoords(tileCoords[nthTile], static_cast<Neighbour>(neighbour)) };
            if (CheckEmpty(checkCoords, false)) {
                Slot* newSlot{ SPAWN->Create<Slot>() };
                slotMap_[checkCoords] = newSlot;
                newSlot->Set(checkCoords, this);
            }
        }
    }
}

void Platform::FixFringe()
{
    Vector<Tile*> tiles{ tileMap_.Values() };
    for (unsigned tile{0}; tile < tiles.Size(); ++tile) {
        tiles[tile]->FixFringe();
    }
}

void Platform::FixFringe(IntVector2 coords)
{
    for (int coordsOffset{0}; coordsOffset < TE_LENGTH; ++coordsOffset) {
        IntVector2 neighbourCoords{ GetNeighbourCoords(coords, static_cast<Neighbour>(coordsOffset)) };
        if (!CheckEmpty(neighbourCoords, true))
            tileMap_[neighbourCoords]->FixFringe();
    }
}

void Platform::SetBuilding(IntVector2 coords, BuildingType type)
{
    tileMap_[coords]->SetBuilding(type);
    FixFringe(coords);
}

bool Platform::CheckEmpty(IntVector2 coords, bool checkTiles) const
{
    if (checkTiles)
        return (!tileMap_.Keys().Contains(coords));
    else
        return (!slotMap_.Keys().Contains(coords));
}


bool Platform::CheckEmptyNeighbour(IntVector2 coords, Neighbour neighbour, bool checkTiles) const
{
    return CheckEmpty(GetNeighbourCoords(coords, neighbour), checkTiles);
}



IntVector2 Platform::GetNeighbourCoords(IntVector2 coords, Neighbour element) const
{
    IntVector2 shift{IntVector2::ZERO};
    switch (element) {
    case NB_NORTH:
        shift.x_ = shift.y_ = 1;
        break;
    case NB_EAST:
        shift.x_ = -1; shift.y_ = 1;
        break;
    case NB_SOUTH:
        shift.x_ = shift.y_ = -1;
        break;
    case NB_WEST:
        shift.x_ = 1; shift.y_ = -1;
        break;
    case NB_NORTHEAST:
        shift.y_ =  1;
        break;
    case NB_SOUTHEAST:
        shift.x_ = -1;
        break;
    case NB_SOUTHWEST:
        shift.y_ = -1;
        break;
    case NB_NORTHWEST:
        shift.x_ = 1;
        break;
    default: break;
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

BuildingType Platform::GetNeighbourType(IntVector2 coords, Neighbour neighbour)
{
    return GetBuildingType(GetNeighbourCoords(coords, neighbour));
}

CornerType Platform::PickCornerType(IntVector2 tileCoords, TileElement element) const
{
    char neighbourMask{ GetNeighbourMask(tileCoords, element) };

    switch (neighbourMask) {

    case 0: case 2:
        return CT_OUT;

    case 4: case 6:
        return element == TE_NORTH || element == TE_SOUTH ? CT_EDGE_A : CT_EDGE_B;

    case 1: case 3:
        return element == TE_NORTH || element == TE_SOUTH  ? CT_EDGE_B : CT_EDGE_A;

    case 5:
        return CT_IN;

    case 7:
        return CT_FILL;

    default:
        return CT_NONE;
    }
}
char Platform::GetNeighbourMask(IntVector2 tileCoords, TileElement element) const
{
    bool emptyCheck[3]{ true, true, true };

    switch (element) {
    case TE_NORTH: {
        emptyCheck[0] = CheckEmptyNeighbour(tileCoords, NB_NORTHWEST);
        emptyCheck[1] = CheckEmptyNeighbour(tileCoords, NB_NORTH);
        emptyCheck[2] = CheckEmptyNeighbour(tileCoords, NB_NORTHEAST);
    }
    break;
    case TE_EAST: {
        emptyCheck[0] = CheckEmptyNeighbour(tileCoords, NB_NORTHEAST);
        emptyCheck[1] = CheckEmptyNeighbour(tileCoords, NB_EAST);
        emptyCheck[2] = CheckEmptyNeighbour(tileCoords, NB_SOUTHEAST);
    }break;
    case TE_SOUTH: {
        emptyCheck[0] = CheckEmptyNeighbour(tileCoords, NB_SOUTHEAST);
        emptyCheck[1] = CheckEmptyNeighbour(tileCoords, NB_SOUTH);
        emptyCheck[2] = CheckEmptyNeighbour(tileCoords, NB_SOUTHWEST);
    }break;
    case TE_WEST: {
        emptyCheck[0] = CheckEmptyNeighbour(tileCoords, NB_SOUTHWEST);
        emptyCheck[1] = CheckEmptyNeighbour(tileCoords, NB_WEST);
        emptyCheck[2] = CheckEmptyNeighbour(tileCoords, NB_NORTHWEST);
    }break;
    default: break;
    }

    int neighbourMask{0};

    for (int i{2}; i >= 0; --i) {
        neighbourMask += !emptyCheck[i] << i;
    }

    return neighbourMask;
}

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
#include "world.h"

namespace Urho3D {
template <> unsigned MakeHash(const IntVector2& value)
  {
    return LucKey::IntVector2ToHash(value);
  }
}

//HashMap<StringHash, StaticModelGroup*> Platform::modelGroups_{};

void Platform::RegisterObject(Context *context)
{
    context->RegisterFactory<Platform>();
}

int Platform::platformCount_{};

Platform::Platform(Context *context):
    SceneObject(context),
    selected_{false},
    modelGroups_{},
    slotGroup_{}
{
    ++platformCount_;
}

void Platform::OnNodeSet(Node *node)
{ if (!node) return;

    node_->AddTag("Platform");

    MC->platformMap_[node_->GetID()] = this;

//    node_->LookAt(Quaternion(Random(360.0f), node_->GetWorldPosition().Normalized()) * node_->GetWorldPosition(), Vector3::ZERO);

//    SetMoveTarget(position);
    //rootNode_->SetRotation(Quaternion(0.0f, Random(360.0f), 0.0f));
    rigidBody_ = node_->CreateComponent<RigidBody>();
//    rigidBody_->SetRotation(Quaternion(180.0f, Vector3::RIGHT));
    //rigidBody_->SetRotation(rootNode_->GetRotation());
    rigidBody_->SetMass(1.0f);
    rigidBody_->SetFriction(0.0f);
    rigidBody_->SetLinearDamping(0.95f);
    rigidBody_->SetAngularDamping(0.75f);
    rigidBody_->SetLinearRestThreshold(0.001f);
    rigidBody_->SetAngularRestThreshold(0.001f);
//    rigidBody_->SetLinearFactor(Vector3(1.0f, 0.0f, 1.0f));
    rigidBody_->SetAngularFactor(Vector3(0.0f, 1.0f, 0.0f));
//    rigidBody_->SetUseGravity(false);

    Realign(1.0f);


    if (!slotGroup_) {
        slotGroup_ = node_->CreateComponent<StaticModelGroup>();
        slotGroup_->SetModel(RESOURCE->GetModel("Slot"));
        slotGroup_->SetMaterial(RESOURCE->GetMaterial("Glow"));
        slotGroup_->SetCastShadows(true);
    }

    // Add base tile
    IntVector2 firstCoordPair{ IntVector2(0,0) };
    AddTile(firstCoordPair);
    // Add random tiles

    bool symmetrical{ true };//static_cast<bool>(Random(2))};
    int addedTiles{ 1 };
    int platformSize{ Ceil(Random(1, 64) * Pow(Random(0.5, 1.5f), 2.0f)) };

    while (addedTiles < platformSize) {
        //Pick a random exsisting tile
        Vector<IntVector2> coordsVector{ tileMap_.Keys() };
        IntVector2 randomTileCoords{ coordsVector[Random((int)coordsVector.Size())] };

        if (randomTileCoords.x_ < 0 && symmetrical)
            randomTileCoords.x_ = -randomTileCoords.x_;

        char startDir{ NB_NORTH };//static_cast<char>(Random(NB_LENGTH))};
        bool clockWise{ Random(2) };
        if (clockWise)
            startDir -= 3;

        for (int direction{startDir}; LucKey::Delta(direction, startDir) < NB_LENGTH; clockWise ? direction += 2 : direction -= 2) {

            int cycledDir{ LucKey::Cycle(direction, NB_NORTH, NB_NORTHWEST) };
            assert((cycledDir >= NB_NORTH) && (cycledDir < NB_LENGTH));
            Neighbour neighbour{ static_cast<Neighbour>(cycledDir) };
            if (CheckEmptyNeighbour(randomTileCoords, neighbour, true)) {

                IntVector2 newTileCoords{ GetNeighbourCoords(randomTileCoords, neighbour) };

                Tile* newTile{ AddTile(newTileCoords) };
                ++addedTiles;
                if (symmetrical) {
                    if (Abs(newTileCoords.x_) % 2 == 1 && newTileCoords.y_ <= 0){
                        newTile->SetBuilding(B_ENGINE);
                    }
                    if (newTileCoords.x_ != 0) {

                        newTileCoords = IntVector2(-newTileCoords.x_, newTileCoords.y_);
                        newTile = AddTile(newTileCoords);
                        ++addedTiles;

                        if (Abs(newTileCoords.x_) % 2 == 1 && newTileCoords.y_ <= 0){
                            newTile->SetBuilding(B_ENGINE);
                        }
                    }
                }
            }
        }
    }

    //Add slots
    AddMissingSlots();
    Deselect();
    DisableSlots();
    FixFringe();

}

void Platform::Set(Vector3 position)
{
//    Vector3 platformPos{ position.Normalized() * WORLD_RADIUS };
    SceneObject::Set(GetScene()->GetComponent<World>()->ToSurface(position));
//    node_->LookAt(Vector3::ZERO, Vector3::DOWN);

//    Constraint* worldConstraint{ node_->GetOrCreateComponent<Constraint>() };
//    worldConstraint->SetConstraintType(CONSTRAINT_POINT);
//    worldConstraint->SetOtherBody(MC->world.sunNode->GetComponent<RigidBody>());
//    worldConstraint->SetPosition(-platformPos);
//    worldConstraint->SetOtherPosition();

    Realign(1.0f);
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
    slotGroup_->AddInstanceNode(slotMap_[coords]->GetNode());
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
    if (slotMap_[coords])
        slotGroup_->RemoveInstanceNode(slotMap_[coords]->GetNode());
}
void Platform::DisableSlots()
{
    for (Slot* s : slotMap_.Values()) {
        if (s)
            DisableSlot(s->coords_);
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

void Platform::Realign(float timeStep)
{
    World* world{ GetScene()->GetComponent<World>() };
    Vector3 up{ -world->GetNearestRhombicCenter(node_->GetWorldPosition()).Normalized() };
    up = node_->GetUp().Lerp(up, Min(1.0f, 5.0f * timeStep));
    Vector3 newDirection{ node_->GetDirection() - node_->GetDirection().DotProduct(up) * up };
    newDirection = node_->GetDirection().Lerp(newDirection, Min(1.0f, 2.0f * timeStep));

    node_->LookAt(world->ToSurface(node_->GetWorldPosition() + newDirection * 3.0f), up);

    rigidBody_->SetAngularVelocity(node_->GetWorldPosition().Normalized() * rigidBody_->GetAngularVelocity().ProjectOntoAxis(node_->GetWorldPosition()));
}

void Platform::Update(float timeStep)
{
    Vector3 rhombicCenter{ GetScene()->GetComponent<World>()->GetNearestRhombicCenter(node_->GetWorldPosition()) };

//    node_->GetComponent<Constraint>()->SetAxis(-node_->GetWorldPosition().Normalized());

    float out{ (node_->GetWorldPosition() - rhombicCenter).ProjectOntoAxis(rhombicCenter) };

    Vector3 newG{ -23.0f * out * Sign(out) * out * rhombicCenter.Normalized() };
    rigidBody_->SetGravityOverride(newG);

//    node_->SetWorldPosition(node_->GetWorldPosition().Normalized() * WORLD_RADIUS);
//rig
    if (rigidBody_->IsActive())
        Realign(timeStep);
}

void Platform::AddNodeInstance(String model, Node* node)
{
    StringHash modelHash{ model.ToHash() };

    if (!modelGroups_.Contains(modelHash)) {
        modelGroups_[modelHash] = node_->CreateComponent<StaticModelGroup>();
        modelGroups_[modelHash]->SetModel(RESOURCE->GetModel(model));
        modelGroups_[modelHash]->SetMaterial(RESOURCE->GetMaterial("VCol"));
        modelGroups_[modelHash]->SetCastShadows(true);
    }

    modelGroups_[modelHash]->AddInstanceNode(node);
}
void Platform::Move(double timeStep)
{
    /*Vector3 relativeMoveTarget = moveTarget_ - rootNode_->GetPosition();
    if (relativeMoveTarget.Length() > 1.0){
        rigidBody_->ApplyForce(10.0f*relativeMoveTarget.Normalized()*timeStep);
        rigidBody_->ApplyTorque(Vector3(0.0f, rootNode_->GetDirection().Angle(relativeMoveTarget)*timeStep*0.1 - pow(rigidBody_->GetAngularVelocity().y_, 3.0f), 0.0f));
    }*/
}

Tile* Platform::AddTile(IntVector2 newTileCoords)
{
    Tile* newTile{ SPAWN->Create<Tile>() };
    newTile->Set(newTileCoords, this);
    tileMap_[newTileCoords] = newTile;

    return newTile;
}

void Platform::AddMissingSlots()
{
    Vector<IntVector2> tileCoords{tileMap_.Keys()};
    for (uint nthTile{0}; nthTile < tileCoords.Size(); ++nthTile){
        for (int neighbour{ NB_NORTH }; neighbour < NB_LENGTH; ++neighbour){
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
    for (int neighbour{0}; neighbour < NB_LENGTH; ++neighbour) {
        IntVector2 neighbourCoords{ GetNeighbourCoords(coords, static_cast<Neighbour>(neighbour)) };
        if (!CheckEmpty(neighbourCoords))
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



IntVector2 Platform::GetNeighbourCoords(IntVector2 coords, Neighbour element)
{
    bool oddColumn{ coords.x_ % 2 };
    IntVector2 shift{ IntVector2::ZERO };

    switch (element) {
    case NB_NORTH:
        shift.y_ = 1;
        break;
    case NB_NORTHEAST:
        shift.x_ = 1;
//        if (oddColumn)
                shift.y_ = 1;
        break;
    case NB_EAST:
        shift.x_ = 1;
        break;
    case NB_SOUTHEAST:
        shift.x_ = 1;
//        if (!oddColumn)
                shift.y_ = -1;
        break;
    case NB_SOUTH:
        shift.y_ = -1;
        break;
    case NB_SOUTHWEST:
        shift.x_ = -1;
//        if (!oddColumn)
                shift.y_ = -1;
        break;
    case NB_WEST:
        shift.x_ = -1;
        break;
    case NB_NORTHWEST:
        shift.x_ = -1;
//        if (oddColumn)
                shift.y_ = 1;
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

    case 0:
        return CT_IN;

    case 1:
        return CT_STRAIGHT;

    case 2:
        if (element == TE_NORTHEAST || element == TE_SOUTHEAST)
            return CT_BRIDGE;

        else return CT_NONE;

    case 5:
        return CT_OUT;

    case 7:
        return CT_FILL;

    default:
        return CT_NONE;
    }
}
char Platform::GetNeighbourMask(IntVector2 tileCoords, TileElement element) const
{
    int checkedNeighbours{ 3 };
    bool emptyCheck[checkedNeighbours]{ true, true, true };

    switch (element) {
    case TE_NORTHEAST: {
        emptyCheck[0] = CheckEmptyNeighbour(tileCoords, NB_NORTH);
        emptyCheck[1] = CheckEmptyNeighbour(tileCoords, NB_NORTHEAST);
        emptyCheck[2] = CheckEmptyNeighbour(tileCoords, NB_EAST);

    }
    break;
    case TE_SOUTHEAST: {
        emptyCheck[0] = CheckEmptyNeighbour(tileCoords, NB_EAST);
        emptyCheck[1] = CheckEmptyNeighbour(tileCoords, NB_SOUTHEAST);
        emptyCheck[2] = CheckEmptyNeighbour(tileCoords, NB_SOUTH);

    }break;
    case TE_SOUTHWEST: {
        emptyCheck[0] = CheckEmptyNeighbour(tileCoords, NB_SOUTH);
        emptyCheck[1] = CheckEmptyNeighbour(tileCoords, NB_SOUTHWEST);
        emptyCheck[2] = CheckEmptyNeighbour(tileCoords, NB_WEST);

    }break;
    case TE_NORTHWEST: {
        emptyCheck[0] = CheckEmptyNeighbour(tileCoords, NB_WEST);
        emptyCheck[1] = CheckEmptyNeighbour(tileCoords, NB_NORTHWEST);
        emptyCheck[2] = CheckEmptyNeighbour(tileCoords, NB_NORTH);

    }break;
    default: break;
    }

    int neighbourMask{0};

    for (int i{ checkedNeighbours - 1 }; i >= 0; --i) {
        neighbourMask += !emptyCheck[i] << i;
    }

    return neighbourMask;
}

#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Math/Vector2.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>

#include "platform.h"
#include "tile.h"
#include "slot.h"

//Todo:
//- boxObject->SetOccluder(true); ???

Platform::Platform(Context *context, Vector3 position, MasterControl* masterControl):
Object(context)
{
    masterControl_ = masterControl;
    SubscribeToEvent(E_UPDATE, HANDLER(Platform, HandleUpdate));
    rootNode_ = masterControl_->world.scene->CreateChild("Platform");
    masterControl_->platformMap_[rootNode_->GetID()] = SharedPtr<Platform>(this);

    rootNode_->SetPosition(position);
    //rootNode_->SetRotation(Quaternion(0.0f, Random(360.0f), 0.0f));
    rigidBody_ = rootNode_->CreateComponent<RigidBody>();
    //rigidBody_->SetRotation(rootNode_->GetRotation());
    rigidBody_->SetMass(1.0f);
    rigidBody_->SetFriction(0.0f);
    //rigidBody_->SetRestitution();
    rigidBody_->SetLinearRestThreshold(0.1f);
    rigidBody_->SetAngularRestThreshold(0.01f);
    rigidBody_->SetLinearFactor(Vector3(1.0f, 0.0f, 1.0f));
    rigidBody_->SetAngularFactor(Vector3(0.0f, 1.0f, 0.0f));

    // Add base tile
    IntVector2 firstCoordPair = IntVector2(0,0);
    tileMap_[firstCoordPair] = new Tile(context_, firstCoordPair, this);
    // Add random tiles
    int addedTiles = 1;
    int platformSize = Random(8, 23);
    //Old random platform creation algorithm
    /*while (addedTiles < platformSize){
        bool foundNeighbour = false;
        const IntVector2 coordPair = IntVector2(Random(-platformSize/Random(1,3),platformSize/Random(1,3)),Random(-platformSize/Random(1,3),platformSize/Random(1,3)));
        const IntVector2 xMirroredPair = coordPair * IntVector2(-1,1);
        Vector<IntVector2> tileCoords = tileMap_.Keys();
        if (tileCoords.Contains(coordPair) || tileCoords.Contains(coordPair)) continue;

        for (int direction = 1; direction <= 4; direction++){
            if(!CheckEmptyNeighbour(coordPair, (TileElement)direction, true) ) foundNeighbour = true; continue;
        }

        if (foundNeighbour){
            addedTiles++;
            tileMap_[coordPair] = new Tile(context_, coordPair, this);
            if (coordPair.x_ != xMirroredPair.x_){
                tileMap_[xMirroredPair] = new Tile(context_, xMirroredPair, this);
                addedTiles++;
            }
        }
    }*/

    while (addedTiles < platformSize){
        //Pick a random exsisting tile from a list.
        Vector<IntVector2> coordsVector = tileMap_.Keys();
        IntVector2 randomTileCoords = coordsVector[Random((int)coordsVector.Length())];

        //Create a vector of numbers 1 to 4
        /*Vector<int> directions;
        for (int i = 1; i <= 4; i++){directions.Push(i);*/
        //Check neighbours in random orer
        char startDir = Random(1,4);
        for (int direction = startDir; direction < startDir+4; direction++){
            //int randomIndex = Random((int)directions.Length());
            //int direction = directions[randomIndex];
            //directions.Remove(direction);
            int clampedDir = direction;
            if (clampedDir > 4) clampedDir -= 4;
            if (CheckEmptyNeighbour(randomTileCoords, (TileElement)clampedDir, true))
            {
                IntVector2 newTileCoords = GetNeighbourCoords(randomTileCoords, (TileElement)clampedDir);
                AddTile(newTileCoords);
                addedTiles++;
                if (newTileCoords.x_ != 0) {
                    newTileCoords *= IntVector2(-1,1);
                    tileMap_[newTileCoords] = new Tile(context_, newTileCoords, this);
                    addedTiles++;
                }
            }
        }
    }

    //Add slots
    AddMissingSlots();
    FixFringe();

    /*rigidBody_->ApplyForce(Vector3(Random(-100.0f,100.0f), 0.0f, Random(-100.0f,100.0f)));
    rigidBody_->ApplyTorque(Vector3(0.0f, Random(-16.0f, 16.0f), 0.0f));*/

    Deselect();
}


void Platform::Start()
{
}

void Platform::Stop()
{
}

void Platform::Select()
{
    selected_ = true;
    for (int i = 0; i < slotMap_.Values().Length(); i++)
    {
        if (GetBuildingType(slotMap_.Values()[i]->coords_) <= B_EMPTY)
            EnableSlot(slotMap_.Values()[i]->coords_);
    }
}

void Platform::Deselect()
{
    selected_ = false;
    for (int i = 0; i < slotMap_.Values().Length(); i++)
    {
        DisableSlot(slotMap_.Values()[i]->coords_);
    }
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

bool Platform::DisableSlot(IntVector2 coords)
{
    slotMap_[coords]->rootNode_->SetEnabled(false);
}

bool Platform::EnableSlot(IntVector2 coords)
{
    slotMap_[coords]->rootNode_->SetEnabled(true);
}

void Platform::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    /*using namespace Update; double timeStep = 100.0f * eventData[P_TIMESTEP].GetFloat();
    //Push object towards y-zeroplane
    rigidBody_->ApplyForce(timeStep * rigidBody_->GetMass() *
                           Vector3(-0.5f*rigidBody_->GetLinearVelocity().x_,
                                   -5.0f*(rootNode_->GetPosition().y_+rigidBody_->GetLinearVelocity().y_),
                                   -0.5f*rigidBody_->GetLinearVelocity().z_));
    //Ease object into hoirzontal allignment and apply angular friction at the same time
    rigidBody_->ApplyTorque(timeStep * rigidBody_->GetMass() *
                            (rigidBody_->GetAngularVelocity()*Vector3(-1.5f, -2.0f, -1.5f)) +
                            (0.5f*(Quaternion::IDENTITY - rootNode_->GetRotation()).EulerAngles()*Vector3(1.0f, 0.0f, 1.0f)));*/
                            //rootNode_->GetRotation().Slerp(Quaternion::IDENTITY, timeStep).EulerAngles()*Vector3(0.1f, 0.0f, 0.1f));
}

void Platform::AddTile(IntVector2 newTileCoords)
{
    tileMap_[newTileCoords] = new Tile(context_, newTileCoords, this);
    //UpdateCenterOfMass();
}

void Platform::AddMissingSlots()
{
    Vector<IntVector2> tileCoords = tileMap_.Keys();
    for (uint nthTile = 0; nthTile < tileCoords.Size(); nthTile++){
        for (int element = 0; element <= 4; element++){
            IntVector2 checkCoords = GetNeighbourCoords(tileCoords[nthTile], (TileElement)element);
            if (CheckEmpty(checkCoords, false))
                slotMap_[checkCoords] = new Slot(context_, this, checkCoords);
        }
    }
}

void Platform::FixFringe()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Vector<SharedPtr<Tile>> tiles = tileMap_.Values();
    for (int tile = 0; tile < tiles.Length(); tile++)
    {
        for (int element = 1; element < TE_LENGTH; element++)
        {
            StaticModel* model = tiles[tile]->elements_[element]->GetComponent<StaticModel>();
            //Fix sides
            if (element <= 4){
                //If corresponding neighbour is empty
                if (CheckEmptyNeighbour(tiles[tile]->coords_, (TileElement)element, true))
                {
                    if (element == 1)
                    {
                        switch (tiles[tile]->GetBuilding())
                        {
                        case B_ENGINE : {
                            model->SetModel(cache->GetResource<Model>("Resources/Models/Engine_end.mdl"));
                            model->SetMaterial(0,cache->GetResource<Material>("Resources/Materials/block_center.xml"));
                            model->SetMaterial(1,cache->GetResource<Material>("Resources/Materials/solid.xml"));
                            model->SetMaterial(2,cache->GetResource<Material>("Resources/Materials/glow.xml"));
                            model->SetMaterial(3,cache->GetResource<Material>("Resources/Materials/glass.xml"));
                        } break;
                        default: model->SetModel(cache->GetResource<Model>("Resources/Models/Block_side.mdl")); break;
                        }
                    }
                    else if (element == 3)
                    {
                        switch (tiles[tile]->GetBuilding())
                        {
                        case B_ENGINE : {
                            model->SetModel(cache->GetResource<Model>("Resources/Models/Engine_start.mdl"));
                            model->SetMaterial(2,cache->GetResource<Material>("Resources/Materials/block_center.xml"));
                            model->SetMaterial(0,cache->GetResource<Material>("Resources/Materials/solid.xml"));
                            model->SetMaterial(1,cache->GetResource<Material>("Resources/Materials/glow.xml"));
                        } break;
                        default: model->SetModel(cache->GetResource<Model>("Resources/Models/Block_side.mdl")); break;
                        }
                    }
                    else model->SetModel(cache->GetResource<Model>("Resources/Models/Block_side.mdl"));
                }
                //If neighbour is not empty
                else {
                    if (element == 1) {
                        switch (tiles[tile]->GetBuilding())
                        {
                        case B_ENGINE : if (GetNeighbourType(tiles[tile]->coords_, (TileElement)element) == B_ENGINE) model->SetModel(SharedPtr<Model>()); break;
                        default : model->SetModel(cache->GetResource<Model>("Resources/Models/Block_tween.mdl")); break;
                        }
                    }
                    else if (element == 4) {model->SetModel(cache->GetResource<Model>("Resources/Models/Block_tween.mdl"));}
                    else model->SetModel(SharedPtr<Model>());
                }
            }
            //Fix corners
            else {
                switch (PickCornerType(tiles[tile]->coords_, (TileElement)element)){
                case CT_NONE:   model->SetModel(SharedPtr<Model>()); break;
                case CT_IN:     model->SetModel(cache->GetResource<Model>("Resources/Models/Block_incorner.mdl")); break;
                case CT_OUT:    model->SetModel(cache->GetResource<Model>("Resources/Models/Block_outcorner.mdl")); break;
                case CT_TWEEN:  model->SetModel(cache->GetResource<Model>("Resources/Models/Block_tweencorner.mdl")); break;
                case CT_DOUBLE: model->SetModel(cache->GetResource<Model>("Resources/Models/Block_doublecorner.mdl")); break;
                case CT_FILL:   model->SetModel(cache->GetResource<Model>("Resources/Models/Block_fillcorner.mdl")); break;
                default: break;
                }
                model->SetMaterial(cache->GetResource<Material>("Resources/Materials/block_center.xml"));
            }
        }
    }
}

void Platform::SetBuilding(IntVector2 coords, BuildingType type)
{
    tileMap_[coords]->SetBuilding(type);
    FixFringe();
}

bool Platform::CheckEmpty(IntVector2 coords, bool checkTiles = true) const
{
    if (checkTiles) return (!tileMap_.Keys().Contains(coords));
    else return (!slotMap_.Keys().Contains(coords));
}


bool Platform::CheckEmptyNeighbour(IntVector2 coords, TileElement element, bool checkTiles = true) const
{
    return CheckEmpty(GetNeighbourCoords(coords, element), checkTiles);
}



IntVector2 Platform::GetNeighbourCoords(IntVector2 coords, TileElement element) const
{
    IntVector2 shift = IntVector2::ZERO;
    switch (element){
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
    if (!CheckEmpty(coords))
    {
        return tileMap_[coords]->buildingType_;
    }
    else return B_SPACE;
}

BuildingType Platform::GetNeighbourType(IntVector2 coords, TileElement element)
{
    return GetBuildingType(GetNeighbourCoords(coords, element));
}

CornerType Platform::PickCornerType(IntVector2 tileCoords, TileElement element) const
{
    bool emptyCheck[3] = {false, false, false};
    switch (element){
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
    int neighbourMask = 0;
    for (int i = 2; i >= 0; i--){
        neighbourMask += !emptyCheck[i] << i;//(!emptyCheck[i]) * (int)pow(2, i);
    }
    switch (neighbourMask){
    case 0: return CT_OUT; break;
    case 1: return CT_TWEEN; break;
    case 2: return CT_DOUBLE; break;
    case 3: return CT_NONE; break;
    case 4: return CT_NONE; break;
    case 5: return CT_IN; break;
    case 6: return CT_NONE; break;
    case 7: if (element == TE_SOUTHEAST){
            return CT_FILL;
        } else return CT_NONE;
        break;
    default: return CT_NONE; break;
    }
}

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

#include "tile.h"
#include "kekelplithf.h"
#include "frop.h"
#include "grass.h"

Tile::Tile(Context *context, const IntVector2 coords, Platform *platform):
Object(context),
  platform_{platform},
  masterControl_{platform->masterControl_},
  coords_{coords}
{
    rootNode_ = platform_->rootNode_->CreateChild("Tile");
    rootNode_->SetPosition(Vector3(static_cast<double>(coords_.x_),
                                   0.0f,
                                   -static_cast<double>(coords_.y_)));
    //Increase platform mass
    platform_->rigidBody_->SetMass(platform_->rigidBody_->GetMass()+1.0f);
    //Add collision shape to platform
    collisionShape_ = platform->rootNode_->CreateComponent<CollisionShape>();
    collisionShape_->SetBox(Vector3::ONE);
    collisionShape_->SetPosition(Vector3(coords_.x_, 0.0f, -coords_.y_));
    //platform_->rigidBody_->EnableMassUpdate();

    //Create random tile addons
    int extraRandomizer{Random(23)};

    //Create a dreamspire
    if (extraRandomizer == 7) {
        Node* spireNode{rootNode_->CreateChild("Spire")};
        if (coords_.x_%2) spireNode->Rotate(Quaternion(180.0f, Vector3::UP));
        StaticModel* model{spireNode->CreateComponent<StaticModel>()};
        model->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Abode.mdl"));
        model->SetMaterial(0, masterControl_->cache_->GetResource<Material>("Resources/Materials/Abode.xml"));
        model->SetCastShadows(true);
    }
    //Create random kekelplithfs
    else if (extraRandomizer < 7){
        int totalImps{Random(1, 5)};
        for (int i{0}; i < totalImps; i++){
            Vector3 position = Vector3(-0.075f * totalImps + 0.15f * i, 0.0f, Random(-0.5f, 0.5f));
            new Kekelplithf(context_, masterControl_, rootNode_, position);
        }
    }
    //Create fire
    else if (extraRandomizer == 8){
        Node* fireNode{rootNode_->CreateChild("Fire")};
        fireNode->Translate(Vector3::DOWN);
        ParticleEmitter* particleEmitter{fireNode->CreateComponent<ParticleEmitter>()};
        ParticleEffect* particleEffect{masterControl_->cache_->GetResource<ParticleEffect>("Resources/Particles/Fire.xml")};
        particleEmitter->SetEffect(particleEffect);
        Light* fireLight{fireNode->CreateComponent<Light>()};
        fireLight->SetRange(2.3f);
        fireLight->SetColor(Color(1.0f, 0.88f, 0.666f));
        fireLight->SetCastShadows(true);
    }
    //Create frop crops
    else if (extraRandomizer > 8 && coords.y_%2 == 0){
        for (int i{0}; i < 4; ++i){
            for (int j{0}; j < 3; ++j){
                    Vector3 position = Vector3(-0.375f + i * 0.25f, 0.0f, -0.3f + j * 0.3f);
                    new Frop(context_, masterControl_, rootNode_, position);
            }
        }
        /*for (int i = 0; i < extraRandomizer - 8; i++){
            Vector3 randomPosition = Vector3(Random(-0.4f, 0.4f), 0.0f, Random(-0.4f, 0.4f));
            new Frop(context_, masterControl_, rootNode_, randomPosition);
        }*/
    }

    /*for (int i = 0; i < (23-extraRandomizer)+16; i++){
        Vector3 randomPosition = Vector3(Random(-0.4f, 0.4f), 0.0f, Random(-0.4f, 0.4f));
        new Grass(context_, masterControl_, rootNode_, randomPosition);
    }*/

    //Set up center and edge nodes.
    for (int i{0}; i <= 8; i++){
        elements_[i] = rootNode_->CreateChild("TilePart");
        int nthOfType{(i-1)%4};
        if (i > 0) elements_[i]->Rotate(Quaternion(0.0f, 90.0f-nthOfType*90.0f, 0.0f));
        //Add the right model to the node
        StaticModel* model{elements_[i]->CreateComponent<StaticModel>()};
        switch (i){
        case TE_CENTER:    model->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Block_center.mdl"));
            model->SetMaterial(masterControl_->cache_->GetResource<Material>("Resources/Materials/BlockCenter.xml"));
            break;
        case TE_NORTH:case TE_EAST:case TE_SOUTH:case TE_WEST:
            model->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Rock_side.mdl"));
            model->SetMaterial(masterControl_->cache_->GetResource<Material>("Resources/Materials/RockSide.xml"));
            break;
        case TE_NORTHEAST:case TE_SOUTHEAST:case TE_SOUTHWEST:case TE_NORTHWEST:
            model->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Block_outcorner.mdl"));
            model->SetMaterial(masterControl_->cache_->GetResource<Material>("Resources/Materials/BlockCenter.xml"));
            break;
                default:break;
        }


        model->SetCastShadows(true);
    }

    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Tile, HandleUpdate));
}

void Tile::Start()
{
}
void Tile::Stop()
{
}

void Tile::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    Input* input{GetSubsystem<Input>()};
    using namespace Update; double timeStep = eventData[P_TIMESTEP].GetFloat();
    if (buildingType_ == B_ENGINE && input->GetKeyDown(KEY_UP)){
        platform_->rigidBody_->ApplyForce(platform_->rootNode_->GetRotation() * rootNode_->GetDirection() * 500.0f*timeStep, platform_->rootNode_->GetRotation() * rootNode_->GetPosition());
    }
    else if (buildingType_ == B_ENGINE && input->GetKeyDown(KEY_DOWN)){
        platform_->rigidBody_->ApplyForce(platform_->rootNode_->GetRotation() * rootNode_->GetDirection() * -500.0f*timeStep, platform_->rootNode_->GetRotation() * rootNode_->GetPosition());
    }
}

void Tile::SetBuilding(BuildingType type)
{
    Vector<SharedPtr<Node> > children{rootNode_->GetChildren()};
    for (unsigned i{0}; i < children.Size(); i++)
        if (children[i]->GetNameHash() != N_TILEPART)
            children[i]->SetEnabledRecursive(false);

    buildingType_ = type;
    if (buildingType_ > B_EMPTY) platform_->DisableSlot(coords_);
    StaticModel* model{elements_[0]->GetComponent<StaticModel>()};
    switch (buildingType_)
    {
    case B_ENGINE: {
        model->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Engine_center.mdl"));
        model->SetMaterial(0, masterControl_->cache_->GetResource<Material>("Resources/Materials/BlockCenter.xml"));
        model->SetMaterial(1, masterControl_->cache_->GetResource<Material>("Resources/Materials/Structure.xml"));
        model->SetMaterial(2, masterControl_->cache_->GetResource<Material>("Resources/Materials/Glow.xml"));
        model->SetMaterial(3, masterControl_->cache_->GetResource<Material>("Resources/Materials/Glass.xml"));
    } break;
    default: break;
    }
}

BuildingType Tile::GetBuilding()
{
    return buildingType_;
}

//Fix this tile's element models and materials according to
void Tile::FixFringe()
{
    for (int element{1}; element < TE_LENGTH; element++)
    {
        StaticModel* model{elements_[element]->GetComponent<StaticModel>()};
        //Fix sides
        if (element <= 4){
            //If corresponding neighbour is empty
            if (platform_->CheckEmptyNeighbour(coords_, (TileElement)element, true))
            {
                if (element == 1)
                {
                    switch (buildingType_)
                    {
                    case B_ENGINE :
                        model->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Engine_end.mdl"));
                        model->SetMaterial(0,masterControl_->cache_->GetResource<Material>("Resources/Materials/BlockCenter.xml"));
                        model->SetMaterial(1,masterControl_->cache_->GetResource<Material>("Resources/Materials/Structure.xml"));
                        model->SetMaterial(2,masterControl_->cache_->GetResource<Material>("Resources/Materials/Glow.xml"));
                        model->SetMaterial(3,masterControl_->cache_->GetResource<Material>("Resources/Materials/Glass.xml"));
                    break;
                    default: model->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Rock_side.mdl"));
                    break;
                    }
                }
                else if (element == 3)
                {
                    switch (buildingType_)
                    {
                    case B_ENGINE :
                        model->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Engine_start.mdl"));
                        model->SetMaterial(2,masterControl_->cache_->GetResource<Material>("Resources/Materials/BlockCenter.xml"));
                        model->SetMaterial(0,masterControl_->cache_->GetResource<Material>("Resources/Materials/Structure.xml"));
                        model->SetMaterial(1,masterControl_->cache_->GetResource<Material>("Resources/Materials/Glow.xml"));
                    break;
                    default: model->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Rock_side.mdl"));
                    break;
                    }
                }
                else model->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Rock_side.mdl"));
            }
            //If neighbour is not empty
            else {
                if (element == 1) {
                    switch (GetBuilding())
                    {
                    case B_ENGINE : if (platform_->GetNeighbourType(coords_, (TileElement)element) == B_ENGINE) model->SetModel(SharedPtr<Model>()); break;
                    default :
                        model->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Block_tween.mdl"));
                        model->SetMaterial(masterControl_->cache_->GetResource<Material>("Resources/Materials/BlockCenter.xml"));
                        break;
                    }
                }
                else if (element == 4) {
                    model->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Block_tween.mdl"));
                    model->SetMaterial(masterControl_->cache_->GetResource<Material>("Resources/Materials/BlockCenter.xml"));
                }
                else model->SetModel(SharedPtr<Model>());
            }
        }
        //Fix corners
        else {
            bool fill{false};
            switch (platform_->PickCornerType(coords_, static_cast<TileElement>(element) )) {
            case CT_NONE:   model->SetModel(SharedPtr<Model>()); break;
            case CT_IN:     model->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Rock_incorner.mdl")); break;
            case CT_OUT:    model->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Rock_outcorner.mdl")); break;
            case CT_TWEEN:  model->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Rock_tweencorner.mdl")); break;
            case CT_DOUBLE: model->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Rock_doublecorner.mdl")); break;
            case CT_FILL:   model->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Block_fillcorner.mdl")); fill = true; break;
            default: break;
            }
            model->SetMaterial(!fill ?
masterControl_->cache_->GetResource<Material>("Resources/Materials/RockSide.xml") :
masterControl_->cache_->GetResource<Material>("Resources/Materials/BlockCenter.xml"));
        }
    }
}























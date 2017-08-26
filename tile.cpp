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
#include "platform.h"

void Tile::RegisterObject(Context *context)
{
    context->RegisterFactory<Tile>();
}

Tile::Tile(Context *context):
SceneObject(context),
  health_{1.0f}
{

}

void Tile::OnNodeSet(Node *node)
{ if (!node) return;


    StaticModel* centerModel{ node_->CreateComponent<StaticModel>() };
    centerModel->SetModel(RESOURCE->GetModel("Terrain/Center_1"));
    centerModel->SetMaterial(RESOURCE->GetMaterial("VCol"));
    centerModel->SetCastShadows(true);

    //Set up compound nodes.
    for (int i{0}; i < TE_LENGTH; ++i) {
        elements_[i] = node_->CreateChild("TilePart");
        elements_[i]->SetPosition(ElementPosition(static_cast<TileElement>(i)));
//        if (i < 3) elements_[i]->Rotate(Quaternion(0.0f, 180.0f, 0.0f));

        StaticModel* model{ elements_[i]->CreateComponent<StaticModel>() };
        model->SetCastShadows(true);
    }

    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Tile, HandleUpdate));
}
Vector3 Tile::ElementPosition(TileElement element)
{
    return Vector3(0.5f - (element / 3),
                   0.0f,
                   1.0f - element % 3);
}


void Tile::Set(const IntVector2 coords, Platform *platform)
{
    coords_ = coords;
    platform_ = platform;

    node_->SetParent(platform_->GetNode());
    node_->SetRotation(Quaternion::IDENTITY);

    SceneObject::Set(Platform::CoordsToPosition(coords));

    //Set up compound nodes.
    for (int i{0}; i < TE_LENGTH; ++i){
        //Add collision shape to platform
        colliders_[i] = platform_->GetNode()->CreateComponent<CollisionShape>();
        colliders_[i]->SetBox(Vector3::ONE * 0.45f,
                                platform_->CoordsToPosition(coords_) + ElementPosition(static_cast<TileElement>(i)),
                                Quaternion(45.0f, Vector3::UP));
        colliders_[i]->SetEnabled(true);
    }

    //Increase platform mass
    platform_->rigidBody_->SetMass(platform_->rigidBody_->GetMass() + 1.0f);

    //Create random tile addons
    int extraRandomizer{ Random(23) };

    //Create a dreamspire
    if (extraRandomizer == 7) {
        Node* spireNode{ node_->CreateChild("Spire") };
        spireNode->Translate(Vector3::UP * PLATFORM_HALF_THICKNESS);
        if (coords_.x_ % 2) spireNode->Rotate(Quaternion(180.0f, Vector3::UP));
        StaticModel* model{ spireNode->CreateComponent<StaticModel>() };
        model->SetModel(RESOURCE->GetModel("Abode"));
        model->SetMaterial(0, RESOURCE->GetMaterial("Abode"));
        model->SetCastShadows(true);
    }
    //Create Ekelplitfs
    else if (extraRandomizer < 7){
        int totalImps{Random(0, 2)};
        for (int i{0}; i < totalImps; ++i){
            Vector3 position{ Vector3(-0.075f * totalImps + 0.15f * i, PLATFORM_HALF_THICKNESS, Random(-0.5f, 0.5f)) };
            SPAWN->Create<Kekelplithf>()->Set(position, node_);
        }
    }
    //Create fire
    else if (extraRandomizer == 8){
        Node* fireNode{ node_->CreateChild("Fire") };
        fireNode->Translate(Vector3::DOWN * PLATFORM_HALF_THICKNESS * 2.0f);
        ParticleEmitter* particleEmitter{ fireNode->CreateComponent<ParticleEmitter>() };
        ParticleEffect* particleEffect{ CACHE->GetResource<ParticleEffect>("Resources/Particles/Fire.xml") };
        particleEmitter->SetEffect(particleEffect);
        Light* fireLight{fireNode->CreateComponent<Light>()};
        fireLight->SetRange(2.3f);
        fireLight->SetColor(Color(1.0f, 0.88f, 0.666f));
        fireLight->SetCastShadows(true);
    }
    //Create frop crops
    else if (extraRandomizer > 8 && coords.y_ % 2 == 0){
        for (int i{0}; i < 4; ++i){
            for (int j{0}; j < 3; ++j){
                    Vector3 position{ Vector3(-0.375f + i * 0.25f, PLATFORM_HALF_THICKNESS, -0.3f + j * 0.3f) };
                    SPAWN->Create<Frop>()->Set(position, node_);
            }
        }
        for (int i = 0; i < extraRandomizer - 8; i++){
            Vector3 randomPosition{ Vector3(Random(-0.4f, 0.4f), PLATFORM_HALF_THICKNESS, Random(-0.4f, 0.4f)) };
            SPAWN->Create<Frop>()->Set(randomPosition, node_);
        }
    }
}

void Tile::Disable()
{
    for (unsigned c{0}; c < TE_LENGTH; ++c)
        colliders_[c]->SetEnabled(false);

    platform_->rigidBody_->SetMass(platform_->rigidBody_->GetMass() - 1.0f);
}

void Tile::Start()
{
}
void Tile::Stop()
{
}

void Tile::HandleUpdate(StringHash eventType, VariantMap &eventData)
{ (void)eventType;

    Input* input{GetSubsystem<Input>()};
    double timeStep = eventData[Update::P_TIMESTEP].GetFloat();
    if (buildingType_ == B_ENGINE && input->GetKeyDown(KEY_UP)){
        platform_->rigidBody_->ApplyForce(platform_->GetNode()->GetDirection() * Clamp(5000.0f - 42.0f * Sign(node_->GetPosition().x_) * platform_->GetNode()->WorldToLocal(platform_->rigidBody_->GetAngularVelocity()).y_, 0.0f, 9000.0f) * timeStep, -platform_->GetNode()->GetRotation() * node_->GetPosition());
    }
    else if (buildingType_ == B_ENGINE && input->GetKeyDown(KEY_DOWN)){
        platform_->rigidBody_->ApplyForce(platform_->GetNode()->GetRotation() * node_->GetDirection() * -5000.0f*timeStep, platform_->GetNode()->GetRotation() * node_->GetPosition());
    }
}

void Tile::SetBuilding(BuildingType type)
{
    Vector<SharedPtr<Node> > children{node_->GetChildren()};
    for (unsigned i{0}; i < children.Size(); i++)
        if (children[i]->GetNameHash() != N_TILEPART)
            children[i]->SetEnabledRecursive(false);

    buildingType_ = type;
    if (buildingType_ > B_EMPTY) platform_->DisableSlot(coords_);
    StaticModel* model{elements_[0]->GetComponent<StaticModel>()};
    switch (buildingType_)
    {
    case B_ENGINE: {
        model->SetModel(MC->CACHE->GetResource<Model>("Resources/Models/Engine_center.mdl"));
        model->SetMaterial(0, MC->CACHE->GetResource<Material>("Resources/Materials/BlockCenter.xml"));
        model->SetMaterial(1, MC->CACHE->GetResource<Material>("Resources/Materials/Structure.xml"));
        model->SetMaterial(2, MC->CACHE->GetResource<Material>("Resources/Materials/Glow.xml"));
        model->SetMaterial(3, MC->CACHE->GetResource<Material>("Resources/Materials/Glass.xml"));
    } break;
    default: break;
    }
}

BuildingType Tile::GetBuilding()
{
    return buildingType_;
}

void Tile::FixFringe()
{
    for (int e{0}; e < TE_LENGTH; ++e) {

        Node* element{ elements_[e] };
        StaticModel* model{ element->GetComponent<StaticModel>() };

        CornerType cornerType{ platform_->PickCornerType(coords_, static_cast<TileElement>(e)) };

        switch (cornerType) {
        case CT_NONE:
            model->SetModel(nullptr);
            break;
        case CT_OUT: {
            model->SetModel(RESOURCE->GetModel("Terrain/BendOut_" + String(Random(1, 3))));

            switch (e) {
            case TE_NORTHEAST:
                elements_[e]->SetRotation(Quaternion(0.0f, 180.0f, 0.0f));
                break;
            case TE_SOUTHEAST:
                elements_[e]->SetRotation(Quaternion(0.0f, -90.0f, 0.0f));
                break;
            case TE_SOUTHWEST:
                elements_[e]->SetRotation(Quaternion(0.0f, 0.0f, 0.0f));
                break;
            case TE_NORTHWEST:
                elements_[e]->SetRotation(Quaternion(0.0f, 90.0f, 0.0f));
                break;
            }

        } break;
        case CT_INA: {
            if (e == TE_EAST || e == TE_WEST)
                model->SetModel(RESOURCE->GetModel("Terrain/BendInA_1"));
            else
                model->SetModel(nullptr);

            switch (e) {
            case TE_EAST:
                elements_[e]->SetRotation(Quaternion(0.0f, 0.0f, 0.0f));
                break;
            case TE_WEST:
                elements_[e]->SetRotation(Quaternion(0.0f, 180.0f, 0.0f));
                break;
            }

        } break;
        case CT_INB: {
            if (e == TE_EAST || e == TE_WEST)
                model->SetModel(RESOURCE->GetModel("Terrain/BendInB_1"));
            else
                model->SetModel(nullptr);

                switch (e) {
                case TE_EAST:
                    elements_[e]->SetRotation(Quaternion(0.0f, 180.0f, 0.0f));
                    break;
                case TE_WEST:
                    elements_[e]->SetRotation(Quaternion(0.0f, 0.0f, 0.0f));
                    break;
                }
        } break;
        case CT_STRAIGHTA: {
            if (e == TE_NORTHEAST || e == TE_NORTHWEST)
                model->SetModel(RESOURCE->GetModel("Terrain/StraightA_1"));
            else
                model->SetModel(nullptr);

            switch (e) {
            case TE_NORTHEAST:
                elements_[e]->SetRotation(Quaternion(0.0f, 180.0f, 0.0f));
                break;
            case TE_NORTHWEST:
                elements_[e]->SetRotation(Quaternion(0.0f, 0.0f, 0.0f));
                break;
            }

        } break;
        case CT_STRAIGHTB: {
            model->SetModel(RESOURCE->GetModel("Terrain/StraightB_1"));

            switch (e) {
            case TE_EAST:
                elements_[e]->SetRotation(Quaternion(0.0f, 180.0f, 0.0f));
                break;
            case TE_WEST:
                elements_[e]->SetRotation(Quaternion(0.0f, 0.0f, 0.0f));
                break;
            }

        } break;
        case CT_FILL: {
            if (e == TE_EAST || e == TE_WEST)
                model->SetModel(RESOURCE->GetModel("Terrain/Fill_1"));
            else
                model->SetModel(nullptr);

            switch (e) {
            case TE_WEST:
                elements_[e]->SetRotation(Quaternion(0.0f, 180.0f, 0.0f));
                break;
            case TE_EAST:
                elements_[e]->SetRotation(Quaternion(0.0f, 0.0f, 0.0f));
                break;
            }

        } break;
        default: break;
        }
        model->SetMaterial(RESOURCE->GetMaterial("VCol"));
    }
}

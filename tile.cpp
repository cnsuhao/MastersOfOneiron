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
#include "ekelplitf.h"
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

    node_->AddTag("Platform");
    node_->AddTag("Tile");

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

//    SubscribeToEvent(E_PHYSICSPOSTSTEP, URHO3D_HANDLER(Tile, HandleFixedUpdate));
}
Vector3 Tile::ElementPosition(TileElement element)
{
    return Vector3(0.5f - (element / 2),
                   0.0f,
                   0.5f - (element % 2));
}


void Tile::Set(const IntVector2 coords, Platform *platform)
{
    coords_ = coords;
    platform_ = platform;

    node_->SetParent(platform_->GetNode());
    node_->SetRotation(Quaternion::IDENTITY);

    SceneObject::Set(platform_->CoordsToPosition(coords));

    //Add collision shape to platform
    collider_ = platform_->GetNode()->CreateComponent<CollisionShape>();
    collider_->SetBox(Vector3(1.1f, 1.1f, 0.5f),
                            node_->GetPosition());
//    collider_->SetEnabled(true);

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
            SPAWN->Create<Ekelplitf>()->Set(position, node_);
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
    collider_->SetEnabled(false);

    platform_->rigidBody_->SetMass(platform_->rigidBody_->GetMass() - 1.0f);
}

void Tile::Start()
{
}
void Tile::Stop()
{
}

void Tile::FixedUpdate(float timeStep)
{
    Input* input{GetSubsystem<Input>()};

    bool up{ input->GetKeyDown(KEY_UP) };
    bool down{ input->GetKeyDown(KEY_DOWN) };
    bool left{ input->GetKeyDown(KEY_LEFT) };
    bool right{ input->GetKeyDown(KEY_RIGHT) };

    if (buildingType_ == B_ENGINE) {
        if (true)//up
//         || (left && node_->GetPosition().x_ > 0.0f)
//         || (right && node_->GetPosition().x_ < 0.0f))
        {
            platform_->rigidBody_->ApplyForce(platform_->GetNode()->GetDirection() * 5000.0f * timeStep, node_->GetPosition());
        }
        else if (down
                || (right && node_->GetPosition().x_ > 0.0f)
                || (left && node_->GetPosition().x_ < 0.0f))
        {
            platform_->rigidBody_->ApplyForce(-platform_->GetNode()->GetDirection() * 5000.0f * timeStep, node_->GetPosition());
        }
    }
}

void Tile::SetBuilding(BuildingType type)
{
    buildingType_ = type;
    if (buildingType_ > B_EMPTY) platform_->DisableSlot(coords_);
    StaticModel* model{ node_->GetComponent<StaticModel>() };
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
        case CT_IN: {
            model->SetModel(RESOURCE->GetModel("Terrain/BendIn_" + String(Random(1, 5))));

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
        case CT_OUT: {
                model->SetModel(RESOURCE->GetModel("Terrain/BendOut_" + String(Random(1, 5))));

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
        case CT_STRAIGHT: {
                model->SetModel(RESOURCE->GetModel("Terrain/Straight_" + String(Random(1, 5))));

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
        case CT_BRIDGE: {
            model->SetModel(RESOURCE->GetModel("Terrain/Bridge_1"));

            switch (e) {
            case TE_NORTHEAST: case TE_SOUTHWEST:
                elements_[e]->SetRotation(Quaternion(0.0f, 0.0f, 0.0f));
                break;
            case TE_SOUTHEAST: case TE_NORTHWEST:
                elements_[e]->SetRotation(Quaternion(0.0f, 90.0f, 0.0f));
                break;
            }
        } break;
        case CT_FILL: {
            if (e == TE_NORTHEAST)
                model->SetModel(RESOURCE->GetModel("Terrain/Fill_1"));
            else
                model->SetModel(nullptr);

        } break;
        default: break;
        }
        if (model->GetModel())
            model->SetMaterial(RESOURCE->GetMaterial("VCol"));
    }
}

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

#include "ekelplitf.h"

void Ekelplitf::RegisterObject(Context *context)
{
    context->RegisterFactory<Ekelplitf>();
}

Ekelplitf::Ekelplitf(Context* context):
    SceneObject(context)
{
}

void Ekelplitf::OnNodeSet(Node *node)
{ if (!node) return;

    node_ = node_->CreateChild("ImpModelNode");
    bodyModel_ = node_->CreateComponent<AnimatedModel>();
//    bodyModel_->SetModel(RESOURCE->GetModel("Ekelplithf_LOD023"));
    bodyModel_->SetMaterial(RESOURCE->GetMaterial("Kekelplithf"));
    bodyModel_->SetCastShadows(true);
/*
    for (int e{0}; e < 3; ++e) {

        AnimatedModel* equipment{ node_->CreateComponent<AnimatedModel>() };
        switch (e) {
        case 0:
            equipment->SetModel(RESOURCE->GetModel("Kilt_LOD023"));
            break;
        case 1:
            equipment->SetModel(RESOURCE->GetModel("Armour_LOD023"));
            break;
        case 2:
            equipment->SetModel(RESOURCE->GetModel("ShieldBlade_LOD023"));
            break;
        }
        equipment_.Push(equipment);
    }
*/
//    AnimationController* animCtrl{node_->CreateComponent<AnimationController>()};
//    animCtrl->PlayExclusive("Resources/Animations/StandUp.ani", 0, true);
//    animCtrl->SetSpeed("Resources/Animations/StandUp.ani", 0.5f + randomizer_);

//    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Ekelplitf, HandleUpdate));
}

void Ekelplitf::Set(Vector3 position, Node *parent)
{
    node_->SetParent(parent);
    node_->SetRotation(Quaternion::IDENTITY);

    SceneObject::Set(position);

    randomizer_ = Random(0.5f,0.75f);
    node_->Rotate(Quaternion(0.0f,Random(360.0f),0.0f));
    node_->SetScale(Random(0.015f,0.016f));

}

void Ekelplitf::Start()
{
}
void Ekelplitf::Stop()
{
}

void Ekelplitf::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
//    double timeStep = eventData[Update::P_TIMESTEP].GetFloat();
}

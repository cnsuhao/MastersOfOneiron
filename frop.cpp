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

#include "frop.h"

void Frop::RegisterObject(Context *context)
{
    context->RegisterFactory<Frop>();
}

Frop::Frop(Context *context):
    SceneObject(context)
{
}

void Frop::OnNodeSet(Node *node)
{ if (!node) return;

    growthStart_ = Random(0.0f, 23.0f);
    node_->Rotate(Quaternion(Random(-10.0f, 10.0f), Random(360.0f), Random(-10.0f, 10.0f)));
    node_->SetScale(0.0f);
    float randomWidth{ Random(0.5f, 1.0f) };
    scale_ = Vector3{randomWidth, Random(0.5f, 0.5f + randomWidth), randomWidth};
    fropModel_ = node_->CreateComponent<StaticModel>();
    fropModel_->SetModel(MC->CACHE->GetResource<Model>("Resources/Models/Frop.mdl"));
    fropModel_->SetMaterial(MC->CACHE->GetResource<Material>("Resources/Materials/Frop.xml"));
    fropModel_->SetCastShadows(true);

    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Frop, HandleUpdate));
}

void Frop::Set(Vector3 position, Node *parent)
{
    node_->SetParent(parent);
    SceneObject::Set(position);
}

void Frop::Start()
{
}

void Frop::Stop()
{
}

void Frop::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    float timeStep{eventData[Update::P_TIMESTEP].GetFloat()};
    age_ += timeStep;
    if (age_ > growthStart_ && node_->GetScale().Length() < scale_.Length())
        node_->SetScale(node_->GetScale() + (5.0f * timeStep * (scale_ - node_->GetScale())));
}

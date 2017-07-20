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

#include "grass.h"

void Grass::RegisterObject(Context *context)
{
    context->RegisterFactory<Grass>();
}

Grass::Grass(Context *context) : SceneObject(context)
{
}

void Grass::OnNodeSet(Node *node)
{ if (!node) return;

    node_->Rotate(Quaternion(Random(-10.0f, 10.0f),Random(360.0f),Random(-10.0f, 10.0f)));
    float randomWidth{Random(0.5f,1.5f)};
    node_->SetScale(Vector3(randomWidth, Random(0.25f,randomWidth), randomWidth));
    grassModel_ = node_->CreateComponent<StaticModel>();
    grassModel_->SetModel(MC->CACHE->GetResource<Model>("Resources/Models/Grass.mdl"));
    grassModel_->SetMaterial(0, MC->CACHE->GetResource<Material>("Resources/Materials/BlockCenter.xml"));
    grassModel_->SetMaterial(1, MC->CACHE->GetResource<Material>("Resources/Materials/Shadow.xml"));
    grassModel_->SetCastShadows(false);

    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Grass, HandleUpdate));
}

void Grass::Set(Vector3 position, Node *parent)
{
    node_->SetParent(parent);
    SceneObject::Set(position);
}

void Grass::Start()
{
}

void Grass::Stop()
{
}

void Grass::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    /*using namespace Update;
    double timeStep = eventData[P_TIMESTEP].GetFloat();*/
}

/* HoverAce
// Copyright (C) 2017 LucKey Productions (luckeyproductions.nl)
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


#include "world.h"

#include "storm.h"

void Storm::RegisterObject(Context* context)
{
    context->RegisterFactory<Storm>();
}

Storm::Storm(Context* context) : SceneObject(context)
{
}

void Storm::OnNodeSet(Node* node)
{ if(!node) return;
}

void Storm::Set(Vector3 position)
{
    SceneObject::Set(position);

    Vector3 rhombicCenter{ GetScene()->GetComponent<World>()->GetNearestRhombicCenter(node_->GetWorldPosition()) };

    node_->LookAt(node_->GetWorldPosition() + (rhombicCenter - node_->GetWorldPosition().Normalized() * rhombicCenter.ProjectOntoAxis(node_->GetWorldPosition())),
                  node_->GetWorldPosition().Normalized());

    StaticModel* sphere{ node_->CreateComponent<StaticModel>() };
    sphere->SetModel(RESOURCE->GetModel("Sphere"));

    node_->SetScale(Vector3(10.0f, 42.0f, 10.0f));

    node_->CreateComponent<RigidBody>();
    node_->CreateComponent<CollisionShape>()->SetSphere(1.0f);
}

void Storm::Update(float timeStep)
{
}





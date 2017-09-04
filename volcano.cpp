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


#include "resourcemaster.h"
#include "world.h"
#include "volcano.h"



void Volcano::RegisterObject(Context* context)
{
    context->RegisterFactory<Volcano>();
}

Volcano::Volcano(Context* context) : SceneObject(context)
{
}

void Volcano::OnNodeSet(Node* node)
{ if(!node) return;
}

void Volcano::Set(Vector3 position)
{
    SceneObject::Set(position);

    Vector3 rhombicCenter{ GetScene()->GetComponent<World>()->GetNearestRhombicCenter(node_->GetWorldPosition()) };

    node_->LookAt(node_->GetWorldPosition() + (rhombicCenter - node_->GetWorldPosition().Normalized() * rhombicCenter.ProjectOntoAxis(node_->GetWorldPosition())),
                  node_->GetWorldPosition().Normalized());

    node_->CreateComponent<StaticModel>()->SetModel(RESOURCE->GetModel("Box"));
    node_->SetScale(Vector3(23.0f, 23.0f, 23.0f));

    node_->CreateComponent<RigidBody>();
    node_->CreateComponent<CollisionShape>()->SetBox(Vector3::ONE);
}

void Volcano::Update(float timeStep)
{
}





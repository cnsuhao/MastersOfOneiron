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

#include "slot.h"

#include "platform.h"

void Slot::RegisterObject(Context *context)
{
    context->RegisterFactory<Slot>();
}

Slot::Slot(Context *context):
SceneObject(context)
{
}

void Slot::OnNodeSet(Node *node)
{ if (!node) return;

    SceneObject::OnNodeSet(node);

    AnimatedModel* model{ node_->CreateComponent<AnimatedModel>() };
    model->SetModel(RESOURCE->GetModel("Slot"));
    model->SetMaterial(RESOURCE->GetMaterial("Glow"));
    model->SetCastShadows(false);
    StaticModel* hitModel{ node_->CreateComponent<StaticModel>() };
    hitModel->SetModel(RESOURCE->GetModel("SlotHitPlane"));
    hitModel->SetMaterial(RESOURCE->GetMaterial("Invisible"));
    model->SetCastShadows(false);

    cursor_ = MC->world.cursor.sceneCursor;

    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Slot, HandleUpdate));
}

void Slot::Set(IntVector2 coords, Platform* platform)
{
    platform_ = platform;
    coords_ = coords;

    node_->SetParent(platform_->GetNode());
    node_->SetRotation(Quaternion::IDENTITY);

    SceneObject::Set(platform_->CoordsToPosition(coords, PLATFORM_HALF_THICKNESS + 0.05f));
}

void Slot::Start()
{
}
void Slot::Stop()
{
}

void Slot::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    //using namespace Update; double timeStep = eventData[P_TIMESTEP].GetFloat();

    //double elapsedTime = platform_->masterControl_->world.scene_->GetElapsedTime();
    //rootNode_->SetScale(Vector3(1.0f+(0.1f*sinf(2.0f*elapsedTime)),1.0f+(0.1f*sinf(10.0f*elapsedTime)),1.0f+(0.1f*sinf(6.0f*elapsedTime))));

    float cursorDist = Max(0.0f, (node_->GetWorldPosition() - cursor_->GetPosition()).Length()-0.7f);
    float scale = Clamp(1.0f - (0.1f * cursorDist), 0.0f, 1.0f);
    for (int i = 0; i < 3; i ++) scale *= scale;
    node_->SetScale(scale);
    //model_->GetMaterial()->SetShaderParameter("MatDiffColor", Color(0.125f, 1.0f, 1.0f, 0.7f));
}

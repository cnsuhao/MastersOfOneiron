/* Masters of Oneiron
// Copyright (C) 2015 LucKey Productions (luckeyproductions.nl)
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

Slot::Slot(Context *context, Platform* platform, IntVector2 coords):
Object(context)
{
    platform_ = platform;
    coords_ = coords;
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Slot, HandleUpdate));
    rootNode_ = platform_->rootNode_->CreateChild("Slot");
    rootNode_->SetPosition(Vector3(coords.x_, 0.0f, -coords.y_));
    StaticModel* model = rootNode_->CreateComponent<StaticModel>();
    model->SetModel(cache->GetResource<Model>("Resources/Models/Slot.mdl"));
    model->SetMaterial(cache->GetResource<Material>("Resources/Materials/Glow.xml"));
    model->SetCastShadows(false);
    StaticModel* hitModel = rootNode_->CreateComponent<StaticModel>();
    hitModel->SetModel(cache->GetResource<Model>("Resources/Models/SlotHitPlane.mdl"));
    hitModel->SetMaterial(cache->GetResource<Material>("Resources/Materials/Invisible.xml"));
    model->SetCastShadows(false);

    cursor_ = platform_->masterControl_->world.cursor.sceneCursor;
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

    float cursorDist = Max(0.0f, (rootNode_->GetWorldPosition() - cursor_->GetPosition()).Length()-0.7f);
    float scale = Clamp(1.0f - (0.1f*cursorDist), 0.0f, 1.0f);
    for (int i = 0; i < 3; i ++) scale *= scale;
    rootNode_->SetScale(scale);
    //model_->GetMaterial()->SetShaderParameter("MatDiffColor", Color(0.125f, 1.0f, 1.0f, 0.7f));
}

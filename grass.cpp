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

#include "grass.h"

Grass::Grass(Context *context, MasterControl *masterControl, Node *parent, Vector3 pos) : Object(context)
{
    masterControl_ = masterControl;
    rootNode_ = parent->CreateChild("Grass");
    rootNode_->SetPosition(pos);
    rootNode_->Rotate(Quaternion(Random(-10.0f, 10.0f),Random(360.0f),Random(-10.0f, 10.0f)));
    float randomWidth = Random(0.5f,1.5f);
    rootNode_->SetScale(Vector3(randomWidth, Random(0.25f,randomWidth), randomWidth));
    grassModel_ = rootNode_->CreateComponent<StaticModel>();
    grassModel_->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Grass.mdl"));
    grassModel_->SetMaterial(0, masterControl_->cache_->GetResource<Material>("Resources/Materials/BlockCenter.xml"));
    grassModel_->SetMaterial(1, masterControl_->cache_->GetResource<Material>("Resources/Materials/Shadow.xml"));
    grassModel_->SetCastShadows(false);

    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Grass, HandleUpdate));
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

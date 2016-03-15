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

#include "frop.h"

Frop::Frop(Context *context, MasterControl *masterControl, Node *parent, Vector3 pos) : Object(context)
{
    growthStart_ = Random(0.0f, 23.0f);
    masterControl_ = masterControl;
    rootNode_ = parent->CreateChild("Frop");
    rootNode_->SetPosition(pos);
    rootNode_->Rotate(Quaternion(Random(-10.0f, 10.0f),Random(360.0f),Random(-10.0f, 10.0f)));
    rootNode_->SetScale(0.0f);
    float randomWidth = Random(0.5f,1.0f);
    scale_ = Vector3(randomWidth, Random(0.5f,0.5f+randomWidth), randomWidth);
    fropModel_ = rootNode_->CreateComponent<StaticModel>();
    fropModel_->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Frop.mdl"));
    fropModel_->SetMaterial(masterControl_->cache_->GetResource<Material>("Resources/Materials/Frop.xml"));
    fropModel_->SetCastShadows(true);

    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Frop, HandleUpdate));
}

void Frop::Start()
{
}

void Frop::Stop()
{
}

void Frop::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    using namespace Update;
    double timeStep = eventData[P_TIMESTEP].GetFloat();
    age_ += timeStep;
    if (age_ > growthStart_ && rootNode_->GetScale().Length() < scale_.Length())
        rootNode_->SetScale(rootNode_->GetScale()+(5.0f * timeStep*(scale_ - rootNode_->GetScale())));
}

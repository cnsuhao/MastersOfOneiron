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

#include "kekelplithf.h"

Kekelplithf::Kekelplithf(Context* context, MasterControl* masterControl, Node* parent, Vector3 pos):
Object(context)
{
    masterControl_ = masterControl;
    randomizer_ = Random(0.5f,0.75f);

    rootNode_ = parent->CreateChild("Imp");
    rootNode_->SetPosition(pos);
    rootNode_->Rotate(Quaternion(0.0f,Random(360.0f),0.0f));
    rootNode_->SetScale(Random(0.015f,0.016f));
    spinNode_= rootNode_->CreateChild("ImpModelNode");
    impModel_ = spinNode_->CreateComponent<AnimatedModel>();
    impModel_->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Kekelplithf.mdl"));
    impModel_->SetMaterial(masterControl_->cache_->GetResource<Material>("Resources/Materials/Kekelplithf.xml"));
    impModel_->SetCastShadows(true);
    impModel_->SetAnimationEnabled(true);

    AnimationController* animCtrl = rootNode_->CreateComponent<AnimationController>();
    animCtrl->PlayExclusive("Resources/Animations/Smoke.ani", 0, true);
    animCtrl->SetSpeed("Resources/Animations/Smoke.ani", 0.5f+randomizer_);

    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Kekelplithf, HandleUpdate));
}

void Kekelplithf::Start()
{
}
void Kekelplithf::Stop()
{
}

void Kekelplithf::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    using namespace Update;
    double timeStep = eventData[P_TIMESTEP].GetFloat();
}

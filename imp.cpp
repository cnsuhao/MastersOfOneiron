#include <Urho3D/Urho3D.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/Animation.h>
#include <Urho3D/Graphics/AnimationState.h>
#include <Urho3D/Graphics/AnimationController.h>

#include "imp.h"

Imp::Imp(Context* context, MasterControl* masterControl, Node* parent, Vector3 pos):
Object(context)
{
    masterControl_ = masterControl;
    randomizer_ = Random(0.5f,0.75f);

    rootNode_ = parent->CreateChild("Imp");
    rootNode_->SetPosition(pos);
    rootNode_->Rotate(Quaternion(0.0f,Random(360.0f),0.0f));
    rootNode_->SetScale(Random(0.015f,0.023f));
    spinNode_= rootNode_->CreateChild("ImpModelNode");
    impModel_ = spinNode_->CreateComponent<AnimatedModel>();
    impModel_->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/ImpClothed_lowpoly.mdl"));
    impModel_->SetMaterial(masterControl_->cache_->GetResource<Material>("Resources/Materials/ImpClothed.xml"));
    impModel_->SetCastShadows(true);
    impModel_->SetAnimationEnabled(true);


    AnimationController* animCtrl = rootNode_->CreateComponent<AnimationController>();
    animCtrl->PlayExclusive("Resources/Animations/Smoke.ani", 0, true);
    animCtrl->SetSpeed("Resources/Animations/Smoke.ani", 0.5f+randomizer_);

    SubscribeToEvent(E_UPDATE, HANDLER(Imp, HandleUpdate));
}

void Imp::Start()
{
}
void Imp::Stop()
{
}

void Imp::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    using namespace Update;
    double timeStep = eventData[P_TIMESTEP].GetFloat();
}

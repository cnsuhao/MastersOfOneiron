#include "imp.h"
#include <Urho3D/Urho3D.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/Animation.h>
#include <Urho3D/Graphics/AnimationState.h>

Imp::Imp(Context* context, MasterControl* masterControl):
Object(context)
{
    masterControl_ = masterControl;
    randomizer_ = Random(0.9f,0.75f);
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    rootNode_ = masterControl_->world.scene->CreateChild("Imp");
    rootNode_->SetPosition(Vector3(Random(-10.0f,10.0f), 0.5f, Random(-10.0f,10.0f)));
    rootNode_->Rotate(Quaternion(0.0f,Random(360.0f),0.0f));
    rootNode_->SetScale(Random(0.1f,0.15f));
    spinNode_= rootNode_->CreateChild("ImpModelNode");
    impModel_ = spinNode_->CreateComponent<AnimatedModel>();
    impModel_->SetModel(cache->GetResource<Model>("Resources/Models/imp.mdl"));
    impModel_->SetMaterial(cache->GetResource<Material>("Resources/Materials/impclothed_lowpol.xml"));
    impModel_->SetCastShadows(true);

    smokeAnim_ = cache->GetResource<Animation>("Resources/Animations/Smoke.ani");
    AnimationState* smokeState = impModel_->AddAnimationState(smokeAnim_);

    if (smokeState)
    {
        smokeState->SetWeight(1.0f);
        smokeState->SetLooped(true);
        smokeState->SetLayer(0);
    }

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
    timeStep *= 5;
    rootNode_->Rotate(Quaternion(0.0f,timeStep*randomizer_,0.0f));
    rootNode_->Translate(0.0f,0.0f,-randomizer_*timeStep*0.1f);

    spinNode_->Rotate(Quaternion(0.0f,23.0f*timeStep*randomizer_,0.0f));
    if (impModel_->GetNumAnimationStates())
    {
            impModel_->GetAnimationStates()[0]->AddTime(100.0f*timeStep);
    }
}

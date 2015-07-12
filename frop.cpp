#include <Urho3D/Urho3D.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/Animation.h>
#include <Urho3D/Graphics/AnimationState.h>

#include "frop.h"

Frop::Frop(Context *context, MasterControl *masterControl, Node *parent, Vector3 pos) : Object(context)
{
    growthStart_ = Random(0.0f, 5.0f);
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

    SubscribeToEvent(E_UPDATE, HANDLER(Frop, HandleUpdate));
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
        rootNode_->SetScale(rootNode_->GetScale()+(timeStep*(scale_ - rootNode_->GetScale())));
}

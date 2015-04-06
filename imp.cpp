#include "imp.h"
#include "common.h"
#include <Urho3D/Urho3D.h>

Imp::Imp(Context* context, MasterControl* masterControl):
Object(context)
{
    masterControl_ = masterControl;
    randomizer_ = Random(0.9f,0.75f);
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    SubscribeToEvent(E_UPDATE, HANDLER(Imp, HandleUpdate));
    rootNode_ = masterControl_->world.scene_->CreateChild("Imp");
    rootNode_->SetPosition(Vector3(Random(-1.0f,1.0f), 0.5f, Random(-1.0f,1.0f)));
    rootNode_->Rotate(Quaternion(0.0f,Random(360.0f),0.0f));
    rootNode_->SetScale(Random(0.01f,0.015f));
    StaticModel* modelObject = rootNode_->CreateComponent<StaticModel>();
    modelObject->SetModel(cache->GetResource<Model>("Resources/Models/imp.mdl"));

    modelObject->SetMaterial(cache->GetResource<Material>("Resources/Materials/impclothed_lowpol.xml"));
    modelObject->SetCastShadows(true);
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
    float timeStep = eventData[P_TIMESTEP].GetFloat();
    timeStep *= 5;
    rootNode_->Rotate(Quaternion(0.0f,timeStep*randomizer_,0.0f));
    rootNode_->Translate(0.0f,0.0f,-randomizer_*timeStep*0.1f);
}

#include "imp.h"
#include "common.h"
#include <Urho3D/Urho3D.h>

Imp::Imp(Context* context, MasterControl* masterControl):
Object(context)
{
    masterControl_ = masterControl;
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    SubscribeToEvent(E_UPDATE, HANDLER(Imp, HandleUpdate));
    rootNode_ = masterControl_->world.scene_->CreateChild("Imp");
    rootNode_->SetPosition(Vector3(Random(-23.0f,23.0f), 0.0f, Random(-23.0f,23.0f)));
    rootNode_->Rotate(Quaternion(0.0f,Random(360.0f),0.0f));
    rootNode_->SetScale(0.1f);
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
    rootNode_->Rotate(Quaternion(0.0f,100.0f*timeStep,0.0f));
}

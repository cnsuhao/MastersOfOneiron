#include <Urho3D/Urho3D.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/Animation.h>
#include <Urho3D/Graphics/AnimationState.h>

#include "grass.h"

Grass::Grass(Context *context, MasterControl *masterControl, Node *parent, Vector3 pos) : Object(context)
{
    masterControl_ = masterControl;
    rootNode_ = parent->CreateChild("Grass");
    rootNode_->SetPosition(pos);
    rootNode_->Rotate(Quaternion(Random(-10.0f, 10.0f),Random(360.0f),Random(-10.0f, 10.0f)));
    float randomWidth = Random(0.5f,1.5f);
    rootNode_->SetScale(randomWidth, Random(0.25f,randomWidth), randomWidth);
    grassModel_ = rootNode_->CreateComponent<StaticModel>();
    grassModel_->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Grass.mdl"));
    grassModel_->SetMaterial(0, masterControl_->cache_->GetResource<Material>("Resources/Materials/block_center.xml"));
    grassModel_->SetMaterial(1, masterControl_->cache_->GetResource<Material>("Resources/Materials/shadow.xml"));
    grassModel_->SetCastShadows(false);

    SubscribeToEvent(E_UPDATE, HANDLER(Grass, HandleUpdate));
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

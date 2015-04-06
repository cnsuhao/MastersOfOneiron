#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>
#include "slot.h"


Slot::Slot(Context *context, Platform* platform, Pair<int, int> coords):
Object(context)
{
    platform_ = platform;
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    SubscribeToEvent(E_UPDATE, HANDLER(Slot, HandleUpdate));
    rootNode_ = platform_->rootNode_->CreateChild("Slot");
    rootNode_->SetPosition(Vector3(coords.first_, 0.0f, coords.second_));
    StaticModel* model_ = rootNode_->CreateComponent<StaticModel>();
    model_->SetModel(cache->GetResource<Model>("Resources/Models/Slot.mdl"));
    model_->SetMaterial(cache->GetResource<Material>("Resources/Materials/glow.xml"));
    model_->SetCastShadows(true);

    cursor_ = platform_->masterControl_->world.cursor.sceneCursor_;
}

void Slot::Start()
{
}
void Slot::Stop()
{
}

void Slot::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    //using namespace Update; float timeStep = eventData[P_TIMESTEP].GetFloat();

    //float elapsedTime = platform_->masterControl_->world.scene_->GetElapsedTime();
    //rootNode_->SetScale(Vector3(1.0f+(0.1f*sinf(2.0f*elapsedTime)),1.0f+(0.1f*sinf(10.0f*elapsedTime)),1.0f+(0.1f*sinf(6.0f*elapsedTime))));

    float cursorDist = Max(0.0f, (rootNode_->GetWorldPosition() - cursor_->GetPosition()).Length()-0.7f);
    float scale = Clamp(1.0f - (0.1f*cursorDist), 0.0f, 1.0f);
    for (int i = 0; i < 3; i ++) scale *= scale;
    rootNode_->SetScale(scale);
    //model_->GetMaterial()->SetShaderParameter("MatDiffColor", Color(0.125f, 1.0f, 1.0f, 0.7f));
}

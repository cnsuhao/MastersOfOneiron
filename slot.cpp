#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>
#include "slot.h"


Slot::Slot(Context *context, Platform* platform, IntVector2 coords):
Object(context)
{
    platform_ = platform;
    coords_ = coords;
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    SubscribeToEvent(E_UPDATE, HANDLER(Slot, HandleUpdate));
    rootNode_ = platform_->rootNode_->CreateChild("Slot");
    rootNode_->SetPosition(Vector3(coords.x_, 0.0f, -coords.y_));
    StaticModel* model = rootNode_->CreateComponent<StaticModel>();
    model->SetModel(cache->GetResource<Model>("Resources/Models/Slot.mdl"));
    model->SetMaterial(cache->GetResource<Material>("Resources/Materials/glow.xml"));
    model->SetCastShadows(false);
    StaticModel* hitModel = rootNode_->CreateComponent<StaticModel>();
    hitModel->SetModel(cache->GetResource<Model>("Resources/Models/SlotHitPlane.mdl"));
    hitModel->SetMaterial(cache->GetResource<Material>("Resources/Materials/invisible.xml"));
    model->SetCastShadows(false);

    cursor_ = platform_->masterControl_->world.cursor.sceneCursor;
}

void Slot::Start()
{
}
void Slot::Stop()
{
}

void Slot::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    //using namespace Update; double timeStep = eventData[P_TIMESTEP].GetFloat();

    //double elapsedTime = platform_->masterControl_->world.scene_->GetElapsedTime();
    //rootNode_->SetScale(Vector3(1.0f+(0.1f*sinf(2.0f*elapsedTime)),1.0f+(0.1f*sinf(10.0f*elapsedTime)),1.0f+(0.1f*sinf(6.0f*elapsedTime))));

    double cursorDist = Max(0.0f, (rootNode_->GetWorldPosition() - cursor_->GetPosition()).Length()-0.7f);
    double scale = Clamp(1.0f - (0.1f*cursorDist), 0.0f, 1.0f);
    for (int i = 0; i < 3; i ++) scale *= scale;
    rootNode_->SetScale(scale);
    //model_->GetMaterial()->SetShaderParameter("MatDiffColor", Color(0.125f, 1.0f, 1.0f, 0.7f));
}

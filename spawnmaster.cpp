
#include "spawnmaster.h"

SpawnMaster::SpawnMaster(Context* context):
    Object(context)
{
}

void SpawnMaster::Activate()
{
    SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(SpawnMaster, HandleSceneUpdate));
}
void SpawnMaster::Deactivate()
{
    UnsubscribeFromAllEvents();
}
void SpawnMaster::Clear()
{
}

void SpawnMaster::Restart()
{
    Clear();
    Activate();
}

void SpawnMaster::HandleSceneUpdate(StringHash eventType, VariantMap &eventData)
{ (void)eventType;

    const float timeStep{ eventData[SceneUpdate::P_TIMESTEP].GetFloat() };
}


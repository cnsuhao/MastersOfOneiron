
#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <Urho3D/Urho3D.h>

#include "mastercontrol.h"
#include "resourcemaster.h"
#include "spawnmaster.h"

class SceneObject : public LogicComponent
{
    URHO3D_OBJECT(SceneObject, LogicComponent);
public:
    SceneObject(Context* context);

    virtual void Set(Vector3 position);
    virtual void OnNodeSet(Node *node);
    virtual void Disable();

    Vector3 GetWorldPosition() const;
protected:
    Vector<SoundSource3D*> sampleSources_;
    float randomizer_;


    void PlaySample(Sound *sample, float gain = 0.3f);
};

#endif // SCENEOBJECT_H


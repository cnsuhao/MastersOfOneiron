
#ifndef RESOURCEMASTER_H
#define RESOURCEMASTER_H

#include <Urho3D/Urho3D.h>
#include "luckey.h"

class ResourceMaster : public Object
{
    URHO3D_OBJECT(ResourceMaster, Object);
public:
    ResourceMaster(Context* context);

    Material* GetMaterial(String name);
    Model* GetModel(String name);
    ParticleEffect*GetParticleEffect(String name);
};

#endif // RESOURCEMASTER_H


#include "resourcemaster.h"



ResourceMaster::ResourceMaster(Context* context) : Object(context)
{
}

Material* ResourceMaster::GetMaterial(String name)
{
    return CACHE->GetResource<Material>("Materials/" + name + ".xml");
}

Model* ResourceMaster::GetModel(String name)
{
    return CACHE->GetResource<Model>("Models/" + name + ".mdl");
}

ParticleEffect* ResourceMaster::GetParticleEffect(String name)
{
    return CACHE->GetResource<ParticleEffect>("Particles/" + name + ".xml");
}

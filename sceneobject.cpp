
#include "sceneobject.h"

SceneObject::SceneObject(Context *context):
    LogicComponent(context),
    randomizer_{Random()}
{
}

void SceneObject::OnNodeSet(Node *node)
{ if (!node) return;

    for (int i{0}; i < 3; ++i){
        sampleSources_.Push(node_->CreateComponent<SoundSource3D>());
        sampleSources_[i]->SetGain(0.3f);
        sampleSources_[i]->SetSoundType(SOUND_EFFECT);
    }
}

void SceneObject::Set(Vector3 position)
{
    node_->SetPosition(position);
    node_->SetEnabledRecursive(true);
}

void SceneObject::Disable()
{
    node_->SetEnabledRecursive(false);
}

void SceneObject::PlaySample(Sound* sample, float gain)
{
    for (SoundSource3D* s : sampleSources_){
        if (!s->IsPlaying()){
            s->SetGain(gain);
            s->Play(sample);
            break;
        }
    }
}

Vector3 SceneObject::GetWorldPosition() const
{
    return node_->GetWorldPosition();
}


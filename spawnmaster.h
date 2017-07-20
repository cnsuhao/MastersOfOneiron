
#ifndef SPAWNMASTER_H
#define SPAWNMASTER_H

#include <Urho3D/Urho3D.h>

#include "mastercontrol.h"

class SpawnMaster : public Object
{
    friend class MasterControl;
    URHO3D_OBJECT(SpawnMaster, Object);
public:
    SpawnMaster(Context* context);

    void Clear();

    template <class T> T* Create(bool recycle = true)
    {
        T* created{ nullptr };

        if (recycle) {
            PODVector<Node*> correctType{};
            MC->GetScene()->GetChildrenWithComponent<T>(correctType);
            for (Node* n : correctType) {

                if (!n->IsEnabled()) {
                    created = n->GetComponent<T>();
                    break;
                }
            }
        }
        if(!created) {

            Node* spawnedNode{ MC->GetScene()->CreateChild(T::GetTypeStatic().ToString()) };
            created = spawnedNode->CreateComponent<T>();
            spawnedNode->SetEnabledRecursive(false);
        }

        return created;
    }

    template <class T> int CountActive()
    {
        int count{0};
        PODVector<Node*> result{};
        MC->GetScene()->GetChildrenWithComponent<T>(result);

        for (Node* r : result) {

            if (r->IsEnabled()) ++count;
        }
        return count;
    }

private:
    void Activate();
    void Deactivate();
    void Restart();

    void HandleSceneUpdate(StringHash eventType, VariantMap &eventData);
};

#endif // SPAWNMASTER_H


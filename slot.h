#pragma once

#include "platform.h"
#include <Urho3D/Urho3D.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Core/CoreEvents.h>

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class Slot : public Object
{
    OBJECT(Slot);
public:
    Slot(Context *context, Platform* platform, Pair<int, int> coords);

    virtual void Start();
    virtual void Stop();

private:
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    Platform* platform_;
    Node* rootNode_;
    StaticModel* model_;
    Node* cursor_;
};

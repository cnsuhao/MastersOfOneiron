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
    friend class Platform;
public:
    IntVector2 coords_;
    Slot(Context *context, Platform* platform, IntVector2 coords);

    virtual void Start();
    virtual void Stop();

private:
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    Platform* platform_;
    Node* rootNode_;
    //StaticModel* model_;
    Node* cursor_;
};

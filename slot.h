/* Masters of Oneiron
// Copyright (C) 2016 LucKey Productions (luckeyproductions.nl)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef SLOT_H
#define SLOT_H

#include <Urho3D/Urho3D.h>

#include "sceneobject.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class Slot : public SceneObject
{
    URHO3D_OBJECT(Slot, SceneObject);
    friend class Platform;
public:
    Slot(Context *context);
    static void RegisterObject(Context* context);

    IntVector2 coords_;
    virtual void Start();
    virtual void Stop();

    virtual void OnNodeSet(Node* node);
    virtual void Set(IntVector2 coords, Platform* platform);

    Platform* GetPlatform() const { return platform_; }
private:
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    Platform* platform_;
    //StaticModel* model_;
    Node* cursor_;
};

#endif // SLOT_H

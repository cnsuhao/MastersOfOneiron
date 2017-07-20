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

#ifndef GRASS_H
#define GRASS_H

#include <Urho3D/Urho3D.h>

#include "sceneobject.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class Grass : public SceneObject
{
    URHO3D_OBJECT(Grass, SceneObject);
public:
    Grass(Context *context);
    static void RegisterObject(Context* context);
    virtual void Start();
    virtual void Stop();
    void OnNodeSet(Node* node);
    void Set(Vector3 position, Node* parent);
private:
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    StaticModel* grassModel_;
    double randomizer_;
};

#endif // GRASS_H

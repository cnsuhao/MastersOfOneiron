/* Masters of Oneiron
// Copyright (C) 2015 LucKey Productions (luckeyproductions.nl)
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

#include "mastercontrol.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class Grass : public Object
{
    OBJECT(Grass);
public:
    Grass(Context *context, MasterControl* masterControl, Urho3D::Node *parent, Vector3 pos);
    Grass(Context* context, MasterControl* masterControl): Grass(context, masterControl, masterControl->world.scene, Vector3::ZERO){}
    virtual void Start();
    virtual void Stop();
private:
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    MasterControl* masterControl_;
    Node* rootNode_;
    StaticModel* grassModel_;
    double randomizer_;
};

#endif // GRASS_H

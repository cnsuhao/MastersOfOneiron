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

#ifndef SLOT_H
#define SLOT_H

#include <Urho3D/Urho3D.h>

#include "platform.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class Slot : public Object
{
    URHO3D_OBJECT(Tile, Object);
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

#endif // SLOT_H

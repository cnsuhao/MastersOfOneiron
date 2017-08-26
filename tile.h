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

#ifndef TILE_H
#define TILE_H

#include <Urho3D/Urho3D.h>

#include "sceneobject.h"
#include "platform.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class Platform;
//class BuildingType;

class Tile : public SceneObject
{
    friend class Platform;
    URHO3D_OBJECT(Tile, SceneObject);
public:
    Tile(Context *context);
    static void RegisterObject(Context* context);
    virtual void Set(const IntVector2 coords, Platform *platform);

    virtual void Start();
    virtual void Stop();


    IntVector2 coords_;
    BuildingType buildingType_;
    float GetHealth() const { return health_; }
    void ApplyDamage(float damage) { health_ = Max(health_ - damage, 0.0f); }
    void OnNodeSet(Node* node);
    void Disable();
    static Vector3 ElementPosition(TileElement element);
private:
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    Platform* platform_;
    Node* elements_[TE_LENGTH];
    CollisionShape* colliders_[TE_LENGTH];
    float health_;
    void SetBuilding(BuildingType type);
    BuildingType GetBuilding();
    void FixFringe();

};

#endif // TILE_H

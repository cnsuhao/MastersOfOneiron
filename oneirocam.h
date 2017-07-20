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

#ifndef ONEIROCAM_H
#define ONEIROCAM_H

#include <Urho3D/Urho3D.h>

#include "sceneobject.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
class Viewport;
class RenderPath;
class Camera;
}

class OneiroCam : public SceneObject
{
    URHO3D_OBJECT(OneiroCam, SceneObject);
public:
    OneiroCam(Context *context);
    static void RegisterObject(Context* context);
    void OnNodeSet(Node* node);

    virtual void Start();
    virtual void Stop();

    SharedPtr<Camera> camera_;
    SharedPtr<Viewport> viewport_;
    SharedPtr<RenderPath> effectRenderPath;

    Vector3 GetWorldPosition();
    Quaternion GetRotation();
    void Update(float timeStep);
    void Lock(Platform* platform);
private:

    Node* pitchNode_;
    Node* altitudeNode_;

    float yaw_;
    float pitch_;
    float roll_;
    float yawDelta_;
    float pitchDelta_;
    float speedMultiplier_;

    Vector3 camTrans_;

    void SetupViewport();

};

#endif // ONEIROCAM_H

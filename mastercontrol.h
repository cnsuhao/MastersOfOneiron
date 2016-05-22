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

#ifndef MASTERCONTROL_H
#define MASTERCONTROL_H

#include <Urho3D/Urho3D.h>

#include "luckey.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class OneiroCam;
class InputMaster;
class Platform;

typedef struct GameWorld
{
    SharedPtr<OneiroCam> camera;
    SharedPtr<Scene> scene;
    SharedPtr<Node> backgroundNode;
    SharedPtr<Node> voidNode;
    SharedPtr<Node> sunNode;
    struct {
        SharedPtr<Node> sceneCursor;
        SharedPtr<Cursor> uiCursor;
        PODVector<RayQueryResult> hitResults;
    } cursor;
} GameWorld;

typedef struct HitInfo
{
    Vector3 position_;
    Vector3 hitNormal_;
    Node* hitNode_;
    Drawable* drawable_;
} HitInfo;

namespace {
StringHash const N_VOID = StringHash("Void");
StringHash const N_CURSOR = StringHash("Cursor");
StringHash const N_TILEPART = StringHash("TilePart");
StringHash const N_SLOT = StringHash("Slot");
}

#define MC MasterControl::GetInstance()
#define WORLDRADIUS 235.0f

class MasterControl : public Application
{
    URHO3D_OBJECT(MasterControl, Application);
    friend class InputMaster;
public:
    MasterControl(Context* context);
    static MasterControl* GetInstance();

    GameWorld world;
    SharedPtr<ResourceCache> cache_;
    SharedPtr<Graphics> graphics_;

    HashMap<unsigned, SharedPtr<Platform> > platformMap_;

    virtual void Setup();
    virtual void Start();
    virtual void Stop();
    void Exit();

    float Sine(const float freq, const float min, const float max, const float shift = 0.0f);
    float Cosine(const float freq, const float min, const float max, const float shift = 0.0f);
private:
    static MasterControl* instance_;

    bool paused_;

    SharedPtr<UI> ui_;
    SharedPtr<Renderer> renderer_;
    SharedPtr<XMLFile> defaultStyle_;

    void SetWindowTitleAndIcon();
    void CreateConsoleAndDebugHud();

    void CreateScene();
    void CreateUI();
    void SubscribeToEvents();

    void HandleSceneUpdate(StringHash eventType, VariantMap& eventData);
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData);

    void CreatePlatform(const Vector3 pos);
    void UpdateCursor(float timeStep);
    bool CursorRayCast(double maxDistance, PODVector<RayQueryResult> &hitResults);
};

#endif // MASTERCONTROL_H

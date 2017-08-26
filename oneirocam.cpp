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

#include "platform.h"

#include "oneirocam.h"

void OneiroCam::RegisterObject(Context *context)
{
    context->RegisterFactory<OneiroCam>();
}

OneiroCam::OneiroCam(Context *context):
    SceneObject(context),
    yaw_{0.0f},
    pitch_{0.0f},
    roll_{0.0f},
    yawDelta_{0.0f},
    pitchDelta_{0.0f},
    speedMultiplier_{1.0f},
    camTrans_{}
{

}

void OneiroCam::OnNodeSet(Node *node)
{ if (!node) return;

    //Create the camera. Limit far clip distance to match the fog
    altitudeNode_ = node_->CreateChild("CamTrans");
    pitchNode_ = altitudeNode_->CreateChild("CamRot");
    camera_ = pitchNode_->CreateComponent<Camera>();
    camera_->SetFarClip(128.0f);
    camera_->SetFov(100.0f);
    //Set an initial position for the camera scene node above the origin
    altitudeNode_->SetPosition(Vector3(0.0f, -WORLD_RADIUS + 5.0f, 0.0f));
    pitchNode_->SetRotation(Quaternion(0.0f, 90.0f, 0.0f));

//    Light* light{rotationNode_->CreateComponent<Light>()};
//    light->SetLightType(LIGHT_DIRECTIONAL);
//    light->SetBrightness(0.23f);

    SetupViewport();
}

void OneiroCam::Start()
{
}

void OneiroCam::Stop()
{
}

void OneiroCam::SetupViewport()
{
//    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer{GetSubsystem<Renderer>()};
    renderer->SetShadowQuality(SHADOWQUALITY_PCF_24BIT);

    //Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
    SharedPtr<Viewport> viewport{new Viewport(context_, MC->world.scene, camera_)};
    viewport_ = viewport;

    //Add anti-asliasing
    effectRenderPath = viewport_->GetRenderPath()->Clone();
    effectRenderPath->Append(MC->CACHE->GetResource<XMLFile>("PostProcess/FXAA3.xml"));
    /*
    effectRenderPath->Append(masterControl_->cache_->GetResource<XMLFile>("PostProcess/AutoExposure.xml"));
    effectRenderPath->SetShaderParameter("AutoExposureLumRange", Vector2(0.42f, 2.0f));
    effectRenderPath->SetShaderParameter("AutoExposureAdaptRate", 5.0f);
    effectRenderPath->SetShaderParameter("AutoExposureMiddleGrey", 0.42f);
    */
    effectRenderPath->Append(MC->CACHE->GetResource<XMLFile>("PostProcess/Bloom.xml"));
    effectRenderPath->SetShaderParameter("BloomThreshold", 0.5f);
    effectRenderPath->SetShaderParameter("BloomMix", Vector2(0.75f, 1.0f));

    viewport_->SetRenderPath(effectRenderPath);
    renderer->SetViewport(0, viewport);
}

Vector3 OneiroCam::GetWorldPosition()
{
    return altitudeNode_->GetWorldPosition();
}

Quaternion OneiroCam::GetRotation()
{
    return pitchNode_->GetRotation();
}

void OneiroCam::Update(float timeStep)
{
    camera_->SetFarClip(WORLD_RADIUS * 2.5f);

    speedMultiplier_ = 1.0f + INPUT->GetKeyDown(KEY_SHIFT);
    //Movement speed as world units per second
    const float moveSpeed{ 42.0f * speedMultiplier_};
    //Mouse sensitivity as degrees per pixel
    const float mouseSensitivity{ 0.1f };

    //Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees. Only move the camera when the cursor is hidden.
    IntVector2 mouseMove{ INPUT->GetMouseMove() };
    yawDelta_ = 0.5f * (yawDelta_ + mouseSensitivity * mouseMove.x_);
    pitchDelta_ = 0.5f * (pitchDelta_ + mouseSensitivity * mouseMove.y_);
    yaw_ += yawDelta_;
    pitch_ += pitchDelta_;
    pitch_ = Clamp(pitch_, -89.0f, 89.0f);

    camTrans_ = camTrans_.Lerp(Vector3::ZERO, Min(timeStep * 5.0f, 1.0f));
    camTrans_.y_ += 0.1f * INPUT->GetMouseMoveWheel();

    //Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
    if (INPUT->GetKeyDown('D') || INPUT->GetKeyDown('A'))
        camTrans_.x_ = Lerp(camTrans_.x_, 1.0f * INPUT->GetKeyDown('D') - INPUT->GetKeyDown('A'), timeStep);
    if (INPUT->GetKeyDown('E') || INPUT->GetKeyDown('Q'))
        camTrans_.y_ = Lerp(camTrans_.y_, 1.0f * INPUT->GetKeyDown('E') - INPUT->GetKeyDown('Q'), timeStep);
    if (INPUT->GetKeyDown('W') || INPUT->GetKeyDown('S'))
        camTrans_.z_ = Lerp(camTrans_.z_, 1.0f * INPUT->GetKeyDown('W') - INPUT->GetKeyDown('S'), timeStep);

    node_->Rotate(Quaternion(-camTrans_.z_ * timeStep * moveSpeed,
                             IsOut() ? -yawDelta_ : yawDelta_,
                             IsOut() ? -camTrans_.x_ : camTrans_.x_ * timeStep * moveSpeed));
    altitudeNode_->Translate(camTrans_.y_ * moveSpeed * timeStep * Vector3::UP, TS_PARENT);
    pitchNode_->SetRotation(Quaternion(pitch_, 0.0f, 0.0f));

    camTrans_ = Quaternion(IsOut() ? -yawDelta_ : yawDelta_, Vector3::UP) * camTrans_;

    //Flip camera when leaving or entering the bubble
    altitudeNode_->SetRotation(altitudeNode_->GetRotation().Slerp(Quaternion(IsOut() ? 180.0f : 0.0f, Vector3::FORWARD), timeStep * 5.0f));

}
bool OneiroCam::IsOut()
{
    return altitudeNode_->GetPosition().Length() > WORLD_RADIUS;
}

void OneiroCam::Lock(Platform* platform)
{
    if (altitudeNode_->GetParent() == MC->world.scene)
    {
        altitudeNode_->SetParent(platform->GetNode());
        pitchNode_->SetParent(altitudeNode_);
    }
    else {
        Vector3 worldPos{ GetWorldPosition()};
        altitudeNode_->SetParent(MC->world.scene);
        altitudeNode_->SetPosition(worldPos);
    }
}

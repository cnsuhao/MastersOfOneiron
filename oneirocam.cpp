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

#include "oneirocam.h"

OneiroCam::OneiroCam(Context *context, MasterControl *masterControl):
    Object(context),
    yaw_{0.0f},
    pitch_{0.0f},
    roll_{0.0f},
    yawDelta_{0.0f},
    pitchDelta_{0.0f},
    forceMultiplier_{1.0f}
{
    masterControl_ = masterControl;
    SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(OneiroCam, HandleSceneUpdate));

    //Create the camera. Limit far clip distance to match the fog
    translationNode_ = masterControl_->world.scene->CreateChild("CamTrans");
    rotationNode_ = translationNode_->CreateChild("CamRot");
    camera_ = rotationNode_->CreateComponent<Camera>();
    camera_->SetFarClip(128.0f);
    camera_->SetFov(80.0f);
    //Set an initial position for the camera scene node above the origin
    translationNode_->SetPosition(Vector3(0.0f, 3.0f, -64.0f));
    rotationNode_->SetRotation(Quaternion(0.0f, 90.0f, 0.0f));
    rigidBody_ = translationNode_->CreateComponent<RigidBody>();
    rigidBody_->SetAngularFactor(Vector3::ZERO);
    CollisionShape* collisionShape{translationNode_->CreateComponent<CollisionShape>()};
    collisionShape->SetSphere(0.1f);
    rigidBody_->SetMass(1.0f);

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
    SharedPtr<Viewport> viewport{new Viewport(context_, masterControl_->world.scene, camera_)};
    viewport_ = viewport;

    //Add anti-asliasing
    effectRenderPath = viewport_->GetRenderPath()->Clone();
    effectRenderPath->Append(masterControl_->cache_->GetResource<XMLFile>("PostProcess/FXAA3.xml"));
    effectRenderPath->SetEnabled("FXAA3", true);
    effectRenderPath->Append(masterControl_->cache_->GetResource<XMLFile>("PostProcess/Bloom.xml"));
    effectRenderPath->SetShaderParameter("BloomThreshold", 0.5f);
    effectRenderPath->SetShaderParameter("BloomMix", Vector2(0.75f, 1.0f));

    viewport_->SetRenderPath(effectRenderPath);
    renderer->SetViewport(0, viewport);
}

Vector3 OneiroCam::GetWorldPosition()
{
    return translationNode_->GetWorldPosition();
}

Quaternion OneiroCam::GetRotation()
{
    return rotationNode_->GetRotation();
}

void OneiroCam::HandleSceneUpdate(StringHash eventType, VariantMap &eventData)
{
    camera_->SetFarClip(translationNode_->GetPosition().Length() + WORLDRADIUS);

    //Take the frame time step, which is stored as a float
    const float t{eventData[Update::P_TIMESTEP].GetFloat()};
    //Movement speed as world units per second
    const float moveSpeed{2000.0f};
    //Mouse sensitivity as degrees per pixel
    const float mouseSensitivity{0.1f};

    //Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees. Only move the camera when the cursor is hidden.
    Input* input{GetSubsystem<Input>()};
    IntVector2 mouseMove{input->GetMouseMove()};
    yawDelta_ = 0.5f * (yawDelta_ + mouseSensitivity * mouseMove.x_);
    pitchDelta_ = 0.5f * (pitchDelta_ + mouseSensitivity * mouseMove.y_);
    yaw_ += yawDelta_;
    pitch_ += pitchDelta_;
    pitch_ = Clamp(pitch_, -89.0f, 89.0f);
    //Construct new orientation for the camera scene node from yaw and pitch. Roll is fixed to zero
    translationNode_->SetRotation(Quaternion(0.0f, 0.0f, 0.0f));
    rotationNode_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));

    //Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
    Vector3 camForce{Vector3::ZERO};
//    if (input->GetKeyDown('W')) camForce += LucKey::Scale( rotationNode_->GetDirection(), Vector3{1.0f,0.0f,1.0f} ).Normalized();
    if (input->GetKeyDown('W'))translationNode_->RotateAround(MC->world.sunNode->GetPosition(), Quaternion(t * 23.0f, rotationNode_->GetRight()), TS_WORLD);
    if (input->GetKeyDown('S'))translationNode_->RotateAround(MC->world.sunNode->GetPosition(), Quaternion(t * -23.0f, rotationNode_->GetRight()), TS_WORLD);
    if (input->GetKeyDown('D'))translationNode_->RotateAround(MC->world.sunNode->GetPosition(), Quaternion(t * 23.0f, rotationNode_->GetDirection()), TS_WORLD);
    if (input->GetKeyDown('A'))translationNode_->RotateAround(MC->world.sunNode->GetPosition(), Quaternion(t * -23.0f, rotationNode_->GetDirection()), TS_WORLD);

    if (input->GetKeyDown('S')) camForce += LucKey::Scale( rotationNode_->GetDirection(), Vector3{-1.0f,0.0f,-1.0f} ).Normalized();
    if (input->GetKeyDown('D')) camForce += LucKey::Scale( rotationNode_->GetRight(), Vector3{1.0f,0.0f,1.0f} ).Normalized();
    if (input->GetKeyDown('A')) camForce += LucKey::Scale( rotationNode_->GetRight(), Vector3{-1.0f,0.0f,-1.0f} ).Normalized();
    if (input->GetKeyDown('E')) camForce += Vector3::UP;
    if (input->GetKeyDown('Q')) camForce += Vector3::DOWN;
    camForce = camForce.Normalized() * moveSpeed * t;

    if ( forceMultiplier_ < 8.0f && (input->GetKeyDown(KEY_LSHIFT)||input->GetKeyDown(KEY_RSHIFT)) ){
        forceMultiplier_ += 0.23f;
    } else {
        forceMultiplier_ = pow(forceMultiplier_, 0.75f);
    }
    rigidBody_->ApplyForce( (forceMultiplier_ * camForce) - (2.3f * rigidBody_->GetLinearVelocity()) );

//    if (translationNode_->GetPosition().y_ < 1.0f)
//    {
//        translationNode_->SetPosition(Vector3(translationNode_->GetPosition().x_, 1.0f, translationNode_->GetPosition().z_));
//        rigidBody_->SetLinearVelocity(Vector3(rigidBody_->GetLinearVelocity().x_, 0.0f, rigidBody_->GetLinearVelocity().z_));
//    }
}

void OneiroCam::Lock(SharedPtr<Platform> platform)
{
    if (translationNode_->GetParent() == masterControl_->world.scene)
    {
        translationNode_->SetParent(platform->rootNode_);
        rotationNode_->SetParent(translationNode_);
    }
    else {
        Vector3 worldPos{GetWorldPosition()};
        translationNode_->SetParent(masterControl_->world.scene);
        translationNode_->SetPosition(worldPos);
    }
}

#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Physics/RigidBody.h>

#include "platform.h"
#include "tile.h"
#include "slot.h"

Platform::Platform(Context *context, Vector3 position, MasterControl* masterControl):
Object(context)
{
    masterControl_ = masterControl;
    SubscribeToEvent(E_UPDATE, HANDLER(Platform, HandleUpdate));
    rootNode_ = masterControl_->world.scene_->CreateChild("Platform");
    rootNode_->SetPosition(position);
    rootNode_->SetRotation(Quaternion(0.0f, Random(360.0f), 0.0f));
    rigidBody_ = rootNode_->CreateComponent<RigidBody>();
    rigidBody_->SetMass(1.0f);
    rigidBody_->SetFriction(0.0f);
    // Add base tile
    Pair<int, int> firstCoordPair = Pair<int, int>(0,0);
    tileMap_[firstCoordPair] = new Tile(context_, firstCoordPair, this);
    // Add random tiles
    int addedTiles = 1;
    int platformSize = Random(2, 16);
    while (addedTiles < platformSize){
        bool foundNeighbour = false;
        const Pair<int, int> coordPair = Pair<int, int>(Random(-platformSize/2,platformSize/2),Random(-platformSize/2,platformSize/2));
        const Pair<int, int> xMirroredPair = Pair<int, int>(-coordPair.first_,coordPair.second_);
        Vector<Pair<int,int>> tileCoords = tileMap_.Keys();
        if (tileCoords.Contains(coordPair) || tileCoords.Contains(coordPair)) continue;

        for (int n = 1; n <= 4; n++){
            int dx;
            int dy;
            switch (n){
            case 1: dx =  0; dy =  1; break;
            case 2: dx =  1; dy =  0; break;
            case 3: dx =  0; dy = -1; break;
            case 4: dx = -1; dy =  0; break;
            }
            Pair<int,int> checkCoords = Pair<int,int>(coordPair.first_+dx, coordPair.second_+dy);
                Pair<int,int> checkXMirroredCoords = Pair<int,int>(xMirroredPair.first_+dx, xMirroredPair.second_+dy);
                if (tileCoords.Contains(checkCoords)||tileCoords.Contains(checkXMirroredCoords)) foundNeighbour = true;
        }

        if (foundNeighbour){
            addedTiles++;
            tileMap_[coordPair] = new Tile(context_, coordPair, this);
            tileMap_[xMirroredPair] = new Tile(context_, xMirroredPair, this);
        }
    }

    //Add slots
    AddMissingSlots();

    rigidBody_->ApplyForce(Vector3(Random(-100.0f,100.0f), 0.0f, Random(-100.0f,100.0f)));
    rigidBody_->ApplyTorque(Vector3(0.0f, Random(-16.0f, 16.0f), 0.0f));
}

void Platform::Start()
{

}
void Platform::Stop()
{
}

void Platform::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    AddMissingSlots();
    using namespace Update; float timeStep = 100.0f * eventData[P_TIMESTEP].GetFloat();
    //Push object towards y-zeroplane
    rigidBody_->ApplyForce(timeStep * rigidBody_->GetMass() *
                           Vector3(-0.5f*rigidBody_->GetLinearVelocity().x_,
                                   -5.0f*(rootNode_->GetPosition().y_+rigidBody_->GetLinearVelocity().y_),
                                   -0.5f*rigidBody_->GetLinearVelocity().z_));
    //Ease object into hoirzontal allignment and apply angular friction at the same time
    rigidBody_->ApplyTorque(timeStep * rigidBody_->GetMass() *
                            (rigidBody_->GetAngularVelocity()*Vector3(-1.5f, -0.1f, -1.5f)) +
                            (0.5f*(Quaternion::IDENTITY - rootNode_->GetRotation()).EulerAngles()*Vector3(1.0f, 0.0f, 1.0f)));
                            //rootNode_->GetRotation().Slerp(Quaternion::IDENTITY, timeStep).EulerAngles()*Vector3(0.1f, 0.0f, 0.1f));
}

void Platform::AddMissingSlots()
{
    Vector<Pair<int,int>> tileCoords = tileMap_.Keys();
    for (uint nthTile = 0; nthTile < tileCoords.Size(); nthTile++){
        for (int n = 0; n <= 4; n++){
            int dx = 0;
            int dy = 0;
            switch (n){
            case 1: dx =  1; dy =  0; break;
            case 2: dx = -1; dy =  0; break;
            case 3: dx =  0; dy =  1; break;
            case 4: dx =  0; dy = -1; break;
            default: break;
            }
            Pair<int,int> checkCoords = Pair<int,int>(tileCoords[nthTile].first_+dx, tileCoords[nthTile].second_+dy);
            if (!slotMap_[checkCoords]) slotMap_[checkCoords] = new Slot(context_, this, checkCoords);
        }
        if (!slotMap_[tileCoords[nthTile]]) new Slot(context_, this, tileCoords[nthTile]);
    }
}

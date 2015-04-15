#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Input/Input.h>

#include "tile.h"

Tile::Tile(Context *context, const IntVector2 coords, Platform *platform):
Object(context)
{
    masterControl_ = platform->masterControl_;
    platform_ = platform;
    coords_ = coords;

    rootNode_ = platform_->rootNode_->CreateChild("Tile");
    rootNode_->SetPosition(Vector3((double)coords_.x_, 0.0f, -(double)coords_.y_));
    //Increase platform mass
    platform_->rigidBody_->SetMass(platform_->rigidBody_->GetMass()+1.0f);
    //Add collision shape to platform
    collisionShape_ = platform->rootNode_->CreateComponent<CollisionShape>();
    collisionShape_->SetBox(Vector3::ONE);
    collisionShape_->SetPosition(Vector3(coords_.x_, 0.0f, -coords_.y_));

    //platform_->rigidBody_->EnableMassUpdate();

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    for (int i = 0; i <= 8; i++){
        //Set up center and edge nodes in this order:
        String name = "TilePart";
        /*switch (i){ //Do I care?
        case TE_CENTER:    name += "Center";break;
        case TE_NORTH:     name += "Edge.N";break;
        case TE_EAST:      name += "Edge.E";break;
        case TE_SOUTH:     name += "Edge.S";break;
        case TE_WEST:      name += "Edge.W";break;
        case TE_NORTHEAST: name += "Corner.NW";break;
        case TE_SOUTHEAST: name += "Corner.NE";break;
        case TE_SOUTHWEST: name += "Corner.SE";break;
        case TE_NORTHWEST: name += "Corner.SW";break;
        default:break;
        }*/
        elements_[i] = rootNode_->CreateChild(name);
        int nthOfType = ((i-1)%4);
        if (i > 0) elements_[i]->Rotate(Quaternion(0.0f, 90.0f-nthOfType*90.0f, 0.0f));
        //Add the right model to the node
        StaticModel* model = elements_[i]->CreateComponent<StaticModel>();
        switch (i){
        case TE_CENTER:    model->SetModel(cache->GetResource<Model>("Resources/Models/Block_center.mdl"));break;
        case TE_NORTH:case TE_EAST:case TE_SOUTH:case TE_WEST:
            model->SetModel(cache->GetResource<Model>("Resources/Models/Block_side.mdl"));break;
        case TE_NORTHEAST:case TE_SOUTHEAST:case TE_SOUTHWEST:case TE_NORTHWEST:
            model->SetModel(cache->GetResource<Model>("Resources/Models/Block_outcorner.mdl"));break;
                default:break;
        }

        model->SetMaterial(cache->GetResource<Material>("Resources/Materials/block_center.xml"));
        model->SetCastShadows(true);
    }

    SubscribeToEvent(E_UPDATE, HANDLER(Tile, HandleUpdate));
}

void Tile::Start()
{
}
void Tile::Stop()
{
}

void Tile::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    Input* input = GetSubsystem<Input>();
    using namespace Update; double timeStep = eventData[P_TIMESTEP].GetFloat();
    if (buildingType_ == B_ENGINE && input->GetKeyDown(KEY_UP)){
        platform_->rigidBody_->ApplyForce(platform_->rootNode_->GetRotation() * rootNode_->GetDirection() * 500.0f*timeStep, platform_->rootNode_->GetRotation() * rootNode_->GetPosition());
    }
}

void Tile::SetBuilding(BuildingType type)
{
    buildingType_ = type;
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    StaticModel* model = elements_[0]->GetComponent<StaticModel>();
    switch (buildingType_)
    {
    case B_ENGINE: {
        model->SetModel(cache->GetResource<Model>("Resources/Models/Engine_center.mdl"));
        model->SetMaterial(0,cache->GetResource<Material>("Resources/Materials/block_center.xml"));
        model->SetMaterial(1,cache->GetResource<Material>("Resources/Materials/solid.xml"));
        model->SetMaterial(2,cache->GetResource<Material>("Resources/Materials/glow.xml"));
        model->SetMaterial(3,cache->GetResource<Material>("Resources/Materials/glass.xml"));
    } break;
    default: break;
    }
}

BuildingType Tile::GetBuilding()
{
    return buildingType_;
}


























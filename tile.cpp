#include "tile.h"
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Urho3D.h>

Tile::Tile(Context *context, const Pair<int, int> coords, Platform *platform):
Object(context)
{
    masterControl_ = platform->masterControl_;
    platform_ = platform;
    coords_ = coords;

    rootNode_ = platform_->rootNode_->CreateChild("Tile");
    rootNode_->SetPosition(Vector3((float)coords_.first_, 0.0f, (float)coords_.second_));
    //Increase platform mass
    platform_->rigidBody_->SetMass(platform_->rigidBody_->GetMass()+1.0f);
    //Add collision shape to platform
    collisionShape_ = platform->rootNode_->CreateComponent<CollisionShape>();
    collisionShape_->SetBox(Vector3::ONE);
    collisionShape_->SetPosition(Vector3(coords_.first_, 0.0f, coords_.second_));

    platform_->rigidBody_->EnableMassUpdate();

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    for (int i = 0; i <= 8; i++){
        //Set up center and edge nodes in this order:
        String name = "TilePart_";
        switch (i){
        case CENTER:    name += "Center";break;
        case NORTH:     name += "Edge.N";break;
        case EAST:      name += "Edge.E";break;
        case SOUTH:     name += "Edge.S";break;
        case WEST:      name += "Edge.W";break;
        case NORTHEAST: name += "Corner.NW";break;
        case SOUTHEAST: name += "Corner.NE";break;
        case SOUTHWEST: name += "Corner.SE";break;
        case NORTHWEST: name += "Corner.SW";break;
        default:break;
        }
        elements_[i] = rootNode_->CreateChild(name);
        int nthOfType = ((i-1)%4);
        if (i > 0) elements_[i]->Rotate(Quaternion(0.0f, nthOfType*90.0f, 0.0f));
        //Add the right model to the node
        StaticModel* model = elements_[i]->CreateComponent<StaticModel>();
        switch (i){
        case CENTER:    model->SetModel(cache->GetResource<Model>("Resources/Models/Block_center.mdl"));break;
        case NORTH:case EAST:case SOUTH:case WEST:
            model->SetModel(cache->GetResource<Model>("Resources/Models/Block_side.mdl"));break;
        case NORTHEAST:case SOUTHEAST:case SOUTHWEST:case NORTHWEST:
            model->SetModel(cache->GetResource<Model>("Resources/Models/Block_outcorner.mdl"));break;
                default:break;
        }

        model->SetMaterial(cache->GetResource<Material>("Materials/StoneTiled.xml"));
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
    using namespace Update; float timeStep = eventData[P_TIMESTEP].GetFloat();
}
































#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/OctreeQuery.h>
#include <Urho3D/IO/FileSystem.h>

#include "inputmaster.h"
#include "platform.h"


InputMaster::InputMaster(Context* context, MasterControl* masterControl) : Object(context)
{
    masterControl_ = masterControl;
    input_ = GetSubsystem<Input>();
    //Subscribe mouse down event
    SubscribeToEvent(E_MOUSEBUTTONDOWN, HANDLER(InputMaster, HandleMouseDown));
    //Subscribe key down event.
    SubscribeToEvent(E_KEYDOWN, HANDLER(InputMaster, HandleKeyDown));
}

void InputMaster::HandleMouseDown(StringHash eventType, VariantMap &eventData)
{
    using namespace MouseButtonDown;
    int button = eventData[P_BUTTON].GetInt();
    if (button == MOUSEB_LEFT){
        //See through cursor
        int first = 0;
        if (masterControl_->world.cursor.hitResults[first].node_->GetNameHash() == N_CURSOR) first = 1;
        SharedPtr<Node> firstHit = SharedPtr<Node>(masterControl_->world.cursor.hitResults[first].node_);
        //Void interaction (create new platform)
        if (firstHit->GetNameHash() == N_VOID){
            new Platform(context_, masterControl_->world.cursor.sceneCursor->GetPosition(), masterControl_);
        }
        //Platform selection
        else if (firstHit->GetNameHash() == N_TILEPART)
        {
            //Select single platform
            if (!(input_->GetKeyDown(KEY_LSHIFT)||input_->GetKeyDown(KEY_RSHIFT)))
            {
                SharedPtr<Platform> platform = masterControl_->platformMap_[firstHit->GetParent()->GetParent()->GetID()];
                SetSelection(platform);
            }
            //Add platform to selection when either of the shift keys is held down
            else
            {
                SharedPtr<Platform> platform = masterControl_->platformMap_[firstHit->GetParent()->GetParent()->GetID()];
                platform->SetSelected(!platform->GetSelected());
                selectedPlatforms_ += platform;
            }
        }
        //Building interaction, if platform was already selected
        //Slot interaction, if Former was selected
        else if (firstHit->GetNameHash() == N_SLOT)
        {
            SharedPtr<Platform> platform = masterControl_->platformMap_[firstHit->GetParent()->GetID()];
            IntVector2 coords = IntVector2(firstHit->GetPosition().x_, -firstHit->GetPosition().z_);
            if (platform->CheckEmpty(coords, true))
            {
                //Add tile
                platform->AddTile(coords);
                platform->FixFringe();
                platform->AddMissingSlots();
            }
            else {
                //Add engine row
                while (!platform->CheckEmpty(coords, true)){
                    platform->SetBuilding(coords, B_ENGINE);
                    coords += IntVector2(0,1);
                }
            }
        }
    }
    else if (button == MOUSEB_RIGHT){
        //Platform move command for each selected platform
    }
}

void InputMaster::SetSelection(SharedPtr<Platform> platform)
{
    DeselectAll();
    selectedPlatforms_ += platform;
    platform->Select();
}

void InputMaster::HandleMouseUp(StringHash eventType, VariantMap &eventData)
{
    using namespace MouseButtonUp;
    int button = eventData[P_BUTTON].GetInt();
    if (button == MOUSEB_LEFT) {}//Deselect when mouse did not move during click on N_VOID
}

void InputMaster::HandleKeyDown(StringHash eventType, VariantMap &eventData)
{
    using namespace KeyDown;
    int key = eventData[P_KEY].GetInt();

    //Exit when ESC is pressed
    if (key == KEY_ESC) DeselectAll();//masterControl_->Exit();

    //Take screenshot
    else if (key == '9')
    {
        Graphics* graphics = GetSubsystem<Graphics>();
        Image screenshot(context_);
        graphics->TakeScreenShot(screenshot);
        //Here we save in the Data folder with date and time appended
        String fileName = GetSubsystem<FileSystem>()->GetProgramDir() + "Screenshots/Screenshot_" +
                Time::GetTimeStamp().Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_')+".png";
        //Log::Write(1, fileName);
        screenshot.SavePNG(fileName);
    }
}

void InputMaster::DeselectAll()
{
    for (int i = 0; i < selectedPlatforms_.Length(); i++)
    {
        selectedPlatforms_[i]->Deselect();
    }
    selectedPlatforms_.Clear();
}

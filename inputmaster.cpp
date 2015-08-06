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

#include "inputmaster.h"
#include "platform.h"
#include "oneirocam.h"

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
        firstHit_ = SharedPtr<Node>(masterControl_->world.cursor.hitResults[first].node_);
        //Void interaction (create new platform)
        if (firstHit_->GetNameHash() == N_VOID){
            new Platform(context_, masterControl_->world.cursor.sceneCursor->GetPosition(), masterControl_);
        }
        //Platform selection
        else if (firstHit_->GetNameHash() == N_TILEPART)
        {
            //Select single platform
            if (!(input_->GetKeyDown(KEY_LSHIFT)||input_->GetKeyDown(KEY_RSHIFT)))
            {
                SharedPtr<Platform> platform = masterControl_->platformMap_[firstHit_->GetParent()->GetParent()->GetID()];
                SetSelection(platform);
            }
            //Add platform to selection when either of the shift keys is held down
            else
            {
                SharedPtr<Platform> platform = masterControl_->platformMap_[firstHit_->GetParent()->GetParent()->GetID()];
                platform->SetSelected(!platform->IsSelected());
                selectedPlatforms_ += platform;
            }
        }
        //Building interaction, if platform was already selected
        //Slot interaction, if Former was selected
        else if (firstHit_->GetNameHash() == N_SLOT)
        {
            SharedPtr<Platform> platform = masterControl_->platformMap_[firstHit_->GetParent()->GetID()];
            IntVector2 coords = IntVector2(firstHit_->GetPosition().x_, -firstHit_->GetPosition().z_);
            if (platform->CheckEmpty(coords, true))
            {
                //Add tile
                platform->AddTile(coords);
                platform->FixFringe(coords);
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
        for (unsigned i = 0; i < selectedPlatforms_.Size(); i++){
            selectedPlatforms_[i]->SetMoveTarget(masterControl_->world.cursor.sceneCursor->GetPosition());
        }
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
    else if (key == KEY_9)
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
    else if (key == KEY_L)
    {
        SharedPtr<Platform> platform = SharedPtr<Platform>();
        if (firstHit_->GetNameHash() == N_TILEPART) platform = masterControl_->platformMap_[firstHit_->GetParent()->GetParent()->GetID()];
        else if (firstHit_->GetNameHash() == N_SLOT) platform = masterControl_->platformMap_[firstHit_->GetParent()->GetID()];
        if (platform) masterControl_->world.camera->Lock(platform);
    }
}

void InputMaster::DeselectAll()
{
    for (unsigned i = 0; i < selectedPlatforms_.Size(); i++)
    {
        selectedPlatforms_[i]->Deselect();
    }
    selectedPlatforms_.Clear();
}

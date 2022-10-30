#pragma once

#include <map>
#include <vector>

namespace PL
{
    class IGameObject;

    enum PLKeyEvent
    {
        KEY_PRESSED, KEY_RELEASED
    };

    enum PLKeyCode
    {
        KEY_W, KEY_A, KEY_S, KEY_D, KEY_ESC
    };

    class InputSubsystem
    {
    public:
        InputSubsystem();
        ~InputSubsystem();

        void ProcessInput();
        void RegisterGO(IGameObject* go);

    protected:
        

        std::vector<std::pair<PLKeyCode, PLKeyEvent>> firstQueuePending {};
        std::vector<std::pair<PLKeyCode, PLKeyEvent>> secondQueuePending {};

        std::vector<std::pair<PLKeyCode, PLKeyEvent>>* pendingEvents {};

        std::vector<std::pair<PLKeyCode, PLKeyEvent>>* swapVectors(){
            if(pendingEvents == &firstQueuePending){
                pendingEvents = &secondQueuePending;
                return &firstQueuePending;
            } 

            pendingEvents = &firstQueuePending;
            return &secondQueuePending;
        }

        std::map<
            std::pair<PLKeyCode, PLKeyEvent>, 
            std::vector<IGameObject*>
        > subscribers {};
    };
}
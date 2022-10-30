#include "../../include/io/InputSubsystem.h"
#include "../../include/game/IGameObject.h"

namespace PL
{
    InputSubsystem::InputSubsystem()
    {

    }

    InputSubsystem::~InputSubsystem()
    {
        this->subscribers.clear();
    }

    void InputSubsystem::RegisterGO(IGameObject* go)
    {
        auto vec = go->SubscribeToKeys();
        for(auto val : vec)
        {
            if(this->subscribers.find(val) == this->subscribers.end()){
                this->subscribers[val] = std::vector<IGameObject*>();
            }

            this->subscribers[val].push_back(go);
        }
    }

    void InputSubsystem::ProcessInput()
    {
        auto orig = this->swapVectors(); // one-command swap so that we wont interfere with async

        auto vec = *orig; // copy pending events
        orig->clear(); // clear orig

        // process inputs
        for(auto value : vec)
        {
            auto start = this->subscribers.begin();
            auto end = this->subscribers.end();

            if(this->subscribers.find(value) != end){
                
                auto vec_subs = this->subscribers[value];
                for(auto sub : vec_subs)
                {
                    sub->OnKeyEvent(value.first, value.second);
                }
            }
        }
    }
}
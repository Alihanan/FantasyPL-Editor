#pragma once

#include "GOTransform.h"
#include "../vulkan/vModel.h"
#include "../general/IUncopiable.h"
#include "InputSubsystem.h"

#include <vector>

namespace PL
{
    class IGameObject : public IUncopiable
    {
    public:
        using id_t = uint32_t;

        virtual ~IGameObject() = 0;

        virtual void OnKeyEvent(PLKeyCode key, PLKeyEvent event)  {}
        virtual std::vector<std::pair<PLKeyCode, PLKeyEvent>> SubscribeToKeys() {
            return {};
        }

        virtual void OnUpdate() = 0;
        virtual void OnStart() = 0;

        const id_t GetGOID() { return this->id; }

        // move/assignment operators are enabled
        IGameObject(IGameObject &&) = default;
        IGameObject &operator=(IGameObject &&) = default;

        const std::vector<vModel*> GetModel() {return this->renderModel;}
        glm::mat4 GetModelMatrix() {return this->transform.M();}

    protected:
        GOTransform transform {};
        std::vector<vModel*> renderModel;
        id_t id;

        IGameObject() : id(GLOBAL_CURRENT_ID++)
        {}

    private:
        inline static uint32_t GLOBAL_CURRENT_ID = 0;
    };
}
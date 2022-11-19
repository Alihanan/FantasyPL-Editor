#pragma once


#include "../general/IUncopiable.h"
#include "../io/InputSubsystem.h"
#include "ModelDefines.h"
#include "GOTransform.h"
#include <vector>

namespace PL
{
    class IGameObject : public IUncopiable
    {
    public:
        using id_t = uint32_t;

        virtual ~IGameObject() = 0;
        const id_t GetGOID() { return this->id; }

        virtual void OnKeyEvent(PLKeyCode key, PLKeyEvent event)  {}
        virtual std::vector<std::pair<PLKeyCode, PLKeyEvent>> SubscribeToKeys() {
            return {};
        }

        virtual void OnUpdate() = 0;
        virtual void OnStart() = 0;

        // move/assignment operators are enabled
        IGameObject(IGameObject &&) = default;
        IGameObject &operator=(IGameObject &&) = default;
        std::vector<IGameObject*> GetChildrenGO() { return this->children; }

        virtual void GetModel(std::vector<GameModel*>* total_render_vector) final{
            for(GameModel* gm : this->renderModels)
            {
                total_render_vector->push_back(gm);
            }
            
            for(IGameObject* children : this->children)
            {
                children->GetModel(total_render_vector);
            }
        }

        bool ShouldRender() { return true; }        
        GOTransform& transf() { return transform;}

    protected:
        GOTransform transform {};
        std::vector<GameModel*> renderModels;
        id_t id;

        IGameObject(IGameObject* parent) : id(GLOBAL_CURRENT_ID++), parent(parent)
        {}

        IGameObject* parent = nullptr;
        std::vector<IGameObject*> children {};

    private:
        inline static uint32_t GLOBAL_CURRENT_ID = 0;
    };
}
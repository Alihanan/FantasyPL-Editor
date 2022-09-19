#pragma once

#include "../general/IUncopiable.h"
#include "../general/IDependent.h"

#include "vMemoryManager.h"
#include <optional>


namespace PL
{
    class vModel : public IUncopiable, public IDependent
    {
    public:
        // IDependent
        const static std::string _DEP_ID;
        inline const static std::vector<std::string> _DEP_NEEDED_DEPS = {
            vMemoryManager::_DEP_ID
        };
        std::vector<std::string> GetNeededDependencies()
        {
            return this->_DEP_NEEDED_DEPS;
        }
        void ReceiveContext(std::vector<std::vector<IDependent*>> context)
        {          
            this->memoryManager = static_cast<vMemoryManager*>(context[0][0]);
            this->Initialize();
        }

        void UpdateContext(std::vector<std::vector<IDependent*>> context)
        {
            auto ret = std::string(typeid(this).name());
        }
        
        bool IsSingleton()
        {
            return true;
        }

        std::string GetDependencyID()
        {
            return this->_DEP_ID;
        }
        
        // Others
        vModel(std::string file) :
            data(this->processFile(file))
        {}
        
        ~vModel();
        void bind();
        void draw();

    protected:
        std::string shaderName;
        vMemoryManager* memoryManager;
        void *data;

        void Initialize(); 
    private:
        vModel() {}
        void* processFile(std::string fileName);


        void* generateGrid(uint32_t WIDTH, uint32_t HEIGH);
    };


}
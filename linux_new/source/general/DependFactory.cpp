#include "../../include/general/DependFactory.h"

#include "../../include/vulkan/vInstance.h"
#include "../../include/vulkan/vWindow.h"
#include "../../include/vulkan/vApplication.h"
#include "../../include/vulkan/vPhysicalDeviceHandler.h"
#include "../../include/vulkan/vMemoryManager.h"
#include "../../include/vulkan/vDevice.h"
#include "../../include/vulkan/vRenderer.h"
#include "../../include/vulkan/vShaderManager.h"
#include "../../include/vulkan/vPipeConfig.h"
#include "../../include/vulkan/vModelManager.h"
#include "../../include/vulkan/vSwapchain.h"
#include "../../include/math/algorithms.h"


namespace PL
{
    DependFactory* DependFactory::I()
    {
        if(DependFactory::instance == nullptr)
        {
            DependFactory::instance = new DependFactory();
        }
        return DependFactory::instance;
    }

    DependFactory::DependFactory() 
    : instanceTracker()
    {}
    
    IDependent* DependFactory::createAnythingFromID(std::string uid)
    {
        IDependent* toAdd;

        if(uid == vInstance::_DEP_ID)
        {   
            toAdd = new vInstance();   
        }
        else if(uid == vWindow::_DEP_ID)
        {
            toAdd = new vWindow();
        }
        else if(uid == vApplication::_DEP_ID)
        {
            toAdd = new vApplication();
        }
        else if(uid == vPhysicalDeviceHandler::_DEP_ID)
        {
            toAdd = new vPhysicalDeviceHandler();
        }
        else if(uid == vDevice::_DEP_ID)
        {
            toAdd = new vDevice();
        }
        else if(uid == vRenderer::_DEP_ID)
        {
            toAdd = new vRenderer();
        }
        else if(uid == vRenderPass::_DEP_ID)
        {
            toAdd = new vRenderPass();
        }
        else if(uid == vModelManager::_DEP_ID)
        {
            toAdd = new vModelManager();
        }
        else if(uid == vPipeConfig::_DEP_ID)
        {
            toAdd = new vPipeConfig();
        }
        else if(uid == vMemoryManager::_DEP_ID)
        {
            toAdd = new vMemoryManager();
        }
        else if(uid == vSwapchain::_DEP_ID)
        {
            toAdd = new vSwapchain();
        }
        else if(uid == vShaderManager::_DEP_ID)
        {
            toAdd = new vShaderManager();
        }
        else{
            throw std::runtime_error("Trying to create non-existing dependent object");
        }
        return toAdd;
    }


    std::vector<IDependent*> DependFactory::createOrGetInstance(std::string uid)
    {
        std::vector<IDependent*> ret;    

        if(this->instanceTracker.find(uid) != this->instanceTracker.end())
        {
            ret = this->instanceTracker[uid];
            return ret;
        }

        auto toAdd = this->createAnythingFromID(uid);
        this->sendContext(toAdd);

        ret.push_back(toAdd);
        this->instanceTracker[uid] = ret;
        return ret;
    }

    std::vector<std::vector<IDependent*>> DependFactory::calculateContext(IDependent* toGet)
    {
        std::vector<std::string> context = toGet->GetNeededDependencies();
        std::vector<std::vector<IDependent*>> ret;
        for(std::string str : context)
        {
            ret.push_back(this->createOrGetInstance(str));
        }
        return ret;
    }
    
    void DependFactory::updateContextAfterCreate(std::string uid)
    {
        for(const auto& [key, value] : this->instanceTracker)
        {
            if(key == uid) continue;

            auto deps = value[0]->GetNeededDependencies();
            bool contains = vectorConstains(deps, uid);
            if(!contains)
            {
                continue;
            }

            for(auto inst : value)
            {
                this->sendContext(inst);
            }
        }
    }

    void DependFactory::sendContext(IDependent* toSend)
    {
        auto context = this->calculateContext(toSend);
        toSend->ReceiveContext(context);
    }

    void DependFactory::deleteInstance(IDependent* dep)
    {
        // TODO

        if(dep == nullptr)
        {
            return;
        }
        if(this->instanceTracker.find(dep->GetDependencyID()) == this->instanceTracker.end())
        {
            return;
        }

        /*
        auto curr = this->instanceTracker[dep->GetDependencyID()];

        for(auto inst : curr)
        {
            
        }*/

        delete dep;
    }

    DependFactory::~DependFactory()
    {}
}
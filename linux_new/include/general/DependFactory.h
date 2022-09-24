#pragma once

#include "IDependent.h"

#include <map>
#include <vector>

namespace PL
{
    class DependFactory
    {
    public:
        static DependFactory* I();

        std::vector<IDependent*> createOrGetInstance(std::string uid);
        
        void deleteInstance(IDependent* dep);

        ~DependFactory();

    protected:
        inline static DependFactory* instance = nullptr;
        DependFactory();

        std::map<std::string, std::vector<IDependent*>> instanceTracker;

        std::vector<std::vector<IDependent*>> calculateContext(IDependent* toGet);
        IDependent* createAnythingFromID(std::string uid);
        void updateContextAfterCreate(std::string uid);
        void sendContext(IDependent* toSend);
    };


}
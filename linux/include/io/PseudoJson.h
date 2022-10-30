#pragma once

#include <fstream>
#include <map>
#include <iostream>
#include <sstream>
#include "../external/json.hpp"

namespace PL
{
    typedef json::JSON PJSON;

    class PseudoJson
    {
    public:        
        inline static PJSON readFile(std::string file)
        {
            std::ifstream t(file);
            std::stringstream buffer;
            buffer << t.rdbuf();
            std::string str = buffer.str();

            PJSON tst = json::JSON::Load(str);

            return tst;
        }
    };
}
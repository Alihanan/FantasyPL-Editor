#include "../../include/math/algorithms.h"

namespace PL
{
    std::vector<std::string> split_string(std::string source, std::string splitters)
    {
        std::string delims = splitters;
        if(splitters.size() == 0)
            delims = " ";

        std::stringstream stringStream(source);
        std::string line;

        std::vector<std::string> wordVector;


        char first_delim = delims[0];
        delims = delims.erase(0);

        while(std::getline(stringStream, line, first_delim)) 
        {
            std::size_t prev = 0, pos;
            while ((pos = line.find_first_of(delims, prev)) != std::string::npos)
            {
                if (pos > prev)
                    wordVector.push_back(line.substr(prev, pos-prev));
                prev = pos+1;
            }
            if (prev < line.length())
                wordVector.push_back(line.substr(prev, std::string::npos));
        }

        return wordVector;
    }
}
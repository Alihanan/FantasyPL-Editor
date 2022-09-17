#pragma once

namespace PL
{
    class va_uncopiable
    {
    public:
        va_uncopiable() {};

        va_uncopiable(const va_uncopiable &) = delete;
        va_uncopiable &operator=(const va_uncopiable &) = delete;
        va_uncopiable(va_uncopiable &&) = delete;
        va_uncopiable &operator=(va_uncopiable &&) = delete;

        virtual ~va_uncopiable() = 0;
    };
    

} 

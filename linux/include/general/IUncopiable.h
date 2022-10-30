#pragma once



namespace PL
{
    class IUncopiable
    {
    public:
        IUncopiable() {}

        IUncopiable(const IUncopiable &) = delete;
        IUncopiable &operator=(const IUncopiable &) = delete;
        IUncopiable(IUncopiable &&) = delete;
        IUncopiable &operator=(IUncopiable &&) = delete;

        virtual ~IUncopiable() = 0;
    };
}
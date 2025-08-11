#pragma once
#include <cstdint>

#include "Span.hpp"

class ICrc  {
public:
    explicit ICrc(const char *name): name(name){}
    const char *name{};
    virtual void Reset() = 0;
    virtual uint32_t Calc(Span<uint8_t>) = 0;
    virtual uint32_t Append(uint32_t, const Span<uint8_t>) = 0;

};

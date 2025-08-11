#pragma once

#include <cstdint>
#include <cstddef>
#include <utility>
#include <functional>
#include <chrono>

#include "Span.hpp"

namespace proto::interface{
    using receiveDelegate = std::function<void(Span<uint8_t> buffer, size_t &read)>;
    using namespace std::chrono_literals;

    class IInterface {
    public:
        explicit IInterface(const char *name){};
        virtual bool Write(Span<uint8_t> buffer, std::chrono::milliseconds timeout = 1s) = 0;

        virtual bool IsOpen() = 0;
        virtual bool Open() = 0;
        virtual bool Close() = 0;
        virtual bool AddReceiveCallback( receiveDelegate) = 0;
//        std::function<void()> on_ready_;

    private:
        virtual int Read(uint8_t *buffer, size_t count) = 0;

    };
}

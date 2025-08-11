#pragma once

#include <vector>
#include <iostream>
#include <mutex>
#include <thread>

#include "Span.hpp"
#include "Interface.hpp"

namespace proto::interface {
    class UartLinuxInterface : public IInterface {
    public:
        explicit UartLinuxInterface() : IInterface("uart linux interface"), is_open_(false) {}

        bool Write(Span<uint8_t> buffer, std::chrono::milliseconds timeout) override;

        bool IsOpen() override;

        bool Open() override;

        int OpenUart(const char* device, int baudrate);

        bool Close() override;

        bool AddReceiveCallback(receiveDelegate callback) override ;

    private:
        int fd_;
        std::mutex write_mtx;
        std::mutex read_mtx;
        bool is_open_;
        std::vector<receiveDelegate> callbacks_;
        uint8_t receive_buffer_[1000];

        std::thread receive_thread_;
        void UartReaderThread();
        int Read(uint8_t*, size_t) override;
    };
}
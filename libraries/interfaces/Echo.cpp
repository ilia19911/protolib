#include "Echo.hpp"

namespace proto::interface{

    bool echoInterface::Write(Span<uint8_t> buffer, std::chrono::milliseconds timeout) {
        if (!is_open_) {
            std::cerr << "Interface is not Open!" << std::endl;
            return false;
        }
        std::lock_guard<std::mutex> lock(write_mtx);
        size_t read = 0;
        if (not callbacks_.empty()) {
            for (auto &callback: callbacks_) {
                callback(buffer.subspan(read), read);
            }
        }
        return true;
    }


    bool echoInterface::IsOpen() {
        return is_open_;
    }

    bool echoInterface::Open()  {
        is_open_ = true;
        return true;
    }

    bool echoInterface::Close() {
        is_open_ = false;
        return true;
    }

    bool echoInterface::AddReceiveCallback(receiveDelegate callback) {
        callbacks_.push_back(callback);
        return true;
    }

    int echoInterface::Read(uint8_t *buffer, size_t count) {
        return 0;
    }
}
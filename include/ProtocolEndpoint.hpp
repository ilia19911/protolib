/**
 * @file ProtocolEndpoint.hpp
 * @brief Base endpoint class for protocol implementations.
 *
 * This header defines a generic ProtocolEndpoint template for managing protocol
 * communication endpoints. It provides containers for RX and TX fields, a snapshot
 * mechanism for the last received frame, and thread-safe access to recent data.
 *
 * Usage:
 *   - Specialize with protocol field sets and CRC type.
 *   - Wire to external interfaces with SetInterfaces().
 *   - Use snapshot/query methods to access last received data.
 *
 * Thread-safety: All snapshot and wait methods are internally synchronized.
 *
 * Example:
 *   ProtocolEndpoint<MyRxFields, MyTxFields, MyCrc> ep;
 *   ep.SetInterfaces(rx_if, tx_if);
 *   // Send data, wait for responses, access last frame, etc.
 */

#pragma once

#include <cstdint>
#include <tuple>
#include <optional>
#include <type_traits>
#include <cstring>
#include <chrono>
#include <ctime>
#include <memory>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <variant>

#include "prototypes/container/RxContainer.hpp"
#include "prototypes/container/TxContainer.hpp"

namespace proto{

/**
 * @brief Protocol endpoint base for protocol implementations.
 *
 * @tparam RxFields Protocol RX field set.
 * @tparam TxFields Protocol TX field set.
 * @tparam Crc      CRC type for integrity.
 *
 * Responsibilities:
 *   - Handles RX/TX containers.
 *   - Snapshots the last received frame's type code (payload can be extracted on-demand via wait_once).
 *   - Provides thread-safe access and waiting.
 *   - Allows user RX callback hooks.
 *
 * Lifecycle:
 *   - Construct, wire interfaces, use send/wait methods, query snapshot.
 */
    template<class RxFields, class TxFields, class Crc = CrcSoft>
    class ProtocolEndpoint {
    public:
        /** @brief RX container type for inbound protocol frames. */
        using RxCont = proto::RxContainer<RxFields, Crc>;
        /** @brief TX container type for outbound protocol frames. */
        using TxCont = proto::TxContainer<TxFields, Crc>;
        using ReceiveType = typename RxCont::ReturnType;

        /**
         * @brief Construct endpoint and install a permanent RX callback that snapshots the last frame's type code.
         * @note A permanent RX callback is installed that snapshots the last received type code.
         */
        explicit ProtocolEndpoint(bool debug=false) {
            SetDebug(debug);
            // Permanent RX-callback
            // Permanent RX-callback that snapshots only the last type code.
            // NOTE: We intentionally do NOT materialize a variant for DATA_FIELD here,
            // to avoid duplicate-type variant instantiations (e.g., multiple uint8_t* PacketInfo).
            rx_delegate_ = rx.AddReceiveCallback(rx_callback);
        }

        void SetDebug(bool v){ rx.SetDebug(v); tx.SetDebug(v); }

        /**
         * @brief Wire the endpoint to external RX and TX interfaces.
         *
         * Sets up RX and TX containers to use the provided interfaces.
         * The RX callback feeds incoming byte chunks to rx_.Fill.
         *
         * @param rx_if Interface for receiving data.
         * @param tx_if Interface for transmitting data.
         */
        void SetInterfaces(proto::interface::IInterface& rx_if,
                           proto::interface::IInterface& tx_if) {
            tx.SetInterface(tx_if);
            rx_if_cb_ = rx_if.AddReceiveCallback([this](Span<uint8_t> s, size_t& r){
                rx.Fill(s, r);
            });
        }
        static constexpr std::chrono::duration receive_timeout_ = std::chrono::milliseconds{1000};
        template<typename... Infos>
        ReceiveType Request( Infos&&... infos) {
            received_ = false;

            ReceiveType result{};
            auto l = [&](auto& container){
                auto& data_field = rx.template Get<proto::FieldName::DATA_FIELD>();
                result = data_field.GetData();
            };
            inflight_cb_ = l;
            tx.SendPacket(std::forward<Infos>(infos)...);

            std::unique_lock<std::mutex> lock(m_);
            cv_.wait_for(lock, receive_timeout_, [this](){return received_;});
            return result;
        }

        RxCont rx;
        TxCont tx;

    protected:


        // sync
        std::mutex m_;
        std::condition_variable cv_;
        uint64_t seq_ = 0;
        bool received_{false};

        // One-shot extractor installed by wait_once to pull typed DATA_FIELD while Rx buffer is valid.
        std::function<void(RxCont&)> inflight_cb_{};


        // Stored delegates to honor [[nodiscard]] on AddReceiveCallback
        typename RxCont::Delegate rx_delegate_{};
        typename RxCont::CallbackType rx_callback{[this](auto& container){
            if(container.IsDebug()){
                std::cout << " \n\n Packet from board is received!! " <<std::endl;
                container.for_each_type([&](auto& field){
                    field.Print();
                });
            }
            received_ = true;
            cv_.notify_all();

            if(inflight_cb_){
                inflight_cb_(container);
            }
            inflight_cb_ = nullptr;
        }};
        proto::interface::Delegate rx_if_cb_{};
    };

}
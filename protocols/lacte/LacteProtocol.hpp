#pragma once
#include <cstdint>
#include <tuple>
#include <optional>
#include <type_traits>
#include <cstring>
#include <chrono>
#include <ctime>
#include <memory>

#include "prototypes/container/RxContainer.hpp"
#include "prototypes/container/TxContainer.hpp"
#include "ymodem.hpp"
#include "Interface.hpp"
#include "fs_posix.hpp"
#include "LacteObjects.hpp"

#include "libraries/crc/crc16Modbus/Crc16Modbus.hpp"

using namespace std::chrono_literals;
namespace Lacte::Proto{


//    template<uint8_t *RX_BASE, uint8_t *TX_BASE>
//    class HostLacteProtocol{
//    public:
//        proto::TxContainer<typename hostPacket<TX_BASE>::host_packet_fields, Crc16Modbus> TxContainer;
//        proto::RxContainer<typename boardPacket<RX_BASE>::board_packet_fields, Crc16Modbus> RxContainer;
//
//
//
//        // compact Spec holder and aggregated spec lists
//        template<auto Code_, class Rx_>
//        struct Spec { static constexpr auto Code = Code_; using Rx = Rx_; };
//
//        using AllCommandSpecs = std::tuple<
//            Spec<CommandSpec<INFO>::Code,      typename CommandSpec<INFO>::Rx>,
//            Spec<CommandSpec<VERSION>::Code,   typename CommandSpec<VERSION>::Rx>,
//            Spec<CommandSpec<UID>::Code,       typename CommandSpec<UID>::Rx>,
//            Spec<CommandSpec<RFID_ID>::Code,   typename CommandSpec<RFID_ID>::Rx>,
//            Spec<CommandSpec<RFID_DATA>::Code, typename CommandSpec<RFID_DATA>::Rx>,
//            Spec<CommandSpec<RESTART>::Code,   typename CommandSpec<RESTART>::Rx>
//        >;
//
//        using AllParamSpecs = std::tuple<
//            Spec<ParamSpec<Params::MAGIC_WORD>::Code,       typename ParamSpec<Params::MAGIC_WORD>::Rx>,
//            Spec<ParamSpec<Params::LACTE_SN>::Code,         typename ParamSpec<Params::LACTE_SN>::Rx>,
//            Spec<ParamSpec<Params::PROD_DATE>::Code,        typename ParamSpec<Params::PROD_DATE>::Rx>,
//            Spec<ParamSpec<Params::MCU_UID>::Code,          typename ParamSpec<Params::MCU_UID>::Rx>,
//            Spec<ParamSpec<Params::MACHINE_SN>::Code,       typename ParamSpec<Params::MACHINE_SN>::Rx>,
//            Spec<ParamSpec<Params::ACTIVATION_TIME>::Code,  typename ParamSpec<Params::ACTIVATION_TIME>::Rx>,
//            Spec<ParamSpec<Params::DRINK_COUNTER>::Code,    typename ParamSpec<Params::DRINK_COUNTER>::Rx>,
//            Spec<ParamSpec<Params::TIME_COUNTER>::Code,     typename ParamSpec<Params::TIME_COUNTER>::Rx>
//        >;
//
//        // === compile-time mapping: Rx type -> numeric code from a tuple of Spec (C++17) ===
//        template<class TupleSpecs, class RxT, std::size_t I = 0>
//        static constexpr auto code_of_rx() {
//            if constexpr (I >= std::tuple_size<TupleSpecs>::value) {
//                // Not found: return default-initialized code of the first Spec's Code type
//                using CodeT = decltype(std::tuple_element_t<0, TupleSpecs>::Code);
//                return CodeT{};
//            } else {
//                using Spec = std::tuple_element_t<I, TupleSpecs>;
//                if constexpr (std::is_same<typename Spec::Rx, RxT>::value) {
//                    return Spec::Code;
//                } else {
//                    return code_of_rx<TupleSpecs, RxT, I + 1>();
//                }
//            }
//        }
//
//        // === user-friendly requests by RESPONSE TYPE ===
//        template<class RxT>
//        std::optional<RxT> RequestByType() {
//            constexpr auto code = code_of_rx<AllCommandSpecs, RxT>();
//            return send_opt<static_cast<Lacte::Proto::packetNumbers>(code), RxT>(proto::EmptyDataType{}, 0);
//        }
//
//        template<class RxT>
//        std::optional<RxT> GetParamByType() {
//            constexpr auto pcode = code_of_rx<AllParamSpecs, RxT>();
//            const uint8_t code8 = static_cast<uint8_t>(pcode);
//            return send_opt<Lacte::Proto::GET_PARAMS, RxT>(code8, 1);
//        }
//
//        // Универсальная обёртка над вашим Send -> std::optional<Rx>
//        template<Lacte::Proto::packetNumbers NAME, typename Rx, typename Tx>
//        std::optional<Rx> send_opt(const Tx& data, size_t size = sizeof(Tx)) {
//            if (auto* p = Send<NAME, Rx>(data, size)) return *p;
//            return std::nullopt;
//        }
//
//// Обычные запросы: типы берём из CommandSpec<N>
//        template<Lacte::Proto::packetNumbers N>
//        std::optional<typename CommandSpec<N>::Rx>
//        Request() {
//            using Tx = typename CommandSpec<N>::Tx;
//            using Rx = typename CommandSpec<N>::Rx;
//            return send_opt<N, Rx>(Tx{}, std::is_same_v<Tx, proto::EmptyDataType> ? 0 : sizeof(Tx));
//        }
//
//// Перегрузка, если вдруг команда N требует непустой Tx;
//        template<Lacte::Proto::packetNumbers N>
//        std::optional<typename CommandSpec<N>::Rx>
//        Request(const typename CommandSpec<N>::Tx& tx) {
//            using Tx = typename CommandSpec<N>::Tx;
//            using Rx = typename CommandSpec<N>::Rx;
//            return send_opt<N, Rx>(tx);
//        }
//
//// Запрос параметра (compile-time): номер параметра в шаблоне → правильный Rx автоматически.
//        template<Lacte::Proto::Params::numbers P>
//        std::optional<typename ParamSpec<P>::Rx>
//        GetParam() {
//            const uint8_t code = static_cast<uint8_t>(P);
//            using Rx = typename ParamSpec<P>::Rx;
//            return send_opt<Lacte::Proto::GET_PARAMS, Rx>(code, 1);
//        }
//
//        typename decltype(RxContainer)::CallbackType rx_delegate{[this](auto& container){
//            if(container.IsDebug()){
//                std::cout << " \n\n Packet from board is received!! " <<std::endl;
//                container.for_each_type([&](auto& field){
//                    field.Print();
//                });
//            }
//            received_ = true;
//            received_cv_.notify_all();
//
//            if(user_callback){
//                user_callback(container);
//            }
//        }};
//        typename decltype(RxContainer)::Delegate rx_delegate_ptr;
//
//
//        explicit HostLacteProtocol(bool debug){
//            SetDebug(debug);
//            rx_delegate_ptr = RxContainer.AddReceiveCallback(rx_delegate);
//        }
//
//        void SetDebug(bool debug){
//            RxContainer.SetDebug(debug);
//            TxContainer.SetDebug(debug);
//        }
//
//        template <packetNumbers NAME, typename RECEIVE, typename TRANSMIT>
//        RECEIVE* Send(TRANSMIT data, size_t size = sizeof(TRANSMIT)){
//            packetNumbers name = NAME;
//            using namespace std::chrono;
//            auto now = system_clock::now();
//            std::time_t unix_time = system_clock::to_time_t(now);
//            received_ = false;
//
//            RECEIVE* result = nullptr;
//            auto l = [&](auto& container){
//                auto& data_field = RxContainer.template Get<proto::FieldName::DATA_FIELD>();
//                if(auto* received = data_field.template GetAs<NAME, RECEIVE>()){
//                    result = received;
//                }
//                return nullptr;
//            };
//            user_callback = l;
//            TxContainer.SendPacket(proto::MakeFieldInfo<proto::FieldName::TYPE_FIELD>(&name),
//                                   proto::MakeFieldInfo<proto::FieldName::DATA_FIELD>(&data, size),
//                                   proto::MakeFieldInfo<proto::FieldName::TIME_FIELD>(&unix_time));
//
//            std::unique_lock<std::mutex> lock(receive_mtx_);
//            received_cv_.wait_for(lock, receive_timeout_, [this](){return received_;});
//            return result;
//        }
//        bool Flash(const char* path){
//            YmodemPrerelease ymodem(*txInerface);
//            return ymodem.send(path);
//        }
//
//        void SetInterfaces(proto::interface::IInterface& rx_interface, proto::interface::IInterface& tx_interface){
//            txInerface = &tx_interface;
//            TxContainer.SetInterface(tx_interface);
//            tx_interface_callback = rx_interface.AddReceiveCallback([this](Span<uint8_t> span, size_t &read){
//                RxContainer.Fill(span, read);
//            });
//        }
//
//    private:
//
//        proto::interface::Delegate tx_interface_callback;
//
//        proto::interface::IInterface* txInerface{};
//        std::function<void(proto::RxContainer<typename boardPacket<RX_BASE>::board_packet_fields, Crc16Modbus>&)> user_callback;
//
//        std::mutex receive_mtx_;
//        bool received_{false};
//        std::condition_variable received_cv_;
//        static constexpr std::chrono::duration receive_timeout_ = std::chrono::milliseconds{1000};
//    };

    template<uint8_t *RX_BASE, uint8_t *TX_BASE>
    class BoardLacteProtocol{
    public:
        proto::TxContainer<typename boardPacket<TX_BASE>::board_packet_fields, Crc16Modbus> TxContainer;
        proto::RxContainer<typename hostPacket<RX_BASE>::host_packet_fields, Crc16Modbus> RxContainer;

        BoardLacteProtocol(bool debug){
            RxContainer.SetDebug(debug);
            TxContainer.SetDebug(debug);
        }
        template<typename T = proto::EmptyDataType>
        void Answer(packetNumbers name, T data = proto::EmptyDataType{}, size_t size = sizeof(T)){
            TxContainer.SendPacket(proto::MakeFieldInfo<proto::FieldName::TYPE_FIELD>(&name), proto::MakeFieldInfo<proto::FieldName::DATA_FIELD>(&data, size));
        }
    };
}

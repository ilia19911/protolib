#pragma once

#include <cstdint>
#include "LacteProtocol.hpp"
#include <libraries/interfaces/Echo.hpp>

namespace Lacte::Proto {


    template<uint8_t *RX_BASE, uint8_t *TX_BASE>
    class VirtualBoard{
    public:
        static constexpr uint8_t default_magic_word[2] = {0xbe, 0xef};
        static constexpr uint8_t default_lacte_sn[4] = {0x22, 0x34, 0x53, 0x34};
        static constexpr uint8_t default_version[2] = {1, 0};
        static constexpr uint8_t default_prod_date[4] = {0x20, 0x23, 0x10, 0x01};
        static constexpr uint8_t default_mcu_uid[12] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B};
        static constexpr uint8_t default_uid_data[12] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B};
        static constexpr uint8_t default_machine_sn[12] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC};
        static constexpr uint8_t default_activation_time[4] = {0x01, 0x05, 0x01, 0x02};
        static constexpr uint8_t default_drink_counter[4] = {0x06, 0x05, 0x04, 0x30};
        static constexpr uint8_t default_time_counter[4] = {0x44, 0x33, 0x22, 0x11};
        static constexpr uint8_t default_reserve[4] = {0x00, 0x00, 0x00, 0x00};
        static constexpr uint8_t default_rfid_id[7] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};

        Lacte::Proto::VersionPacketType version_data{1,0};
        Lacte::Proto::InfoPacketType info_data{BoardStatus::IDLE, 0xffff, {}};
        Lacte::Proto::UIDPacketType uid_data{1,0};
        Lacte::Proto::RFIDPacketType rfid{};
        Lacte::Proto::RFIDDataPacketType rfid_data{};
        Params::MagicWord magic_word{};
        Params::LacteSn lacte_sn{};
        Params::ProdDate prod_date{};
        Params::Reserve reserve{};
        Params::McuUid mcu_uid{};
        Params::MachineSn machine_sn{};
        Params::ActivationTime activation_time{};
        Params::DrinkCounter drink_counter{};
        Params::TimeCounter time_counter{};

        // Setter methods for board state fields
        void set_version(const Lacte::Proto::VersionPacketType& v) { version_data = v; }
        void set_info(const Lacte::Proto::InfoPacketType& v) { info_data = v; }
        void set_uid(const Lacte::Proto::UIDPacketType& v) { uid_data = v; }
        void set_rfid(const Lacte::Proto::RFIDPacketType& v) { rfid = v; }
        void set_rfid_data(const Lacte::Proto::RFIDDataPacketType& v) { rfid_data = v; }
        void set_magic_word(const Params::MagicWord& v) { magic_word = v; }
        void set_lacte_sn(const Params::LacteSn& v) { lacte_sn = v; }
        void set_prod_date(const Params::ProdDate& v) { prod_date = v; }
        void set_reserve(const Params::Reserve& v) { reserve = v; }
        void set_mcu_uid(const Params::McuUid& v) { mcu_uid = v; }
        void set_machine_sn(const Params::MachineSn& v) { machine_sn = v; }
        void set_activation_time(const Params::ActivationTime& v) { activation_time = v; }
        void set_drink_counter(const Params::DrinkCounter& v) { drink_counter = v; }
        void set_time_counter(const Params::TimeCounter& v) { time_counter = v; }

        BoardLacteProtocol<RX_BASE, TX_BASE> board_proto{false};
        proto::interface::echoInterface from_host_interface{};
        proto::interface::echoInterface from_board_interface{};
        proto::interface::Delegate host_interface_send_delegate;

        proto::interface::Delegate board_interface_send_delegate;
        typename decltype(board_proto.RxContainer)::Delegate board_receive_delegate;

        void SetDebug(bool debug) {
            board_proto.RxContainer.SetDebug(debug);
            board_proto.TxContainer.SetDebug(debug);
        }

        template<class HOST_CONTAINER>
        void SetHost(HOST_CONTAINER& host_container){
            board_interface_send_delegate = from_board_interface.AddReceiveCallback([&](Span<uint8_t> span, size_t &read){
                host_container.RxContainer.Fill(span, read);
            });
            host_container.SetInterfaces(from_board_interface, from_host_interface);
        }

        explicit VirtualBoard() {
            from_host_interface.Open();
            from_board_interface.Open();

            board_proto.TxContainer.SetInterface(from_board_interface);

            host_interface_send_delegate = from_host_interface.AddReceiveCallback( [this](Span<uint8_t> span, size_t &read){
                uint8_t data[span.size()];
                memcpy(data, span.data(), span.size());
                board_proto.RxContainer.Fill(span, read);
            });
            board_receive_delegate = board_proto.RxContainer.AddReceiveCallback([&](auto &container){
                auto& field = container.template Get<proto::FieldName::TYPE_FIELD>();
                auto& field_data = container.template Get<proto::FieldName::DATA_FIELD>();
                if(*field.GetPtr() == packetNumbers::VERSION){
                    board_proto.Answer(packetNumbers::VERSION, version_data);
                }
                else if(*field.GetPtr() == packetNumbers::INFO){
                    board_proto.Answer(packetNumbers::INFO, info_data);
                }
                else if(*field.GetPtr() == packetNumbers::UID){
                    board_proto.Answer(packetNumbers::UID, uid_data);
                }
                else if(*field.GetPtr() == packetNumbers::RFID_ID){
                    board_proto.Answer(packetNumbers::RFID_ID, rfid);
                }
                else if(*field.GetPtr() == packetNumbers::RFID_DATA){
                    board_proto.Answer(packetNumbers::RFID_DATA, rfid_data);
                }
                else if(*field.GetPtr() == packetNumbers::RESTART){
                    board_proto.Answer(packetNumbers::RESTART);
                }
                else if(*field.GetPtr() == packetNumbers::GET_PARAMS){
                    auto param = *field_data.GetPtr();
                    if(param == Params::numbers::MAGIC_WORD){
                        board_proto.Answer(packetNumbers::GET_PARAMS, magic_word, sizeof(magic_word));
                    }
                    else if(param == Params::numbers::LACTE_SN){
                        board_proto.Answer(packetNumbers::GET_PARAMS, lacte_sn, sizeof(lacte_sn));
                    }
                    else if(param == Params::numbers::PROD_DATE){
                        board_proto.Answer(packetNumbers::GET_PARAMS, prod_date, sizeof(prod_date));
                    }
                    else if(param == Params::numbers::RESERVE){
                        board_proto.Answer(packetNumbers::GET_PARAMS, reserve, sizeof(reserve));
                    }
                    else if(param == Params::numbers::MCU_UID){
                        board_proto.Answer(packetNumbers::GET_PARAMS, mcu_uid, sizeof(mcu_uid));
                    }
                    else if(param == Params::numbers::MACHINE_SN){
                        board_proto.Answer(packetNumbers::GET_PARAMS, machine_sn, sizeof(machine_sn));
                    }
                    else if(param == Params::numbers::ACTIVATION_TIME){
                        board_proto.Answer(packetNumbers::GET_PARAMS, activation_time, sizeof(activation_time));
                    }
                    else if(param == Params::numbers::DRINK_COUNTER){
                        board_proto.Answer(packetNumbers::GET_PARAMS, drink_counter, sizeof(drink_counter));
                    }
                    else if(param == Params::numbers::TIME_COUNTER){
                        board_proto.Answer(packetNumbers::GET_PARAMS, time_counter, sizeof(time_counter));
                    }
                }
            });

            memcpy(magic_word.data, default_magic_word, sizeof(default_magic_word));
            memcpy(lacte_sn.data, default_lacte_sn, sizeof(default_lacte_sn));
            memcpy((uint8_t*)&version_data, default_version, sizeof(default_version));
            memcpy(prod_date.data, default_prod_date, sizeof(default_prod_date));
            memcpy(mcu_uid.data, default_mcu_uid, sizeof(default_mcu_uid));
            memcpy((uint8_t*)&uid_data, default_uid_data, sizeof(default_uid_data));
            memcpy(machine_sn.data, default_machine_sn, sizeof(default_machine_sn));
            memcpy(activation_time.data, default_activation_time, sizeof(default_activation_time));
            memcpy(drink_counter.data, default_drink_counter, sizeof(default_drink_counter));
            memcpy(time_counter.data, default_time_counter, sizeof(default_time_counter));
            memcpy(reserve.data, default_reserve, sizeof(default_reserve));
            memcpy(rfid.id, default_rfid_id, sizeof(default_rfid_id));
            memcpy((uint8_t*)&info_data.rfid, default_rfid_id, sizeof(default_rfid_id));
        }
    };
}




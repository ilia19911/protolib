#include <gtest/gtest.h>

#include "LacteProtocol.hpp"
#include "Echo.hpp"

using namespace proto;
namespace Lacte::Proto::Tests{
    uint8_t host_rx_buffer[300];
    uint8_t host_tx_buffer[300];
    uint8_t board_rx_buffer[300];
    uint8_t board_tx_buffer[300];

    TEST(LacteProtocolTest, Main){
        HostLacteProtocol<host_rx_buffer, host_tx_buffer> host_proto{false};
        BoardLacteProtocol<board_rx_buffer, board_tx_buffer> board_proto{false};

        auto version_data = Lacte::Proto::VersionPacketType{1,0};
        auto info_data = Lacte::Proto::InfoPacketType{BoardStatus::IDLE, 0};
        auto uid_data = Lacte::Proto::UIDPacketType{1,0};
        auto rfid = Lacte::Proto::RFIDPacketType{};
        auto rfid_data = Lacte::Proto::RFIDDataPacketType{};

        proto::interface::echoInterface from_host_interface{};
        proto::interface::echoInterface from_board_interface{};
        from_host_interface.Open();
        from_board_interface.Open();
        from_board_interface.AddReceiveCallback([&host_proto](Span<uint8_t> span, size_t &read){
            host_proto.RxContainer.Fill(span, read);
        });
        host_proto.TxContainer.SetInterface(from_host_interface);

        from_host_interface.AddReceiveCallback([&board_proto](Span<uint8_t> span, size_t &read){
            uint8_t data[span.size()];
            memcpy(data, span.data(), span.size());
            board_proto.RxContainer.Fill(span, read);
        });
        board_proto.TxContainer.SetInterface(from_board_interface);

        board_proto.RxContainer.SetReceiveHandler([&](auto &container){
            auto& field = container.template Get<proto::FieldName::TYPE_FIELD>();
            if(*field.GetData() == packetNumbers::VERSION){
                board_proto.Answer(packetNumbers::VERSION, version_data);
            }
            if(*field.GetData() == packetNumbers::INFO){
                board_proto.Answer(packetNumbers::INFO, info_data);
            }
            if(*field.GetData() == packetNumbers::UID){
                board_proto.Answer(packetNumbers::UID, uid_data);
            }
            if(*field.GetData() == packetNumbers::RFID_ID){
                board_proto.Answer(packetNumbers::RFID_ID, rfid);
            }
            if(*field.GetData() == packetNumbers::RFID_DATA){
                board_proto.Answer(packetNumbers::RFID_DATA, rfid_data);
            }
        });


        EXPECT_EQ(std::get<InfoPacketType>(host_proto.Request(packetNumbers::INFO)), info_data);
        EXPECT_EQ(std::get<VersionPacketType>(host_proto.Request(packetNumbers::VERSION)), version_data);
        EXPECT_EQ(std::get<UIDPacketType>(host_proto.Request(packetNumbers::UID)), uid_data);
        EXPECT_EQ(std::get<RFIDPacketType>(host_proto.Request(packetNumbers::RFID_ID)), rfid);
        EXPECT_EQ(std::get<RFIDDataPacketType>(host_proto.Request(packetNumbers::RFID_DATA)), rfid_data);

    }

    TEST(LacteProtocolTest, Params){
        Params::MagicWord magic_word{};
        magic_word.data[0] = 0xCC;
        magic_word.data[1] = 0xAA;

        Params::LacteSn lacte_sn{};
        lacte_sn.data[0] = 0x00;
        lacte_sn.data[1] = 0x01;
        lacte_sn.data[2] = 0x02;
        lacte_sn.data[3] = 0x03;
        lacte_sn.data[4] = 0x04;
        lacte_sn.data[5] = 0x05;

        Params::ProdDate prod_date{};
        prod_date.data[0] = 0x01;
        prod_date.data[1] = 0x02;
        prod_date.data[2] = 0x03;
        prod_date.data[3] = 0x04;

        Params::Reserve reserve{};
        reserve.data[0] = 0x05;
        reserve.data[1] = 0x06;
        reserve.data[2] = 0x07;
        reserve.data[3] = 0x08;

        Params::McuUid mcu_uid{};
        mcu_uid.data[0]  = 0x09;
        mcu_uid.data[1]  = 0x0A;
        mcu_uid.data[2]  = 0x0B;
        mcu_uid.data[3]  = 0x0C;
        mcu_uid.data[4]  = 0x0D;
        mcu_uid.data[5]  = 0x0E;
        mcu_uid.data[6]  = 0x0F;
        mcu_uid.data[7]  = 0x10;
        mcu_uid.data[8]  = 0x11;
        mcu_uid.data[9]  = 0x12;
        mcu_uid.data[10] = 0x13;
        mcu_uid.data[11] = 0x14;

        Params::MachineSn machine_sn{};
        machine_sn.data[0] = 0x15;
        machine_sn.data[1] = 0x16;
        machine_sn.data[2] = 0x17;
        machine_sn.data[3] = 0x18;

        Params::ActivationTime activation_time{};
        activation_time.data[0] = 0x19;
        activation_time.data[1] = 0x1A;
        activation_time.data[2] = 0x1B;
        activation_time.data[3] = 0x1C;

        Params::DrinkCounter drink_counter{};
        drink_counter.data[0] = 0x1D;
        drink_counter.data[1] = 0x1E;
        drink_counter.data[2] = 0x1F;
        drink_counter.data[3] = 0x20;

        Params::TimeCounter time_counter{};
        time_counter.data[0] = 0x21;
        time_counter.data[1] = 0x22;
        time_counter.data[2] = 0x23;
        time_counter.data[3] = 0x24;

        HostLacteProtocol<host_rx_buffer, host_tx_buffer> host_proto{false};
        BoardLacteProtocol<board_rx_buffer, board_tx_buffer> board_proto{false};

        proto::interface::echoInterface from_host_interface{};
        proto::interface::echoInterface from_board_interface{};
        from_host_interface.Open();
        from_board_interface.Open();
        from_board_interface.AddReceiveCallback([&host_proto](Span<uint8_t> span, size_t &read){
            host_proto.RxContainer.Fill(span, read);
        });
        host_proto.TxContainer.SetInterface(from_host_interface);

        from_host_interface.AddReceiveCallback([&board_proto](Span<uint8_t> span, size_t &read){
            uint8_t data[span.size()];
            memcpy(data, span.data(), span.size());
            board_proto.RxContainer.Fill(span, read);
        });
        board_proto.TxContainer.SetInterface(from_board_interface);

        board_proto.RxContainer.SetReceiveHandler([&](auto &container){
            auto& field = container.template Get<proto::FieldName::TYPE_FIELD>();
            auto& field_data = container.template Get<proto::FieldName::DATA_FIELD>();
            if(*field.GetData() == packetNumbers::GET_PARAMS){
                auto param = *field_data.GetData();
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

        EXPECT_EQ(std::get<Params::MagicWord>(host_proto.Request(packetNumbers::GET_PARAMS, Params::numbers::MAGIC_WORD)), magic_word);
        EXPECT_EQ(std::get<Params::LacteSn>(host_proto.Request(packetNumbers::GET_PARAMS, Params::numbers::LACTE_SN)), lacte_sn);
        EXPECT_EQ(std::get<Params::ProdDate>(host_proto.Request(packetNumbers::GET_PARAMS, Params::numbers::PROD_DATE)), prod_date);
        EXPECT_EQ(std::get<Params::McuUid>(host_proto.Request(packetNumbers::GET_PARAMS, Params::numbers::MCU_UID)), mcu_uid);
        EXPECT_EQ(std::get<Params::MachineSn>(host_proto.Request(packetNumbers::GET_PARAMS, Params::numbers::MACHINE_SN)), machine_sn);
        EXPECT_EQ(std::get<Params::ActivationTime>(host_proto.Request(packetNumbers::GET_PARAMS, Params::numbers::ACTIVATION_TIME)), activation_time);
        EXPECT_EQ(std::get<Params::DrinkCounter>(host_proto.Request(packetNumbers::GET_PARAMS, Params::numbers::DRINK_COUNTER)), drink_counter);
        EXPECT_EQ(std::get<Params::TimeCounter>(host_proto.Request(packetNumbers::GET_PARAMS, Params::numbers::TIME_COUNTER)), time_counter);

    }
}

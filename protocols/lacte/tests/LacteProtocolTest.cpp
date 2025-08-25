#include <gtest/gtest.h>

#include "LacteProtocol.hpp"
#include "LacteEndpoint.hpp"
#include <libraries/interfaces/Echo.hpp>
#include "VirtualBoard.hpp"

using namespace proto;
namespace Lacte::Proto::Tests{
    uint8_t host_rx_buffer[300];
    uint8_t host_tx_buffer[300];
    uint8_t board_rx_buffer[300];
    uint8_t board_tx_buffer[300];

    using virtual_board = VirtualBoard<board_rx_buffer, board_tx_buffer>;

    TEST(LacteProtocolTest, Main)
    {
        LacteHostProtocol_<host_rx_buffer, host_tx_buffer> host_proto;
        virtual_board board{};
        board.SetDebug(true);

        host_proto.SetInterfaces(board.from_board_interface, board.from_host_interface);
        host_proto.SetDebug(true);

        uint8_t test_buffer[100] = {0xff, 0x00};
        int read = 0;
        Span<uint8_t> test_span(test_buffer, sizeof(test_buffer));
        board.from_host_interface.Write(test_span, std::chrono::milliseconds{1000});

        uint8_t buff[] = {0xff, 0xaa, 0x0d, 0x02, 0x32, 0xff, 0xd8, 0x05, 0x47, 0x50, 0x35, 0x32, 0x30, 0x64, 0x24, 0x57, 0x9e, 0xad};
        Span<uint8_t> test_span2(buff, sizeof(buff));
        board.from_board_interface.Write(test_span2, std::chrono::milliseconds{1000});

        auto request = [&](auto type, auto&& answer_type)->void{
            auto answer = host_proto.Request(MakeFieldInfo<FieldName::TYPE_FIELD>(&type));
            ASSERT_TRUE(std::holds_alternative<std::remove_reference_t<decltype(answer_type)>>(answer));
            EXPECT_EQ(std::get<std::remove_reference_t<decltype(answer_type)>>(answer), answer_type);
        };
        // --- обычные команды ---
        request(Lacte::Proto::INFO, board.info_data);
        request(Lacte::Proto::VERSION, board.version_data);
        request(Lacte::Proto::UID, board.uid_data);
        request(Lacte::Proto::RFID_ID, board.rfid);
        request(Lacte::Proto::RFID_DATA, board.rfid_data);
        request(Lacte::Proto::RESTART, proto::EmptyDataType{});

//      auto request_param = [&](auto type, auto&& answer_type)->void{
//        auto answer = host_proto.ReguestParam(MakeFieldInfo<FieldName::TYPE_FIELD>(&type));
//        ASSERT_TRUE(std::holds_alternative<std::remove_reference_t<decltype(answer_type)>>(answer));
//        EXPECT_EQ(std::get<std::remove_reference_t<decltype(answer_type)>>(answer), answer_type);
//      };
//
//      request_param(Lacte::Proto::Params::numbers::LACTE_SN, board.lacte_sn);

//        // --- параметры (GET_PARAMS) ---
//        {
//            auto magic = host_proto.GetParam<Params::MAGIC_WORD>();
//            ASSERT_TRUE(magic.has_value());
//            EXPECT_EQ(*magic, board.magic_word);
//        }
//        {
//            auto sn = host_proto.GetParam<Params::LACTE_SN>();
//            ASSERT_TRUE(sn.has_value());
//            EXPECT_EQ(*sn, board.lacte_sn);
//        }
//        {
//            auto pd = host_proto.GetParam<Params::PROD_DATE>();
//            ASSERT_TRUE(pd.has_value());
//            EXPECT_EQ(*pd, board.prod_date);
//        }
//        {
//            auto mcu = host_proto.GetParam<Params::MCU_UID>();
//            ASSERT_TRUE(mcu.has_value());
//            EXPECT_EQ(*mcu, board.mcu_uid);
//        }
//        {
//            auto msn = host_proto.GetParam<Params::MACHINE_SN>();
//            ASSERT_TRUE(msn.has_value());
//            EXPECT_EQ(*msn, board.machine_sn);
//        }
//        {
//            auto at = host_proto.GetParam<Params::ACTIVATION_TIME>();
//            ASSERT_TRUE(at.has_value());
//            EXPECT_EQ(*at, board.activation_time);
//        }
//        {
//            auto dc = host_proto.GetParam<Params::DRINK_COUNTER>();
//            ASSERT_TRUE(dc.has_value());
//            EXPECT_EQ(*dc, board.drink_counter);
//        }
//        {
//            auto tc = host_proto.GetParam<Params::TIME_COUNTER>();
//            ASSERT_TRUE(tc.has_value());
//            EXPECT_EQ(*tc, board.time_counter);
//        }
//
//        // --- GetParamByType (по типу ответа) ---
//        {
//            auto magic_t = host_proto.GetParamByType<Params::MagicWord>();
//            ASSERT_TRUE(magic_t.has_value());
//            EXPECT_EQ(*magic_t, board.magic_word);
//        }
//        {
//            auto sn_t = host_proto.GetParamByType<Params::LacteSn>();
//            ASSERT_TRUE(sn_t.has_value());
//            EXPECT_EQ(*sn_t, board.lacte_sn);
//        }
//        {
//            auto pd_t = host_proto.GetParamByType<Params::ProdDate>();
//            ASSERT_TRUE(pd_t.has_value());
//            EXPECT_EQ(*pd_t, board.prod_date);
//        }
//        {
//            auto mcu_t = host_proto.GetParamByType<Params::McuUid>();
//            ASSERT_TRUE(mcu_t.has_value());
//            EXPECT_EQ(*mcu_t, board.mcu_uid);
//        }
//        {
//            auto msn_t = host_proto.GetParamByType<Params::MachineSn>();
//            ASSERT_TRUE(msn_t.has_value());
//            EXPECT_EQ(*msn_t, board.machine_sn);
//        }
//        {
//            auto at_t = host_proto.GetParamByType<Params::ActivationTime>();
//            ASSERT_TRUE(at_t.has_value());
//            EXPECT_EQ(*at_t, board.activation_time);
//        }
//        {
//            auto dc_t = host_proto.GetParamByType<Params::DrinkCounter>();
//            ASSERT_TRUE(dc_t.has_value());
//            EXPECT_EQ(*dc_t, board.drink_counter);
//        }
//        {
//            auto tc_t = host_proto.GetParamByType<Params::TimeCounter>();
//            ASSERT_TRUE(tc_t.has_value());
//            EXPECT_EQ(*tc_t, board.time_counter);
//        }
    }

}

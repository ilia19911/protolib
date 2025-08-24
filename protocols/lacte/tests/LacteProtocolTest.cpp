#include <gtest/gtest.h>

#include "LacteProtocol.hpp"
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
        HostLacteProtocol<host_rx_buffer, host_tx_buffer> host_proto{/*debug=*/false};
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

        // --- обычные команды ---
        {
            auto info = host_proto.Request<INFO>();
            ASSERT_TRUE(info.has_value());
            EXPECT_EQ(*info, board.info_data);
        }
        {
            auto ver = host_proto.Request<VERSION>();
            ASSERT_TRUE(ver.has_value());
            EXPECT_EQ(*ver, board.version_data);
        }
        {
            auto uid = host_proto.Request<UID>();
            ASSERT_TRUE(uid.has_value());
            EXPECT_EQ(*uid, board.uid_data);
        }
        {
            auto rfid = host_proto.Request<RFID_ID>();
            ASSERT_TRUE(rfid.has_value());
            EXPECT_EQ(*rfid, board.rfid);
        }
        {
            auto rfid_data = host_proto.Request<RFID_DATA>();
            ASSERT_TRUE(rfid_data.has_value());
            EXPECT_EQ(*rfid_data, board.rfid_data);
        }
        {
            // RESTART возвращает EmptyDataType — достаточно факта ответа
            auto rst = host_proto.Request<RESTART>();
            EXPECT_TRUE(rst.has_value());
        }

        // --- RequestByType для обычных команд ---
        {
            auto info_t = host_proto.RequestByType<InfoPacketType>();
            ASSERT_TRUE(info_t.has_value());
            EXPECT_EQ(*info_t, board.info_data);
        }
        {
            auto ver_t = host_proto.RequestByType<VersionPacketType>();
            ASSERT_TRUE(ver_t.has_value());
            EXPECT_EQ(*ver_t, board.version_data);
        }
        {
            auto uid_t = host_proto.RequestByType<UIDPacketType>();
            ASSERT_TRUE(uid_t.has_value());
            EXPECT_EQ(*uid_t, board.uid_data);
        }
        {
            auto rfid_t = host_proto.RequestByType<RFIDPacketType>();
            ASSERT_TRUE(rfid_t.has_value());
            EXPECT_EQ(*rfid_t, board.rfid);
        }
        {
            auto rfid_data_t = host_proto.RequestByType<RFIDDataPacketType>();
            ASSERT_TRUE(rfid_data_t.has_value());
            EXPECT_EQ(*rfid_data_t, board.rfid_data);
        }
        {
            // по типу ответа EmptyDataType → RESTART
            auto rst_t = host_proto.RequestByType<proto::EmptyDataType>();
            EXPECT_TRUE(rst_t.has_value());
        }

        // --- параметры (GET_PARAMS) ---
        {
            auto magic = host_proto.GetParam<Params::MAGIC_WORD>();
            ASSERT_TRUE(magic.has_value());
            EXPECT_EQ(*magic, board.magic_word);
        }
        {
            auto sn = host_proto.GetParam<Params::LACTE_SN>();
            ASSERT_TRUE(sn.has_value());
            EXPECT_EQ(*sn, board.lacte_sn);
        }
        {
            auto pd = host_proto.GetParam<Params::PROD_DATE>();
            ASSERT_TRUE(pd.has_value());
            EXPECT_EQ(*pd, board.prod_date);
        }
        {
            auto mcu = host_proto.GetParam<Params::MCU_UID>();
            ASSERT_TRUE(mcu.has_value());
            EXPECT_EQ(*mcu, board.mcu_uid);
        }
        {
            auto msn = host_proto.GetParam<Params::MACHINE_SN>();
            ASSERT_TRUE(msn.has_value());
            EXPECT_EQ(*msn, board.machine_sn);
        }
        {
            auto at = host_proto.GetParam<Params::ACTIVATION_TIME>();
            ASSERT_TRUE(at.has_value());
            EXPECT_EQ(*at, board.activation_time);
        }
        {
            auto dc = host_proto.GetParam<Params::DRINK_COUNTER>();
            ASSERT_TRUE(dc.has_value());
            EXPECT_EQ(*dc, board.drink_counter);
        }
        {
            auto tc = host_proto.GetParam<Params::TIME_COUNTER>();
            ASSERT_TRUE(tc.has_value());
            EXPECT_EQ(*tc, board.time_counter);
        }

        // --- GetParamByType (по типу ответа) ---
        {
            auto magic_t = host_proto.GetParamByType<Params::MagicWord>();
            ASSERT_TRUE(magic_t.has_value());
            EXPECT_EQ(*magic_t, board.magic_word);
        }
        {
            auto sn_t = host_proto.GetParamByType<Params::LacteSn>();
            ASSERT_TRUE(sn_t.has_value());
            EXPECT_EQ(*sn_t, board.lacte_sn);
        }
        {
            auto pd_t = host_proto.GetParamByType<Params::ProdDate>();
            ASSERT_TRUE(pd_t.has_value());
            EXPECT_EQ(*pd_t, board.prod_date);
        }
        {
            auto mcu_t = host_proto.GetParamByType<Params::McuUid>();
            ASSERT_TRUE(mcu_t.has_value());
            EXPECT_EQ(*mcu_t, board.mcu_uid);
        }
        {
            auto msn_t = host_proto.GetParamByType<Params::MachineSn>();
            ASSERT_TRUE(msn_t.has_value());
            EXPECT_EQ(*msn_t, board.machine_sn);
        }
        {
            auto at_t = host_proto.GetParamByType<Params::ActivationTime>();
            ASSERT_TRUE(at_t.has_value());
            EXPECT_EQ(*at_t, board.activation_time);
        }
        {
            auto dc_t = host_proto.GetParamByType<Params::DrinkCounter>();
            ASSERT_TRUE(dc_t.has_value());
            EXPECT_EQ(*dc_t, board.drink_counter);
        }
        {
            auto tc_t = host_proto.GetParamByType<Params::TimeCounter>();
            ASSERT_TRUE(tc_t.has_value());
            EXPECT_EQ(*tc_t, board.time_counter);
        }
    }

}

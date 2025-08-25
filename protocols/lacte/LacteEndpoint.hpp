#pragma once

#include "LacteObjects.hpp"
#include "ProtocolEndpoint.hpp"
#include "Crc16Modbus.hpp"

namespace Lacte::Proto {
    template<uint8_t *RX_BASE, uint8_t *TX_BASE>
    class LacteHostProtocol_ : public proto::ProtocolEndpoint<
            typename boardPacket<RX_BASE>::board_packet_fields,
            typename hostPacket<TX_BASE>::host_packet_fields,
            Crc16Modbus> {
    public:


    };
}
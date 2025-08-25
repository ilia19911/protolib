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
      ParamProtocol<RX_BASE> params;
      using ParamsReturnType =  typename ParamProtocol<RX_BASE>::ReceiveType;
      template<typename... Infos>
      ParamsReturnType ReguestParam(Infos&&... infos){
        auto answer = this->Request(std::forward<Infos>(infos)...);
        if(std::holds_alternative<uint8_t*>(answer)){

        }

      }

     private:

    };
}
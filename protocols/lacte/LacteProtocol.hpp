#pragma once
#include <cstdint>
#include <tuple>
#include <chrono>
#include <ctime>

#include "prototypes/container/RxContainer.hpp"
#include "prototypes/container/TxContainer.hpp"
#include "ymodem.hpp"
#include "Interface.hpp"

#include "libraries/crc/crc16Modbus/Crc16Modbus.hpp"

using namespace std::chrono_literals;
namespace Lacte::Proto{
enum packetNumbers : uint8_t {
  INFO = 0x00,
  VERSION = 0x01,
  UID = 0x02,
  RFID_ID = 0x03,
  RFID_DATA = 0x04,
  SET_PARAMS = 0x40,
  GET_PARAMS = 0x41,
  RESTART = 0x7F
};


enum class BoardStatus: uint8_t {
  IDLE = 0,
  CALIBRATION = 1,
  ERROR = 2,
  READY = 3,
  WORK = 4
};

enum class BoardErrors: uint16_t {
  ERROR_FLAG_CALIB_ERROR = 0,
  ERROR_FLAG_MOTOR_ERROR = 1,
  ERROR_FLAG_RFID_ERROR = 2,
  ERROR_FLAG_RFID_NO_CARD = 3,
  ERROR_FLAG_RFID_BAD_CARD = 4
};

#pragma pack(push, 1)
struct InfoPacketType{
  BoardStatus status;
  uint16_t errors;
  [[nodiscard]] bool IsCalibrationError() const{
    return ((errors >> (int)BoardErrors::ERROR_FLAG_CALIB_ERROR) & 1);
  }

  bool CheckError(BoardErrors offset) const{
    return ((errors >> (int)offset) & 1);
  }

  bool operator==(const InfoPacketType& other) const{
    return status == other.status && errors == other.errors;
  }
};

struct VersionPacketType{
  uint8_t major;
  uint8_t minor;
  bool operator==(const VersionPacketType& other) const{
    return major == other.major && minor == other.minor;
  }
};

struct UIDPacketType{
  uint8_t uid[12];
  bool operator==(const UIDPacketType& other) const {
      return std::memcmp(uid, other.uid, sizeof(uid)) == 0;
  }
};

struct RFIDPacketType{
  uint8_t id[7];
  bool operator==(const RFIDPacketType& other) const {
      return std::memcmp(id, other.id, sizeof(id)) == 0;
  }
};

struct RFIDDataPacketType{
  uint8_t data[48];
  bool operator==(const RFIDDataPacketType& other) const {
      return std::memcmp(data, other.data, sizeof(data)) == 0;
  }
};
#pragma pack(pop)

class Params{
 public:
  enum numbers{
    MAGIC_WORD = 0,
    LACTE_SN = 1,
    PROD_DATE = 2,
    RESERVE = 3,
    MCU_UID = 4,
    MACHINE_SN = 5,
    ACTIVATION_TIME = 6,
    DRINK_COUNTER = 7,
    TIME_COUNTER = 8,
  };

#pragma pack(push,1)
  struct MagicWord{
    const numbers name = numbers::MAGIC_WORD;
    uint8_t data[2]{};
    bool operator==(const MagicWord& other) const {
      return std::memcmp(data, other.data, sizeof(data)) == 0;
    }
  };
  struct LacteSn{
    const numbers name = numbers::LACTE_SN;
    uint8_t data[6]{};
    bool operator==(const LacteSn& other) const {
      return std::memcmp(data, other.data, sizeof(data)) == 0;
    }
  };
  struct ProdDate{
    const numbers name = numbers::PROD_DATE;
    uint8_t data[4]{};
    bool operator==(const ProdDate& other) const {
      return std::memcmp(data, other.data, sizeof(data)) == 0;
    }
  };
  struct Reserve{
    const numbers name = numbers::RESERVE;
    uint8_t data[4]{};
    bool operator==(const Reserve& other) const {
      return std::memcmp(data, other.data, sizeof(data)) == 0;
    }
  };
  struct McuUid{
    const numbers name = numbers::MCU_UID;
    uint8_t data[12]{};
    bool operator==(const McuUid& other) const {
      return std::memcmp(data, other.data, sizeof(data)) == 0;
    }
  };
  struct MachineSn{
    const numbers name = numbers::MACHINE_SN;
    uint8_t data[4]{};
    bool operator==(const MachineSn& other) const {
      return std::memcmp(data, other.data, sizeof(data)) == 0;
    }
  };
  struct ActivationTime{
    const numbers name = numbers::ACTIVATION_TIME;
    uint8_t data[4]{};
    bool operator==(const ActivationTime& other) const {
      return std::memcmp(data, other.data, sizeof(data)) == 0;
    }
  };
  struct DrinkCounter{
    const numbers name = numbers::DRINK_COUNTER;
    uint8_t data[4]{};
    bool operator==(const DrinkCounter& other) const {
      return std::memcmp(data, other.data, sizeof(data)) == 0;
    }
  };
  struct TimeCounter{
    const numbers name = numbers::TIME_COUNTER;
    uint8_t data[4]{};
    bool operator==(const TimeCounter& other) const {
      return std::memcmp(data, other.data, sizeof(data)) == 0;
    }
  };
#pragma pack(pop)

};



template<uint8_t* BASE>
struct hostPacket{
  constexpr static uint8_t host_prefix[2] = {0xFF,0x55};

  using hostPackets = std::tuple<
      proto::PacketInfo<INFO, proto::EmptyDataType>,
      proto::PacketInfo<VERSION, proto::EmptyDataType>,
      proto::PacketInfo<UID, proto::EmptyDataType>,
      proto::PacketInfo<RFID_ID, proto::EmptyDataType>,
      proto::PacketInfo<RFID_DATA, proto::EmptyDataType>,
      proto::PacketInfo<SET_PARAMS, uint8_t*>,
      proto::PacketInfo<GET_PARAMS, uint8_t*>,
      proto::PacketInfo<RESTART, uint8_t*>
  >;

  using idFieldType = proto::FieldPrototype<proto::FieldName::ID_FIELD, const uint8_t*, BASE, proto::FieldFlags::NOTHING, 2, hostPacket::host_prefix>;
  using lenFieldType = proto::FieldPrototype<proto::FieldName::LEN_FIELD, uint8_t, BASE, proto::FieldFlags::IS_IN_CRC>;
  using timeFieldType = proto::FieldPrototype<proto::FieldName::TIME_FIELD, uint32_t , BASE, proto::FieldFlags::IS_IN_CRC | proto::FieldFlags::IS_IN_LEN>;
  using typeFieldType = proto::FieldPrototype<proto::FieldName::TYPE_FIELD, uint8_t , BASE, proto::FieldFlags::IS_IN_CRC | proto::FieldFlags::IS_IN_LEN>;
  using dataFieldType = proto::DataFieldPrototype< hostPackets,BASE, proto::FieldFlags::IS_IN_CRC | proto::FieldFlags::IS_IN_LEN>;
  using crcFieldType = proto::FieldPrototype<proto::FieldName::CRC_FIELD, uint16_t, BASE,  proto::FieldFlags::REVERSE>;

  using host_packet_fields = std::tuple<
      idFieldType,
      lenFieldType,
      timeFieldType,
      typeFieldType,
      dataFieldType,
      crcFieldType
  >;
};
template<uint8_t* BASE>
struct boardPacket {
  constexpr static uint8_t board_prefix[2] = {0xFF,0xAA};



  using boardPackets = std::tuple<
      proto::PacketInfo<0x00, InfoPacketType>,
      proto::PacketInfo<0x01, VersionPacketType>,
      proto::PacketInfo<0x02, UIDPacketType>,
      proto::PacketInfo<0x03, RFIDPacketType>,
      proto::PacketInfo<0x04, RFIDDataPacketType>,
      proto::PacketInfo<0x40, uint8_t*>,
      proto::PacketInfo<0x41, uint8_t*>,
      proto::PacketInfo<0x7F, uint8_t*>
  >;
  using boardIdFieldType = proto::FieldPrototype<proto::FieldName::ID_FIELD, const uint8_t*, BASE, proto::FieldFlags::NOTHING, 2, boardPacket::board_prefix>;
  using boardLenFieldType = proto::FieldPrototype<proto::FieldName::LEN_FIELD, uint8_t, BASE, proto::FieldFlags::IS_IN_CRC>;
  using boardAnsCommFieldType = proto::FieldPrototype<proto::FieldName::TYPE_FIELD, uint8_t , BASE, proto::FieldFlags::IS_IN_CRC | proto::FieldFlags::IS_IN_LEN>;
  using boardDataFieldType = proto::DataFieldPrototype<boardPackets, BASE, proto::FieldFlags::IS_IN_CRC | proto::FieldFlags::IS_IN_LEN>;
  using boardcrcFieldType = proto::FieldPrototype<proto::FieldName::CRC_FIELD, uint16_t, BASE,  proto::FieldFlags::NOTHING>;

  using board_packet_fields = std::tuple<
      boardIdFieldType,
      boardLenFieldType,
      boardAnsCommFieldType,
      boardDataFieldType,
      boardcrcFieldType
  >;
};

template<uint8_t *RX_BASE, uint8_t *TX_BASE>
class HostLacteProtocol{
 public:
  Params params{};
  proto::TxContainer<typename hostPacket<TX_BASE>::host_packet_fields, Crc16Modbus> TxContainer;
  proto::RxContainer<typename boardPacket<RX_BASE>::board_packet_fields, Crc16Modbus> RxContainer;

    using ReturnVariant = std::variant<
            std::monostate,
            InfoPacketType,
            VersionPacketType,
            UIDPacketType,
            RFIDPacketType,
            RFIDDataPacketType,
            Params::MagicWord,
            Params::LacteSn,
            Params::McuUid,
            Params::ActivationTime,
            Params::DrinkCounter,
            Params::ProdDate,
            Params::MachineSn,
            Params::TimeCounter
    >;

    template<typename T = proto::EmptyDataType>
    ReturnVariant Request(packetNumbers name, T data = {}, size_t size = sizeof(T)) {
      switch (name) {
        case INFO:
          if (auto* res = Send<INFO, InfoPacketType>(data))
            return *res;
          break;
        case VERSION:
          if (auto* res = Send<VERSION, VersionPacketType>(data))
            return *res;
          break;
        case UID:
          if (auto* res = Send<UID, UIDPacketType>(data))
            return *res;
          break;
        case RFID_ID:
          if (auto* res = Send<RFID_ID, RFIDPacketType>(data))
            return *res;
          break;
        case RFID_DATA:
          if (auto* res = Send<RFID_DATA, RFIDDataPacketType>(data))
            return *res;
          break;
        case GET_PARAMS:
          if constexpr (std::is_same_v<decltype(data), Params::numbers>) {
            if (data == Params::numbers::MAGIC_WORD) {
              if (auto* res = Send<GET_PARAMS, Params::MagicWord>(data, 1))
                return *res;
            }
            if (data == Params::numbers::LACTE_SN) {
              if (auto* res = Send<GET_PARAMS, Params::LacteSn>(data, 1))
                return *res;
            }
            if (data == Params::numbers::PROD_DATE) {
              if (auto* res = Send<GET_PARAMS, Params::ProdDate>(data, 1))
                return *res;
            }
            if (data == Params::numbers::MCU_UID) {
              if (auto* res = Send<GET_PARAMS, Params::McuUid>(data, 1))
                return *res;
            }
            if (data == Params::numbers::MACHINE_SN) {
              if (auto* res = Send<GET_PARAMS, Params::MachineSn>(data, 1))
                return *res;
            }
            if (data == Params::numbers::ACTIVATION_TIME) {
              if (auto* res = Send<GET_PARAMS, Params::ActivationTime>(data, 1))
                return *res;
            }
            if (data == Params::numbers::DRINK_COUNTER) {
              if (auto* res = Send<GET_PARAMS, Params::DrinkCounter>(data, 1))
                return *res;
            }
            if (data == Params::numbers::TIME_COUNTER) {
              if (auto* res = Send<GET_PARAMS, Params::TimeCounter>(data, 1))
                return *res;
            }
          }
          break;
        default:
          break;
      }
      return std::monostate{};
    }
  explicit HostLacteProtocol(bool debug){
    RxContainer.SetDebug(debug);
    TxContainer.SetDebug(debug);
    RxContainer.SetReceiveHandler([this](auto& container){
      if(container.IsDebug()){
        std::cout << " \n\n Packet from board is received!! " <<std::endl;
        container.for_each_type([&](auto& field){
          field.Print();
        });
      }
      received_ = true;

      if(user_callback){
        user_callback(container);
      }
    });
  }

  void AddReceiveCallback(std::function<void(proto::RxContainer<typename boardPacket<RX_BASE>::board_packet_fields, Crc16Modbus>&)> callback){
    user_callback = callback;
  }




  template <packetNumbers NAME, typename RECEIVE, typename TRANSMIT>
  RECEIVE* Send(TRANSMIT data, size_t size = sizeof(TRANSMIT)){
    packetNumbers name = NAME;
    using namespace std::chrono;
    auto now = system_clock::now();
    std::time_t unix_time = system_clock::to_time_t(now);
    received_ = false;

    TxContainer.SendPacket(proto::MakeFieldInfo<proto::FieldName::TYPE_FIELD>(&name),
                           proto::MakeFieldInfo<proto::FieldName::DATA_FIELD>(&data, size),
                           proto::MakeFieldInfo<proto::FieldName::TIME_FIELD>(&unix_time));

    std::unique_lock<std::mutex> lock(receive_mtx_);
    if(received_cv_.wait_for(lock, receive_timeout_, [this](){return received_;})){
      auto& data_field = RxContainer.template Get<proto::FieldName::DATA_FIELD>();
      if(auto* received = data_field.template GetAs<NAME, RECEIVE>()){
        return received;
      }
    }
    return nullptr;
  }
  bool Flash(const char* path){
    Ymodem ymodem(*txInerface);
    return ymodem.send(path);
  }

  void SetInterface(proto::interface::IInterface& interface){
    txInerface = &interface;
    TxContainer.SetInterface(interface);
    interface.AddReceiveCallback([this](Span<uint8_t> buffer, size_t &read){
      RxContainer.Fill(buffer, read);
    });
  }
 private:
  proto::interface::IInterface* txInerface{};
  std::function<void(proto::RxContainer<typename boardPacket<RX_BASE>::board_packet_fields, Crc16Modbus>&)> user_callback;

  std::mutex receive_mtx_;
  bool received_{false};
  std::condition_variable received_cv_;
  static constexpr std::chrono::duration receive_timeout_ = std::chrono::milliseconds{10};
};

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

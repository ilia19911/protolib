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

    struct RFIDPacketType{
        uint8_t id[7];
        bool operator==(const RFIDPacketType& other) const {
            return std::memcmp(id, other.id, sizeof(id)) == 0;
        }
    };

    struct InfoPacketType{
        BoardStatus status;
        uint16_t errors;
        RFIDPacketType rfid;
        [[nodiscard]] bool IsCalibrationError() const{
            return ((errors >> (int)BoardErrors::ERROR_FLAG_CALIB_ERROR) & 1);
        }

        bool CheckError(BoardErrors offset) const{
            return ((errors >> (int)offset) & 1);
        }

        bool operator==(const InfoPacketType& other) const{
            return status == other.status && errors == other.errors;
        }
        bool operator!=(const InfoPacketType& other) const{
            return status != other.status || errors != other.errors;
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
            numbers name = numbers::MAGIC_WORD;   // non-const to allow assignment
            uint8_t data[2]{};
            [[nodiscard]] bool validate() const { return name == numbers::MAGIC_WORD; }
            bool operator==(const MagicWord& other) const {
                return std::memcmp(data, other.data, sizeof(data)) == 0;
            }
        };
        struct LacteSn{
            numbers name = numbers::LACTE_SN;     // non-const
            uint8_t data[6]{};
            [[nodiscard]] bool validate() const { return name == numbers::LACTE_SN; }
            bool operator==(const LacteSn& other) const {
                return std::memcmp(data, other.data, sizeof(data)) == 0;
            }
        };
        struct ProdDate{
            numbers name = numbers::PROD_DATE;    // non-const
            uint8_t data[4]{};
            [[nodiscard]] bool validate() const { return name == numbers::PROD_DATE; }
            bool operator==(const ProdDate& other) const {
                return std::memcmp(data, other.data, sizeof(data)) == 0;
            }
        };
        struct Reserve{
            numbers name = numbers::RESERVE;      // non-const
            uint8_t data[4]{};
            [[nodiscard]] bool validate() const { return name == numbers::RESERVE; }
            bool operator==(const Reserve& other) const {
                return std::memcmp(data, other.data, sizeof(data)) == 0;
            }
        };
        struct McuUid{
            numbers name = numbers::MCU_UID;      // non-const
            uint8_t data[12]{};
            [[nodiscard]] bool validate() const { return name == numbers::MCU_UID; }
            bool operator==(const McuUid& other) const {
                return std::memcmp(data, other.data, sizeof(data)) == 0;
            }
        };
        struct MachineSn{
            numbers name = numbers::MACHINE_SN;   // non-const
            uint8_t data[4]{};
            [[nodiscard]] bool validate() const { return name == numbers::MACHINE_SN; }
            bool operator==(const MachineSn& other) const {
                return std::memcmp(data, other.data, sizeof(data)) == 0;
            }
        };
        struct ActivationTime{
            numbers name = numbers::ACTIVATION_TIME; // non-const
            uint8_t data[4]{};
            [[nodiscard]] bool validate() const { return name == numbers::ACTIVATION_TIME; }
            bool operator==(const ActivationTime& other) const {
                return std::memcmp(data, other.data, sizeof(data)) == 0;
            }
        };
        struct DrinkCounter{
            numbers name = numbers::DRINK_COUNTER;   // non-const
            uint8_t data[4]{};
            [[nodiscard]] bool validate() const { return name == numbers::DRINK_COUNTER; }
            bool operator==(const DrinkCounter& other) const {
                return std::memcmp(data, other.data, sizeof(data)) == 0;
            }
        };
        struct TimeCounter{
            numbers name = numbers::TIME_COUNTER;    // non-const
            uint8_t data[4]{};
            [[nodiscard]] bool validate() const { return name == numbers::TIME_COUNTER; }
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

        using idFieldType = proto::FieldPrototype<proto::FieldName::ID_FIELD, const uint8_t*, BASE, proto::FieldFlags::NOTHING, 2, 2, hostPacket::host_prefix>;
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
        using boardIdFieldType = proto::FieldPrototype<proto::FieldName::ID_FIELD, const uint8_t*, BASE, proto::FieldFlags::NOTHING, 2, 2, boardPacket::board_prefix>;
        using boardLenFieldType = proto::FieldPrototype<proto::FieldName::LEN_FIELD, uint8_t, BASE, proto::FieldFlags::IS_IN_CRC>;
        using boardAnsCommFieldType = proto::FieldPrototype<proto::FieldName::TYPE_FIELD, uint8_t , BASE, proto::FieldFlags::IS_IN_CRC | proto::FieldFlags::IS_IN_LEN>;
        using boardDataFieldType = proto::DataFieldPrototype<boardPackets, BASE, proto::FieldFlags::IS_IN_CRC | proto::FieldFlags::IS_IN_LEN>;
        using boardcrcFieldType = proto::FieldPrototype<proto::FieldName::CRC_FIELD, uint16_t, BASE,  proto::FieldFlags::REVERSE>;

        using board_packet_fields = std::tuple<
                boardIdFieldType,
                boardLenFieldType,
                boardAnsCommFieldType,
                boardDataFieldType,
                boardcrcFieldType
        >;
    };

    // ----  CommandSpec: что шлём/что ждём у обычных команд ------------------
    template<Lacte::Proto::packetNumbers N>
    struct CommandSpec; // общий шаблон — не определяем (ошибка компиляции при неверном N)

    template<> struct CommandSpec<Lacte::Proto::INFO> {
        static constexpr auto Code = Lacte::Proto::INFO;
        using Tx = proto::EmptyDataType;
        using Rx = Lacte::Proto::InfoPacketType;
    };
    template<> struct CommandSpec<Lacte::Proto::VERSION> {
        static constexpr auto Code = Lacte::Proto::VERSION;
        using Tx = proto::EmptyDataType;
        using Rx = Lacte::Proto::VersionPacketType;
    };
    template<> struct CommandSpec<Lacte::Proto::UID> {
        static constexpr auto Code = Lacte::Proto::UID;
        using Tx = proto::EmptyDataType;
        using Rx = Lacte::Proto::UIDPacketType;
    };
    template<> struct CommandSpec<Lacte::Proto::RFID_ID> {
        static constexpr auto Code = Lacte::Proto::RFID_ID;
        using Tx = proto::EmptyDataType;
        using Rx = Lacte::Proto::RFIDPacketType;
    };
    template<> struct CommandSpec<Lacte::Proto::RFID_DATA> {
        static constexpr auto Code = Lacte::Proto::RFID_DATA;
        using Tx = proto::EmptyDataType;
        using Rx = Lacte::Proto::RFIDDataPacketType;
    };
    template<> struct CommandSpec<Lacte::Proto::RESTART> {
        static constexpr auto Code = Lacte::Proto::RESTART;
        using Tx = proto::EmptyDataType;
        using Rx = proto::EmptyDataType;
    };

// ----  ParamSpec: соответствие номера параметра возвращаемому типу ------
    template<Lacte::Proto::Params::numbers P>
    struct ParamSpec; // общий шаблон — не определяем

    template<> struct ParamSpec<Lacte::Proto::Params::MAGIC_WORD>      { static constexpr auto Code = Lacte::Proto::Params::MAGIC_WORD;       using Rx = Lacte::Proto::Params::MagicWord; };
    template<> struct ParamSpec<Lacte::Proto::Params::LACTE_SN>        { static constexpr auto Code = Lacte::Proto::Params::LACTE_SN;         using Rx = Lacte::Proto::Params::LacteSn; };
    template<> struct ParamSpec<Lacte::Proto::Params::PROD_DATE>       { static constexpr auto Code = Lacte::Proto::Params::PROD_DATE;        using Rx = Lacte::Proto::Params::ProdDate; };
    template<> struct ParamSpec<Lacte::Proto::Params::MCU_UID>         { static constexpr auto Code = Lacte::Proto::Params::MCU_UID;          using Rx = Lacte::Proto::Params::McuUid; };
    template<> struct ParamSpec<Lacte::Proto::Params::MACHINE_SN>      { static constexpr auto Code = Lacte::Proto::Params::MACHINE_SN;       using Rx = Lacte::Proto::Params::MachineSn; };
    template<> struct ParamSpec<Lacte::Proto::Params::ACTIVATION_TIME> { static constexpr auto Code = Lacte::Proto::Params::ACTIVATION_TIME;  using Rx = Lacte::Proto::Params::ActivationTime; };
    template<> struct ParamSpec<Lacte::Proto::Params::DRINK_COUNTER>   { static constexpr auto Code = Lacte::Proto::Params::DRINK_COUNTER;    using Rx = Lacte::Proto::Params::DrinkCounter; };
    template<> struct ParamSpec<Lacte::Proto::Params::TIME_COUNTER>    { static constexpr auto Code = Lacte::Proto::Params::TIME_COUNTER;     using Rx = Lacte::Proto::Params::TimeCounter; };


    template<uint8_t *RX_BASE, uint8_t *TX_BASE>
    class HostLacteProtocol{
    public:
        proto::TxContainer<typename hostPacket<TX_BASE>::host_packet_fields, Crc16Modbus> TxContainer;
        proto::RxContainer<typename boardPacket<RX_BASE>::board_packet_fields, Crc16Modbus> RxContainer;



        // compact Spec holder and aggregated spec lists
        template<auto Code_, class Rx_>
        struct Spec { static constexpr auto Code = Code_; using Rx = Rx_; };

        using AllCommandSpecs = std::tuple<
            Spec<CommandSpec<INFO>::Code,      typename CommandSpec<INFO>::Rx>,
            Spec<CommandSpec<VERSION>::Code,   typename CommandSpec<VERSION>::Rx>,
            Spec<CommandSpec<UID>::Code,       typename CommandSpec<UID>::Rx>,
            Spec<CommandSpec<RFID_ID>::Code,   typename CommandSpec<RFID_ID>::Rx>,
            Spec<CommandSpec<RFID_DATA>::Code, typename CommandSpec<RFID_DATA>::Rx>,
            Spec<CommandSpec<RESTART>::Code,   typename CommandSpec<RESTART>::Rx>
        >;

        using AllParamSpecs = std::tuple<
            Spec<ParamSpec<Params::MAGIC_WORD>::Code,       typename ParamSpec<Params::MAGIC_WORD>::Rx>,
            Spec<ParamSpec<Params::LACTE_SN>::Code,         typename ParamSpec<Params::LACTE_SN>::Rx>,
            Spec<ParamSpec<Params::PROD_DATE>::Code,        typename ParamSpec<Params::PROD_DATE>::Rx>,
            Spec<ParamSpec<Params::MCU_UID>::Code,          typename ParamSpec<Params::MCU_UID>::Rx>,
            Spec<ParamSpec<Params::MACHINE_SN>::Code,       typename ParamSpec<Params::MACHINE_SN>::Rx>,
            Spec<ParamSpec<Params::ACTIVATION_TIME>::Code,  typename ParamSpec<Params::ACTIVATION_TIME>::Rx>,
            Spec<ParamSpec<Params::DRINK_COUNTER>::Code,    typename ParamSpec<Params::DRINK_COUNTER>::Rx>,
            Spec<ParamSpec<Params::TIME_COUNTER>::Code,     typename ParamSpec<Params::TIME_COUNTER>::Rx>
        >;

        // === compile-time mapping: Rx type -> numeric code from a tuple of Spec (C++17) ===
        template<class TupleSpecs, class RxT, std::size_t I = 0>
        static constexpr auto code_of_rx() {
            if constexpr (I >= std::tuple_size<TupleSpecs>::value) {
                // Not found: return default-initialized code of the first Spec's Code type
                using CodeT = decltype(std::tuple_element_t<0, TupleSpecs>::Code);
                return CodeT{};
            } else {
                using Spec = std::tuple_element_t<I, TupleSpecs>;
                if constexpr (std::is_same<typename Spec::Rx, RxT>::value) {
                    return Spec::Code;
                } else {
                    return code_of_rx<TupleSpecs, RxT, I + 1>();
                }
            }
        }

        // === user-friendly requests by RESPONSE TYPE ===
        template<class RxT>
        std::optional<RxT> RequestByType() {
            constexpr auto code = code_of_rx<AllCommandSpecs, RxT>();
            return send_opt<static_cast<Lacte::Proto::packetNumbers>(code), RxT>(proto::EmptyDataType{}, 0);
        }

        template<class RxT>
        std::optional<RxT> GetParamByType() {
            constexpr auto pcode = code_of_rx<AllParamSpecs, RxT>();
            const uint8_t code8 = static_cast<uint8_t>(pcode);
            return send_opt<Lacte::Proto::GET_PARAMS, RxT>(code8, 1);
        }

        // Универсальная обёртка над вашим Send -> std::optional<Rx>
        template<Lacte::Proto::packetNumbers NAME, typename Rx, typename Tx>
        std::optional<Rx> send_opt(const Tx& data, size_t size = sizeof(Tx)) {
            if (auto* p = Send<NAME, Rx>(data, size)) return *p;
            return std::nullopt;
        }

// Обычные запросы: типы берём из CommandSpec<N>
        template<Lacte::Proto::packetNumbers N>
        std::optional<typename CommandSpec<N>::Rx>
        Request() {
            using Tx = typename CommandSpec<N>::Tx;
            using Rx = typename CommandSpec<N>::Rx;
            return send_opt<N, Rx>(Tx{}, std::is_same_v<Tx, proto::EmptyDataType> ? 0 : sizeof(Tx));
        }

// Перегрузка, если вдруг команда N требует непустой Tx;
        template<Lacte::Proto::packetNumbers N>
        std::optional<typename CommandSpec<N>::Rx>
        Request(const typename CommandSpec<N>::Tx& tx) {
            using Tx = typename CommandSpec<N>::Tx;
            using Rx = typename CommandSpec<N>::Rx;
            return send_opt<N, Rx>(tx);
        }

// Запрос параметра (compile-time): номер параметра в шаблоне → правильный Rx автоматически.
        template<Lacte::Proto::Params::numbers P>
        std::optional<typename ParamSpec<P>::Rx>
        GetParam() {
            const uint8_t code = static_cast<uint8_t>(P);
            using Rx = typename ParamSpec<P>::Rx;
            return send_opt<Lacte::Proto::GET_PARAMS, Rx>(code, 1);
        }

        typename decltype(RxContainer)::CallbackType rx_delegate{[this](auto& container){
            if(container.IsDebug()){
                std::cout << " \n\n Packet from board is received!! " <<std::endl;
                container.for_each_type([&](auto& field){
                    field.Print();
                });
            }
            received_ = true;
            received_cv_.notify_all();

            if(user_callback){
                user_callback(container);
            }
        }};
        typename decltype(RxContainer)::Delegate rx_delegate_ptr;


        explicit HostLacteProtocol(bool debug){
            SetDebug(debug);
            rx_delegate_ptr = RxContainer.AddReceiveCallback(rx_delegate);
        }

        void SetDebug(bool debug){
            RxContainer.SetDebug(debug);
            TxContainer.SetDebug(debug);
        }

        template <packetNumbers NAME, typename RECEIVE, typename TRANSMIT>
        RECEIVE* Send(TRANSMIT data, size_t size = sizeof(TRANSMIT)){
            packetNumbers name = NAME;
            using namespace std::chrono;
            auto now = system_clock::now();
            std::time_t unix_time = system_clock::to_time_t(now);
            received_ = false;

            RECEIVE* result = nullptr;
            auto l = [&](auto& container){
                auto& data_field = RxContainer.template Get<proto::FieldName::DATA_FIELD>();
                if(auto* received = data_field.template GetAs<NAME, RECEIVE>()){
                    result = received;
                }
                return nullptr;
            };
            user_callback = l;
            TxContainer.SendPacket(proto::MakeFieldInfo<proto::FieldName::TYPE_FIELD>(&name),
                                   proto::MakeFieldInfo<proto::FieldName::DATA_FIELD>(&data, size),
                                   proto::MakeFieldInfo<proto::FieldName::TIME_FIELD>(&unix_time));

            std::unique_lock<std::mutex> lock(receive_mtx_);
            received_cv_.wait_for(lock, receive_timeout_, [this](){return received_;});
            return result;
        }
        bool Flash(const char* path){
            YmodemPrerelease ymodem(*txInerface);
            return ymodem.send(path);
        }

        void SetInterfaces(proto::interface::IInterface& rx_interface, proto::interface::IInterface& tx_interface){
            txInerface = &tx_interface;
            TxContainer.SetInterface(tx_interface);
            tx_interface_callback = rx_interface.AddReceiveCallback([this](Span<uint8_t> span, size_t &read){
                RxContainer.Fill(span, read);
            });
        }

    private:

        proto::interface::Delegate tx_interface_callback;

        proto::interface::IInterface* txInerface{};
        std::function<void(proto::RxContainer<typename boardPacket<RX_BASE>::board_packet_fields, Crc16Modbus>&)> user_callback;

        std::mutex receive_mtx_;
        bool received_{false};
        std::condition_variable received_cv_;
        static constexpr std::chrono::duration receive_timeout_ = std::chrono::milliseconds{1000};
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

    static inline std::istream& operator>>(std::istream& is, Lacte::Proto::Params::numbers& num) {
        int n;
        is >> n;
        if (n < 0 || n > Lacte::Proto::Params::numbers::TIME_COUNTER) {
            is.setstate(std::ios::failbit);
        } else {
            num = static_cast<Lacte::Proto::Params::numbers>(n);
        }
        return is;
    }

    inline void print_bytes(std::ostream& os, const uint8_t* data, std::size_t n) {
        if (!data || n == 0) return;

        std::ios old(nullptr);
        old.copyfmt(os);              // сохранить формат
        auto old_fill = os.fill();

        os << std::hex << std::setfill('0');
        for (std::size_t i = 0; i < n; ++i) {
            if (i) os << ' ';         // пробел только между байтами
            os << std::setw(2) << static_cast<unsigned>(data[i]);
        }

        os.fill(old_fill);
        os.copyfmt(old);              // восстановить формат
        os << std::endl;
    }

    inline std::ostream& operator<<(std::ostream& os, const Params::MagicWord& v) {
        os << "\nMagicWord: ";
        print_bytes(os, v.data, sizeof v.data);
        return os;
    }
    inline std::ostream& operator<<(std::ostream& os, const Params::LacteSn& v) {
        os << "\nLacteSn: ";
        print_bytes(os, v.data, sizeof v.data);
        return os;
    }
    inline std::ostream& operator<<(std::ostream& os, const Params::ProdDate& v) {
        os << "\nProdDate: ";
        print_bytes(os, v.data, sizeof v.data);
        return os;
    }
    inline std::ostream& operator<<(std::ostream& os, const Params::McuUid& v) {
        os << "\nMcuUid: ";
        print_bytes(os, v.data, sizeof v.data);
        return os;
    }
    inline std::ostream& operator<<(std::ostream& os, const Params::MachineSn& v) {
        os << "\nMachineSn: ";
        print_bytes(os, v.data, sizeof v.data);
        return os;
    }
    inline std::ostream& operator<<(std::ostream& os, const Params::ActivationTime& v) {
        os << "\nActivationTime: ";
        print_bytes(os, v.data, sizeof v.data);
        return os;
    }
    inline std::ostream& operator<<(std::ostream& os, const Params::DrinkCounter& v) {
        os << "\nDrinkCounter: ";
        print_bytes(os, v.data, sizeof v.data);
        return os;
    }
    inline std::ostream& operator<<(std::ostream& os, const Params::TimeCounter& v) {
        os << "\nTimeCounter: ";
        print_bytes(os, v.data, sizeof v.data);
        return os;
    }
    inline std::ostream& operator<<(std::ostream& os, const InfoPacketType& v) {
        os << "\nInfoPacket: status=" << static_cast<int>(v.status) << "\nErrors=0x" << std::hex << v.errors << std::endl;
        os << "RFID ID: ";
        print_bytes(os, v.rfid.id, sizeof(v.rfid.id));
        return os;
    }
    inline std::ostream& operator<<(std::ostream& os, const VersionPacketType& v) {
        os << "\nVersionPacket: major=" << static_cast<int>(v.major)
           << ", minor=" << static_cast<int>(v.minor) << std::endl;
        return os;
    }
    inline std::ostream& operator<<(std::ostream& os, const UIDPacketType& v) {
        os << "\nUIDPacket: ";
        print_bytes(os, v.uid, sizeof(v.uid));
        return os;
    }
    inline std::ostream& operator<<(std::ostream& os, const RFIDPacketType& v) {
        os << "\nRFIDPacket: ";
        print_bytes(os, v.id, sizeof(v.id));
        return os;
    }
    inline std::ostream& operator<<(std::ostream& os, const RFIDDataPacketType& v) {
        os << "\nRFIDDataPacket: ";
        print_bytes(os, v.data, sizeof(v.data));
        return os;
    }
    inline std::ostream& operator<<(std::ostream& os, const proto::EmptyDataType&) {
        os << "\nEmptyDataType" << std::endl;
        return os;
    }

//    template <class... Ts>
//    struct overloaded : Ts... { using Ts::operator()...; };
//    template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

}

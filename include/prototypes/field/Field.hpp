#pragma once

#include <cstdint>
#include <cstring>
#include <sstream>
#include <typeindex>
#include <type_traits>
#include <iostream>
#include <iomanip>
#include <array>
#include <string_view>
#include <limits>

namespace proto
{

    enum class FieldName: uint64_t
    {
        ID_FIELD [[maybe_unused]],
        ID_2_FIELD [[maybe_unused]],
        TYPE_FIELD [[maybe_unused]],
        REQ_TYPE_FIELD [[maybe_unused]],
        ANS_TYPE_FIELD [[maybe_unused]],
        LEN_FIELD [[maybe_unused]],
        ALEN_FIELD [[maybe_unused]],
        SOURCE_FIELD [[maybe_unused]],
        DEST_FIELD [[maybe_unused]],
        VERSION_FIELD [[maybe_unused]],
        NUMBER_FIELD [[maybe_unused]],
        DATA_FIELD [[maybe_unused]],
        CRC_FIELD [[maybe_unused]],
        SESSION_FIELD [[maybe_unused]],
        DUMP_FIELD [[maybe_unused]],
        HEADER_FIELD [[maybe_unused]],
        BIN_FIELD [[maybe_unused]],
        TIME_FIELD [[maybe_unused]],
        HEIGHT_FIELD [[maybe_unused]],
        WIDTH_FIELD [[maybe_unused]],
        STATUS_FIELD [[maybe_unused]],
    };

    static constexpr std::array<std::pair<FieldName, std::string_view>, 21> kFieldNameStrings{{
                                                                                              {FieldName::ID_FIELD,        "ID_FIELD"},
                                                                                              {FieldName::ID_2_FIELD,      "ID_2_FIELD"},
                                                                                              {FieldName::TYPE_FIELD,      "TYPE_FIELD"},
                                                                                              {FieldName::REQ_TYPE_FIELD,  "REQ_TYPE_FIELD"},
                                                                                              {FieldName::ANS_TYPE_FIELD,  "ANS_TYPE_FIELD"},
                                                                                              {FieldName::LEN_FIELD,       "LEN_FIELD"},
                                                                                              {FieldName::ALEN_FIELD,      "ALEN_FIELD"},
                                                                                              {FieldName::SOURCE_FIELD,    "SOURCE_FIELD"},
                                                                                              {FieldName::DEST_FIELD,      "DEST_FIELD"},
                                                                                              {FieldName::VERSION_FIELD,   "VERSION_FIELD"},
                                                                                              {FieldName::NUMBER_FIELD,    "NUMBER_FIELD"},
                                                                                              {FieldName::DATA_FIELD,      "DATA_FIELD"},
                                                                                              {FieldName::CRC_FIELD,       "CRC_FIELD"},
                                                                                              {FieldName::SESSION_FIELD,   "SESSION_FIELD"},
                                                                                              {FieldName::DUMP_FIELD,      "DUMP_FIELD"},
                                                                                              {FieldName::HEADER_FIELD,    "HEADER_FIELD"},
                                                                                              {FieldName::BIN_FIELD,       "BIN_FIELD"},
                                                                                              {FieldName::TIME_FIELD,      "TIME_FIELD"},
                                                                                              {FieldName::HEIGHT_FIELD,    "HEIGHT_FIELD"},
                                                                                              {FieldName::WIDTH_FIELD,     "WIDTH_FIELD"},
                                                                                              {FieldName::STATUS_FIELD,    "STATUS_FIELD"},
                                                                                      }};

    constexpr std::string_view ToString(FieldName name) {
        for (const auto& [kEy, kStr] : kFieldNameStrings) {
            if (kEy == name)
                return kStr;
        }
        return "UNKNOWN";
    }

    enum class FieldFlags : uint64_t {
        NOTHING    = 0,
        IS_IN_LEN  = 1,
        IS_IN_CRC  = 1 << 1,
        REVERSE    = 1 << 2,
        SUPPRESS   = 1 << 3,
        CONST_SIZE = 1 << 4
    };

    constexpr FieldFlags operator|(FieldFlags a, FieldFlags b) {
        return static_cast<FieldFlags>(static_cast<uint64_t>(a) | static_cast<uint64_t>(b));
    }

    constexpr FieldFlags operator&(FieldFlags a, FieldFlags b) {
        return static_cast<FieldFlags>(static_cast<uint64_t>(a) & static_cast<uint64_t>(b));
    }

    constexpr FieldFlags operator~(FieldFlags a) {
        return static_cast<FieldFlags>(~static_cast<uint64_t>(a));
    }

    // Provide contextual conversion for FieldFlags
    constexpr bool operator!(FieldFlags f) {
        return static_cast<uint64_t>(f) == 0;
    }

    template<typename Enum>
    constexpr bool HasFlag(Enum value, Enum flag) {
        return static_cast<uint64_t>(value & flag) != 0;
    }

    // ostream operator<< for FieldFlags
    inline std::ostream& operator<<(std::ostream& os, FieldFlags flags) {
        if (flags == FieldFlags::NOTHING) return os << "NOTHING";

        bool first = true;
        auto append = [&](const char* name) {
            if (!first) os << "|";
            os << name;
            first = false;
        };

        if (HasFlag(flags, FieldFlags::IS_IN_LEN)) append("IS_IN_LEN");
        if (HasFlag(flags, FieldFlags::IS_IN_CRC)) append("IS_IN_CRC");
        if (HasFlag(flags, FieldFlags::REVERSE)) append("REVERSE");
        if (HasFlag(flags, FieldFlags::SUPPRESS)) append("SUPPRESS");
        if (HasFlag(flags, FieldFlags::CONST_SIZE)) append("CONST_SIZE");

        return os;
    }

    enum class MatchStatus
    {
        NOT_MATCH,
        PROCESSING,
        SUPPRESS,
        MATCH
    };

    struct EmptyDataType{
    };

static constexpr size_t kAnySize = std::numeric_limits<size_t>::max();
    static constexpr size_t kMaxFieldSize = 2048;


    template< FieldName NAME,  typename T, uint8_t* BASE, FieldFlags FLAGS, size_t SIZE = (std::is_pointer_v<T> ? kAnySize : sizeof(T)), typename std::conditional_t<
            std::is_pointer_v<T>,
            std::remove_pointer_t<T>,
            T>* CONST_VALUE = nullptr>
    class FieldPrototype  {

        template<typename Fields, typename TCrc>
        friend class FieldContainer;
        template<typename Fields, typename TCrc>
        friend class RxContainer;
        template<typename Fields, typename TCrc>
        friend class TxContainer;

    public:
        using FieldType = typename std::conditional_t<
                std::is_pointer_v<T>,
                std::remove_pointer_t<T>,
                T>;

        static constexpr FieldName name_{NAME};
        static constexpr FieldFlags flags_{FLAGS};
        static constexpr  uint8_t* const base_{BASE};
        static constexpr FieldType* const const_value_ {(FieldType*)CONST_VALUE};
        static constexpr size_t max_size_{kMaxFieldSize};

        size_t read_count_{};
        MatchStatus (*matcher_)(void*) = nullptr;

        //to avoid copying
        FieldPrototype(FieldPrototype &&) = delete;
        FieldPrototype(const FieldPrototype &) = delete;
        FieldPrototype& operator=(FieldPrototype &&) = delete;
        FieldPrototype& operator=(const FieldPrototype &) = delete;

        FieldPrototype()  {
            size_ = SIZE;
            if constexpr (!std::is_pointer_v<T>) {
                if constexpr (sizeof(T)!= SIZE) {
                    static_assert(sizeof(T) == SIZE, "Size of T must match SIZE");
                }
            }
        }
        virtual ~FieldPrototype() =default;

        [[nodiscard]] const uint8_t* begin() const {
            return BASE + offset_;
        }

        [[nodiscard]] const uint8_t* end() const {
            return BASE + offset_ + size_;
        }

        [[nodiscard]] size_t GetOffset([[maybe_unused]]void* opt = nullptr) const {
            return offset_;
        }

        [[nodiscard]] virtual size_t GetSize() const {
            return size_;
        }

        [[nodiscard]] constexpr const FieldType* GetData() const{
            return reinterpret_cast<FieldType*>(BASE + offset_);
        }

        void Print(){
            std::cout << '\n' << std::string(90, '-') << '\n';
            std::cout << "| "<< std::setw(15) << std::left << "FieldName"
                      << " | " << std::setw(24) << "Value (Hex)"
                      << " | " << std::setw(6)  << "Size"
                      << " | " << std::setw(6)  << "Offset"
                      << " | " << std::setw(10)  << "Is in len"
                      << " | " << std::setw(10)  << "Is in crc" << " |\n";


            using Type = std::decay_t<decltype(*this)>;

            std::ostringstream hex_stream;
            const auto* data = reinterpret_cast<const uint8_t*>(begin());
            for (size_t i = 0; i < GetSize(); ++i) {
                hex_stream << std::hex << std::uppercase
                          << std::setw(2) << std::setfill('0')
                          << static_cast<int>(data[i]) << " ";
            }

            std::cout << "| " << std::setw(15) << ToString(Type::name_)
                      << " | " << std::setw(24) << hex_stream.str()
                      << " | " << std::setw(6) << GetSize()
                      << " | " << std::setw(6) << GetOffset()
                      << " | " << std::setw(10) << (((flags_ & FieldFlags::IS_IN_LEN) != FieldFlags::NOTHING) ? "TRUE" : "FALSE")
                      << " | " << std::setw(10) << (((flags_ & FieldFlags::IS_IN_CRC) != FieldFlags::NOTHING) ? "TRUE" : "FALSE")
                      << " |";
            if (const_value_ != nullptr) {
                std::ostringstream const_stream;
                for (size_t i = 0; i < GetSize(); ++i) {
                    const_stream << std::hex << std::uppercase
                                << std::setw(2) << std::setfill('0')
                                << static_cast<int>(reinterpret_cast<const uint8_t*>(const_value_)[i]) << " ";
                }
                std::cout << "\n| " << std::setw(15) << "ConstValue"
                          << " | " << std::setw(24) << const_stream.str()
                          << " |";
            }
//            std::cout << std::string(90, '-');
        }
    protected:
        size_t size_{SIZE};
        size_t offset_{0};

        template<class DATA>
        constexpr void SetSize( size_t size_to_set = kAnySize) {
            if constexpr (const_value_!= nullptr){
                static_assert(CONST_VALUE == nullptr, "You can't set size for fields with const_value");
            }
            if constexpr (SIZE == kAnySize){
                if constexpr (not std::is_same_v<std::remove_pointer_t<DATA>, proto::EmptyDataType>){
                    constexpr size_t kStaticSize = std::is_pointer_v<DATA> ? 0 : sizeof(DATA);
                    if (size_to_set != kAnySize){
                        this->size_ = size_to_set;
                    }
                    else if constexpr (kStaticSize > 0) {
                        this->size_ = kStaticSize;
                    }else{
//                        static_assert(false, "DataFieldPrototype size not found in Packets");
                    }
                }
                else{
                    this->size_ = 0;
                }
            }
        }

        void SetOffset(size_t offset) {
            offset_ = offset;
        }

        constexpr void Set(const T &value) {
            static_assert(CONST_VALUE == nullptr, "Const value mustn't be provided for Set");
            if constexpr (std::is_pointer_v<T>) {
                Set((void*)value);
//                std::memcpy(BASE + offset_, value, GetSize());
            } else {
                Set((void*)&value);
//                *reinterpret_cast<T*>(BASE + offset_) = value;
            }
        }

        virtual void Set(const void* value) {
            if constexpr ((FLAGS & FieldFlags::REVERSE) != FieldFlags::NOTHING){
                for(int i = 0; i < GetSize(); i ++){
                    (BASE + offset_)[i] = ((uint8_t*)value)[GetSize()-1-i];
                }
            }
            else{
                std::memcpy(BASE + offset_, value, GetSize());
            }
        }

        void ApplyConst() {
            if (CONST_VALUE!= nullptr){
                if constexpr ((FLAGS & FieldFlags::REVERSE) != FieldFlags::NOTHING){
                    for(int i = 0; i < GetSize(); i ++){
                        (BASE + offset_)[i] = CONST_VALUE[GetSize()-1-i];
                    }
                }
                else{
                    std::memcpy(BASE + offset_, CONST_VALUE, GetSize());
                }
            }
        }


        virtual void Reset()  {
            offset_ = 0;
          read_count_ = 0;
            size_ = SIZE;
//            if (CONST_VALUE == nullptr){
//                if constexpr (!std::is_pointer_v<T>){
//                    size_ = sizeof(T);
//                } else {
//                    size_ = 0;
//                }
//            }
        }

    };


    template <typename Field>
    struct FieldTraits {
        using Type = typename std::remove_reference<Field>::type;
        static constexpr auto const_value = Type::const_value_;
        static constexpr auto name = Type::name_;
        static constexpr auto flags = Type::flags_;
        static constexpr bool is_const = (Type::const_value != nullptr);

    };

    template<typename Field>
    struct fieldsTuple; // основа

    template<FieldName NAME, typename T, uint8_t* BASE, FieldFlags FLAGS, size_t SIZE, auto* CONST_VALUE>
    struct fieldsTuple<proto::FieldPrototype<NAME, T, BASE, FLAGS, SIZE, CONST_VALUE>> {
        using Type = FieldPrototype<NAME, T, BASE, FLAGS, SIZE, CONST_VALUE>;
    };

}

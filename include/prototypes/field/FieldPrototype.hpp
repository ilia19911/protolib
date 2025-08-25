#pragma once
/**
 * @file FieldPrototype.hpp
 * @brief FieldPrototype template for describing and handling protocol fields.
 *
 * Provides:
 *  - Definition of field metadata (name, flags, size, offset, const values);
 *  - Access to raw field data in a shared buffer;
 *  - Helpers for printing, setting, and resetting field content;
 *  - Traits for compile-time inspection of field properties.
 */

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

#include "prototypes/field/FieldFlags.hpp"

namespace proto {

    /**
     * @enum MatchStatus
     * @brief Status returned by matcher functions when comparing field values.
     */
    enum class MatchStatus {
        NOT_MATCH,   //!< Field does not match expected value.
        PROCESSING,  //!< Matching is in progress.
        MATCH        //!< Field matches expected value.
    };

    /**
     * @struct EmptyDataType
     * @brief Marker type representing "no data".
     *
     * Used in templates where an optional data type is required.
     */
    struct EmptyDataType {
        bool operator==(const EmptyDataType& other) const{
            return true;
        }

    };

    /// Special constant meaning "size can be any".
    static constexpr size_t kAnySize = std::numeric_limits<size_t>::max();

    /// Maximum allowed field size in bytes.
//    static constexpr size_t kMaxFieldSize = 2048;

    /// Matcher function type alias.
    using MatcherType = MatchStatus (*)(void*);

    /**
     * @class FieldPrototype
     * @brief Template for protocol field definition and runtime handling.
     *
     * Each field has:
     *  - Name (enum @ref proto::FieldName);
     *  - Type (`T`), which can be a plain type or pointer;
     *  - Shared buffer base pointer (`BASE`);
     *  - Flags (@ref FieldFlags);
     *  - Compile-time or dynamic size;
     *  - Optional constant value (for fixed fields).
     *
     * @tparam NAME       FieldName identifier.
     * @tparam T          Field type (plain type or pointer).
     * @tparam BASE       Base pointer to shared buffer.
     * @tparam FLAGS      FieldFlags mask.
     * @tparam SIZE       Compile-time size (or @ref kAnySize for dynamic).
     * @tparam CONST_VALUE Optional constant pointer to a fixed value.
     */
    template<
            FieldName NAME,
            typename T,
            uint8_t* BASE,
            FieldFlags FLAGS,
            size_t MAX_SIZE =4096,
            size_t SIZE = (std::is_pointer_v<T> ? kAnySize : sizeof(T)),
            typename std::conditional_t<std::is_pointer_v<T>, std::remove_pointer_t<T>, T>* CONST_VALUE = nullptr,
            MatcherType MATCHER = nullptr>
    class FieldPrototype {
        template<typename Fields, typename TCrc>
        friend class FieldContainer;
        template<typename Fields, typename TCrc>
        friend class RxContainer;
        template<typename Fields, typename TCrc>
        friend class TxContainer;

    public:
        /// Resolved type (dereferenced if T is a pointer).
        using FieldType = typename std::conditional_t<
                std::is_pointer_v<T>,
                std::remove_pointer_t<T>,
                T>;

        /// Compile-time field metadata.
        static constexpr FieldName name_{NAME};
        static constexpr FieldFlags flags_{FLAGS};
        static constexpr uint8_t* const base_{BASE};
        static constexpr FieldType* const const_value_{(FieldType*)CONST_VALUE};
//        static constexpr size_t max_size_{kMaxFieldSize};

        // Disable copying to ensure uniqueness.
        FieldPrototype(FieldPrototype&&) = delete;
        FieldPrototype(const FieldPrototype&) = delete;
        FieldPrototype& operator=(FieldPrototype&&) = delete;
        FieldPrototype& operator=(const FieldPrototype&) = delete;

        /**
         * @brief Construct a field with compile-time or dynamic size.
         *
         * Ensures that fixed-size fields match the provided SIZE.
         */
        FieldPrototype() {
            if constexpr (!std::is_pointer_v<T>) {
                if constexpr (sizeof(T) != SIZE) {
                    static_assert(sizeof(T) == SIZE, "Size of T must match SIZE");
                }
            }
        }

        virtual ~FieldPrototype() = default;

        /// @return Pointer to start of field data in buffer.
        [[nodiscard]] const uint8_t* begin() const { return BASE + offset_; }

        /// @return Pointer to one-past-end of field data in buffer.
        [[nodiscard]] const uint8_t* end() const { return BASE + offset_ + size_; }

        /// @return Current offset (relative to buffer start).
        [[nodiscard]] size_t GetOffset([[maybe_unused]] void* opt = nullptr) const { return offset_; }

        /// @return Current size in bytes.
        [[nodiscard]] virtual size_t GetSize() const { return size_ < MAX_SIZE? size_:MAX_SIZE; }

        /// @return Typed pointer to field data.
        [[nodiscard]] constexpr const FieldType* GetData() const {
            return reinterpret_cast<FieldType*>(BASE + offset_);
        }
        [[nodiscard]] constexpr const uint8_t* GetPtr() const {
            return (BASE + offset_);
        }


        /**
         * @brief Print field contents in a table format.
         *
         * Shows:
         *  - Field name
         *  - Value (hex)
         *  - Size
         *  - Offset
         *  - Inclusion in length/CRC
         *  - Const value (if defined)
         */
        void Print() {
            std::cout << '\n' << std::string(90, '-') << '\n';
            std::cout << "| " << std::setw(15) << std::left << "FieldName"
                      << " | " << std::setw(24) << "Value (Hex)"
                      << " | " << std::setw(6)  << "Size"
                      << " | " << std::setw(6)  << "Offset"
                      << " | " << std::setw(10) << "Is in len"
                      << " | " << std::setw(10) << "Is in crc" << " |\n";

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
                      << " | " << std::setw(10)
                      << (((flags_ & FieldFlags::IS_IN_LEN) != FieldFlags::NOTHING) ? "TRUE" : "FALSE")
                      << " | " << std::setw(10)
                      << (((flags_ & FieldFlags::IS_IN_CRC) != FieldFlags::NOTHING) ? "TRUE" : "FALSE")
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
        }

    protected:

        /// Runtime state: how many bytes were read into this field.
        size_t read_count_{};
        /// Optional matcher callback for this field.
        MatcherType matcher_ = MATCHER;
        /// Current size (may differ from @ref SIZE if dynamic).
        size_t size_{SIZE};

        /// Offset in the shared buffer.
        size_t offset_{0};

        /**
         * @brief Set size for dynamic fields.
         * @tparam DATA Type of field data.
         * @param size_to_set New size or @ref kAnySize to auto-deduce.
         */
        template<class DATA>
        constexpr bool SetSize(size_t size_to_set = kAnySize) {
            if constexpr (const_value_ != nullptr) {
                static_assert(CONST_VALUE == nullptr, "You can't set size for fields with const_value");
            }
            if constexpr (SIZE == kAnySize) {
                if constexpr (!std::is_same_v<std::remove_pointer_t<DATA>, proto::EmptyDataType>) {
                    constexpr size_t kStaticSize = std::is_pointer_v<DATA> ? 0 : sizeof(DATA);
                    if (size_to_set != kAnySize) {
                        if(size_to_set <= MAX_SIZE){
                            this->size_ = size_to_set;
                        }
                        else{
                            return false;
                        }
                    } else if constexpr (kStaticSize > 0) {
                        this->size_ = kStaticSize;
                    }
                } else {
                    this->size_ = 0;
                }
            }
            return true;
        }

        /// Assign offset relative to buffer start.
        void SetOffset(size_t offset) { offset_ = offset; }

        /// Set field value from typed input.
        constexpr void Set(const T& value) {
            static_assert(CONST_VALUE == nullptr, "Const value mustn't be provided for Set");
            if constexpr (std::is_pointer_v<T>) {
                Set((void*)value);
            } else {
                Set((void*)&value);
            }
        }

        /**
         * @brief Set field value from raw pointer.
         *
         * Handles flags such as @ref FieldFlags::REVERSE.
         */
        virtual void Set(const void* value) {
            if constexpr ((FLAGS & FieldFlags::REVERSE) != FieldFlags::NOTHING) {
                for (int i = 0; i < GetSize(); i++) {
                    (BASE + offset_)[i] = ((uint8_t*)value)[GetSize() - 1 - i];
                }
            } else {
                std::memcpy(BASE + offset_, value, GetSize());
            }
        }

        /**
         * @brief Apply constant value (if defined).
         *
         * Copies @ref CONST_VALUE into buffer.
         */
        void ApplyConst() {
            if constexpr (CONST_VALUE != nullptr) {
                if constexpr ((FLAGS & FieldFlags::REVERSE) != FieldFlags::NOTHING) {
                    for (int i = 0; i < GetSize(); i++) {
                        (BASE + offset_)[i] = CONST_VALUE[GetSize() - 1 - i];
                    }
                } else {
                    std::memcpy(BASE + offset_, CONST_VALUE, GetSize());
                }
            }
        }

        /// Reset field to initial state.
        virtual void Reset() {
            offset_ = 0;
            read_count_ = 0;
            size_ = SIZE;
        }
    };

    /**
     * @struct FieldTraits
     * @brief Compile-time helper to extract field properties.
     *
     * Provides static members for:
     *  - `const_value`
     *  - `name`
     *  - `flags`
     *  - `is_const`
     */
    template <typename Field>
    struct FieldTraits {
        using Type = typename std::remove_reference<Field>::type;
        static constexpr auto const_value = Type::const_value_;
        static constexpr auto name = Type::name_;
        static constexpr auto flags = Type::flags_;
        static constexpr bool is_const = (Type::const_value != nullptr);
    };

    /**
     * @struct fieldsTuple
     * @brief Specialization used to wrap FieldPrototype types.
     */
    template<typename Field>
    struct fieldsTuple; // base template

    template<FieldName NAME, typename T, uint8_t* BASE, FieldFlags FLAGS, size_t MAX_SIZE, size_t SIZE, auto* CONST_VALUE, MatcherType MATCHER>
    struct fieldsTuple<proto::FieldPrototype<NAME, T, BASE, FLAGS, MAX_SIZE, SIZE, CONST_VALUE, MATCHER>> {
        using Type = FieldPrototype<NAME, T, BASE, FLAGS, MAX_SIZE, SIZE, CONST_VALUE, MATCHER>;
    };

} // namespace proto
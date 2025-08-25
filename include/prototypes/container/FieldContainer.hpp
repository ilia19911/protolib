#pragma once
#pragma once
/**
 * @file FieldContainer.hpp
 * @brief Defines the FieldContainer class template, a generic container for protocol fields.
 *
 * This header provides a generic container template that holds protocol field instances,
 * manages CRC calculation, supports debug facilities, and provides convenient access
 * and iteration over the fields by name or index. The container is designed to work
 * with protocol field types, enabling lookup by name or index, state reset, and
 * debug output for protocol development and diagnostics.
 */

#include <stdexcept>
#include <utility>
#include <unordered_map>
#include <tuple>
#include <type_traits>
#include "prototypes/field/FieldPrototype.hpp"
#include "prototypes/field/DataField.hpp"
#include "libraries/crc/crcSoft/CrcSoft.hpp"
#include "Crc.hpp"

namespace proto
{
    /**
     * @brief Transform a tuple of field prototypes into a tuple of actual field types.
     *
     * This struct template is used to convert a tuple of field prototypes
     * (such as FieldPrototype types) into a tuple of the actual field types
     * to be stored in the container.
     *
     * @tparam Tuple Tuple of field prototype types.
     */
    template<typename Tuple>
    struct TransformToFieldsTuple;

    /**
     * @brief Specialization of TransformToFieldsTuple for std::tuple of Fields.
     *
     * For a tuple of field prototypes, produces a tuple of the corresponding field types.
     * Used internally by FieldContainer to instantiate storage for all fields.
     *
     * @tparam Fields Variadic list of field prototype types.
     */
    template<typename... Fields>
    struct TransformToFieldsTuple<std::tuple<Fields...>> {
        using Type = std::tuple<typename fieldsTuple<Fields>::Type...>;
    };

    /**
     * @brief Metafunction to find a field type by its FieldName within a tuple of field types.
     *
     * @tparam Tuple  Tuple of concrete field types (not prototypes).
     * @tparam NAME   Target FieldName to search for.
     * @tparam Index  Current index (implementation detail).
     */
    // Lookup helper that avoids out-of-bounds tuple_element instantiation
    template<typename Tuple, FieldName NAME, std::size_t Index, bool AtEnd = (Index >= std::tuple_size_v<Tuple>)>
    struct FieldByNameImpl;

    // End of recursion: no such field
    template<typename Tuple, FieldName NAME, std::size_t Index>
    struct FieldByNameImpl<Tuple, NAME, Index, true> {
        using type = void;
    };

    // Recursive case
    template<typename Tuple, FieldName NAME, std::size_t Index>
    struct FieldByNameImpl<Tuple, NAME, Index, false> {
        using Curr = std::tuple_element_t<Index, Tuple>;
        using type = std::conditional_t<
            (Curr::name_ == NAME),
            Curr,
            typename FieldByNameImpl<Tuple, NAME, Index + 1>::type
        >;
    };

    // Public alias: preserves the existing FieldByName<...>::type usage
    template<typename Tuple, FieldName NAME, std::size_t Index = 0>
    using FieldByName = FieldByNameImpl<Tuple, NAME, Index>;

    /**
     * @brief Generic container for protocol fields with CRC and debug support.
     *
     * This class template stores a tuple of protocol field instances, provides methods
     * for accessing fields by name or index, resetting all fields and CRC state,
     * and iterating over all fields. It also supports debug output for protocol diagnostics.
     *
     * @tparam Fields Tuple of field prototype types, which will be instantiated as fields.
     * @tparam TCrc CRC calculation class to use (default: CrcSoft).
     *
     * Usage:
     *   - Use Get<FieldName>() or Get<Index>() to access fields by name or index.
     *   - Use HasField<FieldName>() to check for field presence.
     *   - Use Reset() to reset all fields and CRC to default state.
     *   - Use for_each_type() to apply a function to each field.
     *   - Set debug mode via SetDebug().
     */
    template<typename Fields, typename TCrc = CrcSoft>
    class FieldContainer
    {
    public:
        /**
         * @brief Construct a FieldContainer.
         *
         * Default constructor initializes all fields and CRC to default state.
         */
        FieldContainer() = default;

        /**
         * @brief Tuple type with concrete field instances for this container.
         */
        using FieldsTuple = typename TransformToFieldsTuple<Fields>::Type;

        /**
         * @brief Return type deduced from DATA_FIELD at compile time.
         *
         * - If the container has a DATA_FIELD and that field satisfies
         *   `is_data_field_prototype<>::value`, this resolves to the field's `Variant` type.
         * - If the container has a DATA_FIELD that is a regular field, this resolves to the
         *   field's `FieldType`.
         * - If the container has no DATA_FIELD, this resolves to `void`.
         */
//        using ReturnType = typename FieldByName<FieldsTuple, FieldName::DATA_FIELD>::type::FieldType;
            using DataField  = typename FieldByName<FieldsTuple, FieldName::DATA_FIELD>::type;
            using ReturnType = decltype(std::declval<const DataField&>().GetData());

//        /**
//         * @brief Backward-compatible alias expected by some code paths.
//         * @note Provided to satisfy existing usages (e.g., ProtocolEndpoint) that refer to `Variant`.
//         */
//        using Variant = ReturnType;

        /**
         * @brief Enable or disable debug mode for the container and its fields.
         *
         * When debug is enabled, additional diagnostic information may be output
         * during protocol processing.
         *
         * @param debug Set true to enable debug mode, false to disable.
         */
        void SetDebug(bool debug){
            debug_ = debug;
        }

        /**
         * @brief Check if debug mode is enabled.
         *
         * @return true if debug mode is enabled, false otherwise.
         */
        bool IsDebug(){
            return debug_;
        }


        /**
         * @brief The number of fields in the container.
         */
        static constexpr std::size_t size = std::tuple_size<FieldsTuple>::value;

        /**
         * @brief Access a field by its FieldName (compile-time constant).
         *
         * Usage: container.Get<MY_FIELD_NAME>()
         *
         * @tparam NAME The FieldName enum or identifier of the field.
         * @tparam Index (internal, do not specify) Index to start searching from.
         * @return Reference to the field with the given name.
         * @throws (compile-time) If the field is not found, returns the first field (may static_assert or throw in future).
         */
        template<FieldName NAME, std::size_t Index = 0>
        constexpr auto& Get() {
            if constexpr (Index >= std::tuple_size<decltype(fields_)>::value) {
                return std::get<0>(fields_); // or throw an exception
            } else if constexpr (std::tuple_element_t<Index, decltype(fields_)>::name_ == NAME) {
                return std::get<Index>(fields_);
            } else {
                return Get<NAME, Index + 1>();
            }
        }

        /**
         * @brief Access a field by its zero-based index.
         *
         * Usage: container.Get<0>() for the first field, etc.
         *
         * @tparam INDEX Index of the field (compile-time constant).
         * @return Reference to the field at the given index.
         * @throws (compile-time) If index is out of bounds, triggers a static_assert.
         */
        template<uint64_t INDEX>
        constexpr auto& Get() {
            return std::get<INDEX>(fields_);
        }

        /**
         * @brief Check if a field with the given FieldName exists in the container.
         *
         * Usage: FieldContainer::HasField<MY_FIELD_NAME>()
         *
         * @tparam NAME The FieldName enum or identifier of the field.
         * @tparam Index (internal, do not specify) Index to start searching from.
         * @return true if the field exists, false otherwise.
         */
        template<FieldName NAME, std::size_t Index = 0>
        static constexpr bool HasField() {
            if constexpr (Index >= std::tuple_size<FieldsTuple>::value) {
                return false;
            } else if constexpr (std::tuple_element_t<Index, FieldsTuple>::name_ == NAME) {
                return true;
            } else {
                return HasField<NAME, Index + 1>();
            }
        }

        /**
         * @brief Reset all fields, CRC, and internal state to defaults.
         *
         * This method resets the CRC calculator, all field values (by calling their Reset()),
         * clears all field offsets, and resets the field index.
         *
         * Usage: Call before filling the container with new data.
         */
        virtual void Reset(){
            this->crc_.Reset();
            this->for_each_type([&](auto& field){
                field.Reset();
            });
            for (auto& [key, value] : offsets) {
                value = 0;
            }
            field_index_ = 0;
        }

        /**
         * @brief Apply a callable to each field in the container.
         *
         * The callable should accept a reference to the field.
         * Usage: container.for_each_type([](auto& field){ ... });
         *
         * @tparam Func Type of the callable.
         * @param f Callable to apply to each field.
         */
        template<typename Func>
        constexpr void for_each_type( Func&& f) {
            constexpr std::size_t N = std::tuple_size_v<FieldsTuple>;
            for_each_type_impl(this->fields_, std::forward<Func>(f), std::make_index_sequence<N>{});
        }

    protected:
        /**
         * @brief Debug flag for enabling/disabling protocol debug output.
         */
        bool debug_ = false;
        /**
         * @brief Tuple holding all field instances.
         */
        FieldsTuple fields_;
        /**
         * @brief Map of field buffer pointers to their offsets (used for serialization/deserialization).
         */
        std::unordered_map<uint8_t* , size_t> offsets;
        /**
         * @brief CRC calculator instance for the protocol frame.
         */
        TCrc crc_{};
        /**
         * @brief Current field index for iteration or state tracking.
         */
        size_t field_index_{};

        /**
         * @brief Helper to apply a callable to each field in the tuple (implementation).
         *
         * @tparam Tuple The tuple type holding the fields.
         * @tparam Func The callable to apply.
         * @tparam Is Index sequence for tuple expansion.
         * @param obj Tuple of fields.
         * @param f Callable to apply.
         * @param ...is Index sequence.
         */
        template<typename Tuple, typename Func, std::size_t... Is>
        constexpr void for_each_type_impl(Tuple& obj, Func&& f, std::index_sequence<Is...>) {
            (f.template operator()<std::tuple_element_t<Is, Tuple>>(std::get<Is>(obj)), ...);
        }

    private:
        /**
         * @brief Number of fields in the container (internal constant).
         */
        static constexpr std::size_t N = std::tuple_size<decltype(fields_)>::value;
    };

}
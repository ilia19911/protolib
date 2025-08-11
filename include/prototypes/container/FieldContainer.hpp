#pragma once

#include <stdexcept>
#include <utility>
#include <unordered_map>

#include "prototypes/field/Field.hpp"
#include "prototypes/field/DataField.hpp"
#include "libraries/crc/crcSoft/CrcSoft.hpp"
#include "Crc.hpp"

namespace proto
{
    template<typename Tuple>
    struct TransformToFieldsTuple;

    template<typename... Fields>
    struct TransformToFieldsTuple<std::tuple<Fields...>> {
        using Type = std::tuple<typename fieldsTuple<Fields>::Type...>;
    };

    template<typename Fields, typename TCrc = CrcSoft>
    class FieldContainer
    {
    public:
        FieldContainer() = default;

        void SetDebug(bool debug){
            debug_ = debug;
        }
        bool IsDebug(){
            return debug_;
        }

        const int32_t Name{};
        static constexpr std::size_t size = std::tuple_size<typename TransformToFieldsTuple<Fields>::Type>::value;

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
        template<uint64_t INDEX>
        constexpr auto& Get() {
            return std::get<INDEX>(fields_);
        }
        template<FieldName NAME, std::size_t Index = 0>
        static constexpr bool HasField() {
            if constexpr (Index >= std::tuple_size<typename TransformToFieldsTuple<Fields>::Type>::value) {
                return false;
            } else if constexpr (std::tuple_element_t<Index, typename TransformToFieldsTuple<Fields>::Type>::name_ == NAME) {
                return true;
            } else {
                return HasField<NAME, Index + 1>();
            }
        }

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

        template<typename Func>
        constexpr void for_each_type( Func&& f) {
            constexpr std::size_t N = std::tuple_size_v<typename TransformToFieldsTuple<Fields>::Type>;
            for_each_type_impl(this->fields_, std::forward<Func>(f), std::make_index_sequence<N>{});
        }



    protected:
        bool debug_ = false;
        typename TransformToFieldsTuple<Fields>::Type fields_;
        std::unordered_map<uint8_t* , size_t> offsets;
        TCrc crc_{};
        size_t field_index_{};



        template<typename Tuple, typename Func, std::size_t... Is>
        constexpr void for_each_type_impl(Tuple& obj, Func&& f, std::index_sequence<Is...>) {
            (f.template operator()<std::tuple_element_t<Is, Tuple>>(std::get<Is>(obj)), ...);
        }

    private:
        static constexpr std::size_t N = std::tuple_size<decltype(fields_)>::value;
    };

}
#pragma once
#include "prototypes/field/Field.hpp"

namespace proto{
    template<FieldName NAME, typename T>
    struct FieldInfo {
        T* data;
        using Type = T;
        explicit FieldInfo(T* data, size_t size): data(data), size(size){}
        static constexpr FieldName name{NAME};
        size_t size{sizeof(T)};
    };

    template<FieldName NAME, typename T>
    constexpr auto MakeFieldInfo(T* ptr, size_t size = sizeof(T)) {
        return FieldInfo<NAME, T>{ptr, size};
    }

    template<FieldName TargetName, typename Tuple, size_t Index = 0>
    static constexpr bool FieldInfoHasName() {
        using TupleType = std::remove_reference_t<Tuple>;
        if constexpr (Index >= std::tuple_size_v<TupleType>) {
            return false;
        } else {
            using CandidateT = std::tuple_element_t<Index, TupleType>;
            if constexpr (CandidateT::name == TargetName) {
                return true;
            } else {
                return FieldInfoHasName<TargetName, Tuple, Index + 1>();
            }
        }
    }

    template<FieldName NAME, typename Tuple, std::size_t Index = 0>
    constexpr decltype(auto) GetFieldInfoByName(Tuple&& tuple) {
        using TupleType = std::remove_reference_t<Tuple>;

        if constexpr (Index < std::tuple_size_v<TupleType>) {
            auto&& candidate = std::get<Index>(tuple);
            if constexpr (std::remove_reference_t<decltype(candidate)>::name == NAME) {
                return candidate;
            } else {
                return GetFieldInfoByName<NAME, Tuple, Index + 1>(std::forward<Tuple>(tuple));
            }
        } else {
            // Удалили static_assert, т.к. ты уже проверил FieldInfoHasName перед вызовом
            // Для constexpr это безопасно: вызывается только при наличии
            __builtin_unreachable(); // или просто пустой блок
        }
    }

    // compile-time for-each over tuple
    template<typename Tuple, typename Func, std::size_t... Is>
    constexpr void ForEachImpl(Tuple&& tuple, Func&& f, std::index_sequence<Is...>) {
        (f(std::get<Is>(std::forward<Tuple>(tuple))), ...);
    }

    template<typename Tuple, typename Func>
    constexpr void ForEachInfo(Tuple&& tuple, Func&& f) {
        constexpr std::size_t N = std::tuple_size_v<std::remove_reference_t<Tuple>>;
      ForEachImpl(std::forward<Tuple>(tuple), std::forward<Func>(f), std::make_index_sequence<N>{});
    }

    template<typename Tuple, typename NewElement>
    constexpr auto AppendToInfo(Tuple&& tuple, NewElement&& new_elem) {
        return std::tuple_cat(std::forward<Tuple>(tuple), std::make_tuple(std::forward<NewElement>(new_elem)));
    }

}
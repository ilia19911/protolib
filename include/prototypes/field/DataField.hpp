#pragma once

#include <tuple>
#include <type_traits>
#include <variant>
#include <stdexcept>
#include <optional>

#include "prototypes/field/Field.hpp"

namespace proto{

    template<size_t Id, typename PacketType>
    struct PacketInfo {
        static constexpr size_t id = Id;
        using type = PacketType;
    };

    template<typename PACKETS, uint8_t *BASE, FieldFlags FLAGS>
    class DataFieldPrototype : public FieldPrototype<FieldName::DATA_FIELD, uint8_t *, BASE, FLAGS> {
        template<typename Fields, typename TCrc>
        friend class FieldContainer;
        template<typename Fields, typename TCrc>
        friend class RxContainer;
        template<typename Fields, typename TCrc>
        friend class TxContainer;
    public:
        static constexpr bool IS_DATA_FIELD{true};
        using Packets = PACKETS;

        bool SetId(int id) {
            bool valid = false;
            std::apply([&](auto&&... packet) {
                ((packet.id == id ? (void)(valid = true) : (void)0), ...);
            }, Packets{});
            if (valid) current_id_ = id;
            return valid;
        }

        template<typename T>
        T* GetIf() {
            if constexpr (std::is_same_v<T, EmptyDataType>) {
                return nullptr;
            } else {
                constexpr int id = GetNumber<T>();
                if (current_id_ == id) {
                    return reinterpret_cast<T*>(this->base_ + this->offset_);
                }
                return nullptr;
            }
        }

        [[nodiscard]] size_t GetSize() const override{
            return PacketSize(current_id_);
        }

        template<int NAME, typename T>
        T* GetAs() {
            constexpr auto index = GetIndex<NAME>();
            using Current = std::tuple_element_t<index, Packets>;
            if (current_id_ == Current::id) {
                return reinterpret_cast<T*>(this->base_ + this->offset_);
            }
            return nullptr;
        }


    protected:

        template<int NAME, std::size_t I = 0>
        static constexpr int GetIndex() {
            if constexpr (I >= std::tuple_size_v<Packets>) {
                static_assert(false, "Type not found in Packets");
            } else {
                using Current = std::tuple_element_t<I, Packets>;
                if constexpr (NAME == Current::id) {
                    return I;
                } else {
                    return GetIndex<NAME, I + 1>();
                }
            }
        }

        template<typename T, std::size_t I = 0>
        static constexpr int GetNumber() {
            if constexpr (I >= std::tuple_size_v<Packets>) {
                static_assert(false, "Type not found in Packets");
            } else {
                using Current = std::tuple_element_t<I, Packets>;
                if constexpr (std::is_same_v<T, typename Current::type>) {
                    return Current::id;
                } else {
                    return GetNumber<T, I + 1>();
                }
            }
        }

        void Reset() override  {
            current_id_ = -1;
            this->size_ = 0;
        }
    private:
        int current_id_ = -1;
        [[nodiscard]] constexpr size_t PacketSize(int id) const {
            auto result = proto::kAnySize;

            // Helper to handle one PacketInfo<T>
            auto handle_one = [&](const auto& pkt) {
                using PktType = typename std::decay_t<decltype(pkt)>::type;
                if (pkt.id == id) {
                    if constexpr (std::is_pointer_v<PktType>) {
                        result = this->size_;
                    } else if constexpr (std::is_same_v<PktType, EmptyDataType>) {
                        result = 0;
                    } else {
                        result = sizeof(PktType);
                    }
                }
            };

            std::apply([&](const auto&... pkts) {
                (void)std::initializer_list<int>{ (handle_one(pkts), 0)... };
            }, Packets{});

            return result;
        }


    };

    template<typename Field>
    struct fieldsTuple; // основа

    template<typename PACKETS, uint8_t* BASE, FieldFlags FLAGS>
    struct fieldsTuple<proto::DataFieldPrototype<PACKETS, BASE, FLAGS>> {
        using Type = proto::DataFieldPrototype<PACKETS, BASE, FLAGS>;
    };

    template<typename T, typename = void>
    struct is_data_field_prototype : std::false_type {};

    template<typename T>
    struct is_data_field_prototype<
            T,
            std::void_t<decltype(std::remove_cv_t<std::remove_reference_t<T>>::IS_DATA_FIELD)>
    > : std::true_type {};
}

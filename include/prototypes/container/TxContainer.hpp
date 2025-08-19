#pragma once

#include <tuple>
#include <algorithm>
#include <unordered_map>
#include <cstring>
#include <type_traits>

#include "prototypes/field/FieldInfo.hpp"
#include "prototypes/container/FieldContainer.hpp"
#include "Span.hpp"
#include "Interface.hpp"

namespace proto
{
    template<typename Fields, typename TCrc = CrcSoft>
    class TxContainer : public FieldContainer<Fields, TCrc>
    {
    public:
        TxContainer(){
            if((*this).template HasField<FieldName::LEN_FIELD>()){
                (*this).template Get<FieldName::LEN_FIELD>().matcher_ = &TxContainer::CalcLen;
            }
            if((*this).template HasField<FieldName::ALEN_FIELD>()){
                (*this).template Get<FieldName::ALEN_FIELD>().matcher_ = &TxContainer::SetALen;
            }
            if((*this).template HasField<FieldName::CRC_FIELD>()){
                (*this).template Get<FieldName::CRC_FIELD>().matcher_ = &TxContainer::SetCrc;
            }
            this->Reset();
        }

        template<typename... Infos>
        void SendPacket(Infos&&... infos) {
            this->Reset();
            auto info_tuple = std::make_tuple(std::forward<Infos>(infos)...);
            using InfoTuple = decltype(info_tuple);

            if constexpr (TxContainer<Fields>::template HasField<FieldName::DATA_FIELD>() &&
                          TxContainer<Fields>::template HasField<FieldName::TYPE_FIELD>()){
                auto& data_field = this->template Get<FieldName::DATA_FIELD>();
                if constexpr ( FieldInfoHasName<FieldName::DATA_FIELD, InfoTuple>() && is_data_field_prototype<decltype(data_field)>::value) {
                    auto &type_field = this->template Get<FieldName::TYPE_FIELD>();
                    auto &data_info = GetFieldInfoByName< FieldName::DATA_FIELD>(info_tuple);
                    using DataType = typename std::remove_reference_t<decltype(data_info)>::Type;

                    if constexpr ( FieldInfoHasName<FieldName::TYPE_FIELD, InfoTuple>()){
                        auto &type_info = GetFieldInfoByName< FieldName::TYPE_FIELD>(info_tuple);
                        data_field.SetId(*type_info.data);
                        ConstructPacket(std::forward<Infos>(infos)...);
                    }
                    else{
                        int packet_id = data_field.template GetNumber<DataType>();
                        data_field.SetId(packet_id);
                        data_field.template SetSize<DataType>();
                        auto type_info = MakeFieldInfo<proto::FieldName::TYPE_FIELD>(&packet_id);
                        auto expanded_tuple = std::tuple_cat(info_tuple, std::make_tuple(type_info));
                        ConstructPacketFromTuple(expanded_tuple);}
                    }
            }
            else{
                ConstructPacket(std::forward<Infos>(infos)...);
            }
        }

        void Reset() override {
            FieldContainer<Fields, TCrc>::Reset();
        }

        void SetInterface(interface::IInterface &interface){
            interface_ = &interface;
        }
    private:
        template<typename InfoTuple>
        void ConstructPacketImpl(InfoTuple const& info_tuple) {


            ForEachInfo(info_tuple, [&](auto const& info) {
                using InfoT = std::decay_t<decltype(info)>;   // снимаем ссылки/const
                auto& field = this->template Get<InfoT::name>();
                field.template SetSize<typename InfoT::Type>(info.size);
            });

            this->for_each_type( [&](auto& field) {
                using FieldType = std::remove_reference_t<decltype(field)>;
                using field_type = typename std::remove_reference<decltype(field)>::type;
                field.SetOffset(this->offsets[field.base_]);
                if constexpr (FieldInfoHasName<FieldType::name_, InfoTuple>()){
                    auto &data_info = GetFieldInfoByName<FieldType::name_>(info_tuple);
                    field.Set((void*)data_info.data);
                }
                if (field.matcher_ != nullptr) {
                    field.matcher_(this);
                } else if (field_type::const_value_ != nullptr) {
                    field.ApplyConst();
                }
                this->offsets[field.base_] += field.size_;
            });
            if(this->IsDebug()){
                this-> for_each_type([&](auto& field){
                    field.Print();
                });
            }
            if(interface_){
                this-> for_each_type([&](auto& field){
                    interface_->Write({field.begin(), field.GetSize()});
                });
            }
        }

        template<typename... Infos>
        void ConstructPacket(Infos&&... infos) {
            auto info_tuple = std::make_tuple(std::forward<Infos>(infos)...);
            ConstructPacketImpl<decltype(info_tuple)>(info_tuple);
        }

        template<typename... Ts>
        void ConstructPacketFromTuple(const std::tuple<Ts...>& info_tuple) {
            ConstructPacketImpl(info_tuple);
        }


        // Overload to handle when a tuple is passed directly (to avoid nested tuple)
        template <typename... Ts>
        void ConstructPacket(const std::tuple<Ts...>& info_tuple) {
            ConstructPacketImpl(info_tuple);
        }


        interface::IInterface *interface_{};
        static MatchStatus CalcLen(void* obj) {
            auto& container = *static_cast<TxContainer<Fields>*>(obj);
            auto& len_field = container.template Get<FieldName::LEN_FIELD>();
            using LenFieldType = typename std::remove_reference_t<decltype(len_field)>::FieldType;
            LenFieldType len = 0;
            container.for_each_type([&](auto& field){
                using FieldType = std::remove_reference_t<decltype(field)>;
                if constexpr (HasFlag(FieldType::flags_, FieldFlags::IS_IN_LEN)) {
                    len += field.size_;
                }
            });
            len_field.Set(len);
            return MatchStatus::MATCH;
        }

        static MatchStatus SetALen(void* obj) {
            auto& container = *static_cast<TxContainer<Fields>*>(obj);
            auto& len_field = container.template Get<FieldName::LEN_FIELD>();
            if constexpr (TxContainer<Fields>::template HasField<FieldName::ALEN_FIELD>()){
                auto& alen_field = container.template Get<FieldName::ALEN_FIELD>();
                alen_field.Set(~(*len_field.GetData()));
            }
            return MatchStatus::MATCH;
        }

        static MatchStatus SetCrc(void *obj){
            auto& container = *static_cast<TxContainer<Fields, TCrc>*>(obj);
            auto& crc_field = container.template Get<FieldName::CRC_FIELD>();
            container.crc_.Reset();
            using crc_type =typename std::remove_reference_t<decltype(crc_field)>::FieldType;
            uint32_t crc = 0;
            container.for_each_type([&](auto& field){
                using FieldType = std::remove_reference_t<decltype(field)>;
                if constexpr (HasFlag(FieldType::flags_, FieldFlags::IS_IN_CRC)) {
                    auto *data = field.GetData();
                    size_t size = field.GetSize();
                    crc = container.crc_.Append(crc, {(uint8_t*)data, size});
                }
            });
            crc_field.Set(crc);
            return MatchStatus::MATCH;
        }
    };
}
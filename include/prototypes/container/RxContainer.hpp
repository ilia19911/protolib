#pragma once

#include "FieldContainer.hpp"
#include <tuple>
#include <algorithm>
#include <unordered_map>
#include <cstring>

#include "Span.hpp"
#include <functional>

namespace proto
{
    template<typename Fields, typename TCrc = CrcSoft>
    class RxContainer : public FieldContainer<Fields, TCrc>
    {
    public:
        using delegate = std::function<void(RxContainer<Fields, TCrc>&)>;
        RxContainer(){

            if((*this).template HasField<FieldName::LEN_FIELD>()){
                (*this).template Get<FieldName::LEN_FIELD>().matcher_ = &RxContainer::SetDataLen;
            }

            if((*this).template HasField<FieldName::ALEN_FIELD>()){
                (*this).template Get<FieldName::ALEN_FIELD>().matcher_ = &RxContainer::CheckAlen;
            }

            if((*this).template HasField<FieldName::CRC_FIELD>()){
                (*this).template Get<FieldName::CRC_FIELD>().matcher_ = &RxContainer::CheckCrc;
            }

            if((*this).template HasField<FieldName::TYPE_FIELD>()){
                (*this).template Get<FieldName::TYPE_FIELD>().matcher_ = &RxContainer::CheckType;
            }

            this->Reset();
        }

        template<typename Func, std::size_t I = 0, typename... Args>
        static MatchStatus static_for_index(std::size_t runtime_index, Func&& f, Args&&... args) {
            if constexpr (I < FieldContainer<Fields, TCrc>::size) {
                if (runtime_index == I) {
                    return f.template operator()(std::integral_constant<std::size_t, I>{} ,args...);
                } else {
                    return static_for_index<Func, I + 1>(runtime_index, std::forward<Func>(f), std::forward<Args>(args)...);
                }
            }
            return MatchStatus::NOT_MATCH;
        }

        void Fill(const Span<uint8_t> &src, size_t &read){
            Span<uint8_t> ptr = src;
            MatchStatus result = MatchStatus::NOT_MATCH;
            while(!ptr.empty() )
            {
                static_for_index(
                        this->field_index_,
                        [&](auto index_c, Span<uint8_t>& ptr, size_t& read) -> MatchStatus {
                            read = 0;
                            constexpr std::size_t I = decltype(index_c)::value;
                            auto & field = std::get<I>(this->fields_);
                            field.offset_ = this->offsets[field.base_];
                            auto result = this->FillFields<I>( ptr, read);

                            if( result == MatchStatus::NOT_MATCH){
                                if(field.read_count_!=0){
                                    read = 0;
                                }
                                if constexpr (I != 0 ){
                                    read = 0;
                                }
                                this->Reset();
                            }
                            else if(result == MatchStatus::MATCH){
                                this->offsets[field.base_] = field.size_ + field.GetOffset();
                                this->field_index_++;
                                if(this->field_index_ >= this->size)
                                {
                                    if(receive_handler_){
                                        receive_handler_(*this);
                                    }
                                    this->Reset();
                                }
                            }
                            ptr = ptr.subspan(read);
                            return result;
                        },
                        ptr, read
                );
            }
//            return result;
        }

        template<size_t Index>
        MatchStatus FillFields( Span<uint8_t>& ptr, size_t& read) {
            auto & field = std::get<Index>(this->fields_);
            if(field.GetSize() == 0){
                return MatchStatus::MATCH;
            }
            size_t byte_to_read = std::min(ptr.size(), field.GetSize() - field.read_count_);
            if (FieldTraits<decltype(field)>::const_value != nullptr){
                if constexpr (HasFlag(std::remove_reference_t<decltype(field)>::flags_, FieldFlags::REVERSE)){
                    for(int i = 0; i < byte_to_read; i ++){
                        if(ptr[i] != field.const_value_[field.GetSize() - 1 - field.read_count_ - i]){
                            ++read;
                            if(this->IsDebug()){
                                std::cout<< "Mismatch in field: " << ToString(FieldTraits<decltype(field)>::name) << " at position: " << field.read_count_ + i << std::endl;
                                std::cout << "Expected: " << (uint8_t)field.const_value_[field.GetSize() - 1 - field.read_count_ - i] << ", Received: " << (uint8_t)ptr[i] << std::endl;
                            }
                            return MatchStatus::NOT_MATCH;
                        }
                    }
                }
                else{
                    if(std::memcmp(ptr.data(), (uint8_t*)field.const_value_ + field.read_count_, byte_to_read) != 0){
                        ++read;
                        if(this->IsDebug()){
                            std::cout << "Mismatch in field: " << ToString(FieldTraits<decltype(field)>::name) << " at position: " << field.read_count_ << std::endl;
                            std::cout << "Expected: ";
                            for(size_t i = 0; i < byte_to_read; i++){
                                std::cout << ((uint8_t*)field.const_value_)[field.read_count_ + i] << " ";
                            }
                            std::cout << ", Received: ";
                            for(size_t i = 0; i < field.read_count_ + 1; i++) {
                                std::cout << (uint8_t) ptr.data()[i] << " ";
                            }
                        }
                        return MatchStatus::NOT_MATCH;
                    }
                }
            }
            read += byte_to_read;
            if constexpr (HasFlag(std::remove_reference_t<decltype(field)>::flags_, FieldFlags::REVERSE)){
                for(int i = 0; i < byte_to_read; i ++){
                    uint8_t *data = (field.base_ + field.offset_ + field.GetSize()-1) - field.read_count_ -i;
                    *data = ptr.data()[i];
                }
            }
            else{
                std::memcpy(field.base_ + field.offset_ + field.read_count_, ptr.data(), byte_to_read);
            }
            field.read_count_ += byte_to_read;

            if(field.read_count_ < field.size_){
                if constexpr (HasFlag(FieldTraits<decltype(field)>::flags, FieldFlags::SUPPRESS)){
                    return MatchStatus::SUPPRESS;
                }
                return MatchStatus::PROCESSING;
            }
            else if(field.matcher_){
                return field.matcher_((void*)this);
            }
            else {
                field.read_count_ = 0;
                return MatchStatus::MATCH;
            }
        }

        static MatchStatus SetDataLen(void* obj){
            auto& container = *static_cast<RxContainer<Fields>*>(obj);
            auto &data_field = container.template Get<FieldName::DATA_FIELD>();
            auto &len_field = container.template Get<FieldName::LEN_FIELD>();
            auto len = *len_field.GetData();

            container.for_each_type( [&](auto& field){
                if (field.name_ != FieldName::DATA_FIELD) {
                    if (HasFlag(field.flags_, FieldFlags::IS_IN_LEN)) {
                        len -= field.size_;
                    }
                }
            });

            if constexpr (RxContainer<Fields>::template HasField<FieldName::DATA_FIELD>()){
                if (data_field.size_ != 0 && data_field.size_ != kAnySize) {
                    if (len != data_field.GetSize()) {
                        if(container.IsDebug()){
                            auto expected = *len_field.GetData() + (data_field.GetSize() - len);
                            std::cout << "Mismatch in length field (method SetDataLen): expected size "
                                      << expected << ", got size" << len<< std::endl;
                        }
                        return MatchStatus::NOT_MATCH;
                    }
                    data_field.size_ = len;
                    return MatchStatus::MATCH;
                }
            }

            data_field.size_ = len;
            return MatchStatus::MATCH;
        }

        static MatchStatus CheckAlen(void *obj){
            auto& container = *static_cast<RxContainer<Fields>*>(obj);
            auto len = *container.template Get<FieldName::LEN_FIELD>().GetData();
            auto alen = *container.template Get<FieldName::ALEN_FIELD>().GetData();

            alen = ~alen;

            bool result = len == alen;
            if(container.IsDebug() && not result){
                std::cout << "Mismatch in ALEN field: expected " << ~len
                          << ", got " << ~alen << std::endl;
            }
            return result ? MatchStatus::MATCH : MatchStatus::NOT_MATCH;
        }

        static MatchStatus CheckCrc(void *obj){
            auto& container = *static_cast<RxContainer<Fields, TCrc>*>(obj);
            auto crc_in_field = *container.template Get<FieldName::CRC_FIELD>().GetData();
            int crc = 0;
            container.crc_.Reset();

            container.for_each_type([&](auto& field){
                using field_type = typename std::remove_reference<decltype(field)>::type;
                if constexpr (HasFlag(field_type::flags_, FieldFlags::IS_IN_CRC)) {
                    auto *data = field.GetData();
                    size_t size = field.GetSize();
                    crc = container.crc_.Append(crc, {(uint8_t*)data, size});
                }
            });

            bool result = crc_in_field == static_cast<decltype(crc_in_field)>(crc);
            if(container.IsDebug() && not result){
                std::cout << "Mismatch in CRC field: expected " << crc
                          << ", got " << crc_in_field << std::endl;
            }
            return result? MatchStatus::MATCH : MatchStatus::NOT_MATCH;
        }

        static MatchStatus CheckType(void *obj){
            auto& container = *static_cast<RxContainer<Fields>*>(obj);

            int type = *container.template Get<FieldName::TYPE_FIELD>().GetData();
            auto& data_field = container.template Get<FieldName::DATA_FIELD>();

            if constexpr (is_data_field_prototype<decltype(data_field)>::value) {
                if (not data_field.SetId(type)){
                    return MatchStatus::NOT_MATCH;
                }
            }
            size_t packet_size = data_field.GetSize();
            if(packet_size!= kAnySize ){
                if(data_field.size_!=0 && data_field.size_ !=packet_size){

                    if(container.IsDebug()){
                        std::cout << "Mismatch in data field size(method CheckType): expected size" << packet_size
                                  << ", got " << data_field.size_ << std::endl;
                        std::cout << "Type field value: " << type << std::endl;
                    }

                    return MatchStatus::NOT_MATCH;
                }
                else{
                    data_field.size_ = packet_size;
                }
                return MatchStatus::MATCH;
            }
            return MatchStatus::NOT_MATCH;
        }

        [[nodiscard]] size_t GetSize() const {
            return this->template Get<FieldName::DATA_FIELD>().GetSize();
        }
        void SetReceiveHandler(delegate handler) {
            receive_handler_ = handler;
        }
    private:
        delegate receive_handler_;
    };
}
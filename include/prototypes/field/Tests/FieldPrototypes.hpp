#pragma once

#include "Field.hpp"
#include "DataField.hpp"


namespace proto{

    namespace test{
    template< FieldName  NAME,  typename T, uint8_t* BASE, proto::FieldFlags FLAGS, size_t SIZE = sizeof(T), std::conditional_t<
            std::is_pointer_v<T>,
            std::remove_pointer_t<T>,
            T>* CONST_VALUE = nullptr>
    class TestFieldPrototype : public proto::FieldPrototype<NAME, T, BASE, FLAGS, SIZE, CONST_VALUE> {
    public:
        using delegate = proto::MatchStatus (*)(void*);
        delegate TestMatcher(){
            return this->matcher_;
        }
        static const size_t size{SIZE};
        size_t TestOffset() {;
            return this->offset_;
        }
        uint8_t* TestBase() {;
            return this->base_;
        }
        uint8_t* TestGetRaw() {;
            return this->GetRaw();
        }
        size_t TemplateSize(){
            return this->size_;
        };
        void TestSetOffset(size_t offset) {
            this->SetOffset(offset);
        }
        void TestSet(const T &value) {
            this->Set(value);
        }
        void TestApplyConst() {
            this->ApplyConst();
        }
        [[nodiscard]] auto* TestGet() {
            return this->Value();
        }
        [[nodiscard]] const T* TestGet() const {
            return this->Get();
        }
    };

    template<typename PACKETS, uint8_t* BASE, FieldFlags FLAGS>
    class TestDataFieldPrototype : public proto::DataFieldPrototype<PACKETS, BASE, FLAGS> {
    public:
        using delegate = proto::MatchStatus (*)(void*);
        delegate TestMatcher(){
            return this->matcher_;
        }
        size_t TemplateSize(){
            return this->size_;
        };
        size_t TemplateOffset() {;
            return this->offset_;
        }
        void TestSetOffset(size_t offset) {
            this->SetOffset(offset);
        }
        void TestSet(uint8_t *value) {
            this->Set(value);
        }
        [[nodiscard]] uint8_t* TestGet() {
            return this->Get();
        }
        [[nodiscard]] const uint8_t* TestGet() const {
            return this->Get();
        }
    };

#pragma pack(push, 1)
    class dataType{
    public:
        uint8_t u8{8};
        uint16_t u16{16};
        uint32_t u32{32};
        float f{3.14f};
        double d{2.718281828459045};
        [[nodiscard]] bool Equal(const dataType &other) const {
            return u8 == other.u8 && u16 == other.u16 && u32 == other.u32 &&
                   f == other.f && d == other.d;
        }
        bool operator==(const dataType &other) const {
            return Equal(other);
        }
    };
#pragma pack(pop)

#pragma pack(push, 1)
    class dataType2{
    public:
        uint8_t u8{8};
        [[nodiscard]] bool Equal(const dataType2 &other) const {
            return u8 == other.u8;
        }
        bool operator==(const dataType2 &other) const {
            return Equal(other);
        }
    };

#pragma pack(pop)
#pragma pack(push, 1)
    class dataType3{
    public:
        uint16_t u16{16};
        uint32_t u32{32};
        float f{3.14f};
        double d{2.718281828459045};
        [[nodiscard]] bool Equal(const dataType &other) const {
            return  u16 == other.u16 && u32 == other.u32 &&
                    f == other.f && d == other.d;
        }
        bool operator==(const dataType &other) const {
            return Equal(other);
        }
    };
#pragma pack(pop)

        extern uint8_t buffer1[100];
        extern uint8_t buffer2[100];
        extern uint8_t buffer3[100];
        extern uint8_t buffer4[100];
        extern uint8_t buffer5[100];
        extern uint8_t buffer6[100];
    extern uint8_t pref1[3];

    using MyPackets = std::tuple<
            proto::PacketInfo<1, dataType>,
            proto::PacketInfo<2, dataType2>,
            proto::PacketInfo<3, dataType3>,
            proto::PacketInfo<4, proto::EmptyDataType>
    >;

    using idFieldType = TestFieldPrototype<proto::FieldName::ID_FIELD, uint8_t *, buffer1, proto::FieldFlags::NOTHING, 3, pref1>;
    using lenFieldType = TestFieldPrototype<proto::FieldName::LEN_FIELD, uint8_t, buffer1, proto::FieldFlags::IS_IN_CRC | proto::FieldFlags::IS_IN_LEN>;
    using alenFieldType = TestFieldPrototype<proto::FieldName::ALEN_FIELD, uint8_t, buffer1, proto::FieldFlags::IS_IN_CRC | proto::FieldFlags::IS_IN_LEN>;
    using dataFieldType = TestFieldPrototype<proto::FieldName::DATA_FIELD, dataType, buffer1, proto::FieldFlags::IS_IN_CRC | proto::FieldFlags::IS_IN_LEN>;
    using crcFieldType = TestFieldPrototype<proto::FieldName::CRC_FIELD, uint16_t, buffer1,  proto::FieldFlags::IS_IN_LEN>;
    using proto_fields =
            std::tuple<idFieldType,
                    lenFieldType,
                    alenFieldType,
                    dataFieldType,
                    crcFieldType>;

    using idField2Type = TestFieldPrototype<proto::FieldName::ID_FIELD, uint8_t*, buffer1, proto::FieldFlags::NOTHING, 3, pref1>;
    using lenField2Type = TestFieldPrototype<proto::FieldName::LEN_FIELD, uint8_t, buffer2, proto::FieldFlags::IS_IN_CRC | proto::FieldFlags::IS_IN_LEN>;
    using alenField2Type = TestFieldPrototype<proto::FieldName::ALEN_FIELD, uint8_t, buffer3, proto::FieldFlags::IS_IN_CRC | proto::FieldFlags::IS_IN_LEN>;
    using typeField2Type = TestFieldPrototype<proto::FieldName::TYPE_FIELD, uint8_t, buffer4, proto::FieldFlags::IS_IN_CRC | proto::FieldFlags::IS_IN_LEN>;
    using dataField2Type = TestDataFieldPrototype<MyPackets, buffer5, proto::FieldFlags::IS_IN_LEN>;
    using crcField2Type = TestFieldPrototype<proto::FieldName::CRC_FIELD, uint16_t, buffer6,  proto::FieldFlags::IS_IN_LEN>;
    using proto_fields2 =
            std::tuple<idField2Type,
                    lenField2Type,
                    alenField2Type,
                    typeField2Type,
                    dataField2Type,
                    crcField2Type>;
    }

    template<auto NAME, typename T, uint8_t* BASE, FieldFlags FLAGS, size_t SIZE, auto* CONST_VALUE>
    struct fieldsTuple<test::TestFieldPrototype<NAME, T, BASE, FLAGS, SIZE, CONST_VALUE>> {
        using Type = test::TestFieldPrototype<NAME, T, BASE, FLAGS, SIZE, CONST_VALUE>;
    };

    template< typename T, uint8_t* BASE, FieldFlags FLAGS>
    struct fieldsTuple<test::TestDataFieldPrototype<T, BASE, FLAGS>> {
        using Type = test::TestDataFieldPrototype<T, BASE, FLAGS>;
    };

}
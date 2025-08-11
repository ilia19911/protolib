#include "Prototypes.hpp"



TestRxContainer<proto::test::proto_fields> rxContainer;
TestTxContainer<proto::test::proto_fields> txContainer;
TestRxContainer<proto::test::proto_fields2> rxContainer2;
TestTxContainer<proto::test::proto_fields2> txContainer2;


uint8_t TxBufferTest[100]{};
uint8_t RxBufferTest[100]{};
proto::test::dataType testType{1, 2, 3, 4.f, 2.718281828459045};
proto::test::dataType2 testType2;
proto::test::dataType3 testType3;


size_t GetTestPack(uint8_t* ptr, proto::test::dataType& obj){

    auto &lenField = rxContainer.Get<proto::FieldName::LEN_FIELD>();
    auto &alenField = rxContainer.Get<proto::FieldName::ALEN_FIELD>();
    auto &crcField = rxContainer.Get<proto::FieldName::CRC_FIELD>();

    CrcSoft crc_generator;
    crc_generator.Reset();
    uint32_t crc{0};
    uint8_t len = sizeof(proto::test::dataType) + lenField.GetSize() + alenField.GetSize() + crcField.GetSize();
    uint8_t alen = ~len;
    std::memcpy(ptr, &proto::test::pref1, sizeof(proto::test::pref1));
    ptr[3] = len;
    ptr[4] = alen;
    crc = crc_generator.Append(crc, {&ptr[3], 2});
    std::memcpy(&ptr[5], &obj, sizeof(proto::test::dataType));
    crc = crc_generator.Append(crc, {&ptr[5], sizeof(proto::test::dataType)});
    std::memcpy(&ptr[5 + sizeof(proto::test::dataType)], (uint8_t*)&crc, crcField.GetSize() );
    return sizeof(proto::test::pref1) + 2 + sizeof(proto::test::dataType) + 2;
}

size_t GetTestPack2(uint8_t* ptr, proto::test::dataType2& obj){

    auto &len2Field = rxContainer2.Get<proto::FieldName::LEN_FIELD>();
    auto &alen2Field = rxContainer2.Get<proto::FieldName::ALEN_FIELD>();
    auto &type2Field = rxContainer2.Get<proto::FieldName::TYPE_FIELD>();
    auto &crc2Field = rxContainer2.Get<proto::FieldName::CRC_FIELD>();

    CrcSoft crc_generator;
    crc_generator.Reset();
    uint32_t crc{0};
    uint8_t len = sizeof(proto::test::dataType2) + len2Field.GetSize() + alen2Field.GetSize() + type2Field.GetSize() + crc2Field.GetSize();
    uint8_t alen = ~len;

    std::memcpy(ptr, &proto::test::pref1, sizeof(proto::test::pref1));
    ptr[3] = len;
    ptr[4] = alen;
    ptr[5] = 2;
    std::memcpy(&ptr[6], &obj, sizeof(proto::test::dataType2));
    crc = crc_generator.Append(crc, {&ptr[3], 3} );
    std::memcpy(&ptr[6 + sizeof(proto::test::dataType2)], (uint8_t*)&crc, crc2Field.GetSize() );
    return sizeof(proto::test::pref1) + 2 + 1 + sizeof(proto::test::dataType2) + 2;
}

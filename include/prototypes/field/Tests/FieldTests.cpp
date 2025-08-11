
#include <gtest/gtest.h>
#include "FieldPrototypes.hpp"

TEST(FieldTest, CheckOffset) {
    proto::test::proto_fields fields[3];
    for(auto &field: fields)
    {
        std::get<1>(field).TestSetOffset(proto::test::idFieldType::size);
        std::get<2>(field).TestSetOffset(proto::test::idFieldType::size + proto::test::lenFieldType::size);
        std::get<3>(field).TestSetOffset(proto::test::idFieldType::size + proto::test::lenFieldType::size + proto::test::alenFieldType::size);
        std::get<4>(field).TestSetOffset(proto::test::idFieldType::size + proto::test::lenFieldType::size + proto::test::alenFieldType::size + proto::test::dataFieldType::size);
        uint8_t *id = proto::test::buffer1;
        uint8_t *len = id + std::get<0>(field).GetSize();
        uint8_t *alen = len + std::get<1>(field).GetSize();
        uint8_t *data = alen + std::get<2>(field).GetSize();
        uint8_t *crc = data + std::get<3>(field).GetSize();

        EXPECT_EQ(std::get<0>(field).GetData(), id);
        EXPECT_EQ(std::get<1>(field).GetData(), len);
        EXPECT_EQ(std::get<2>(field).GetData(), alen);
        EXPECT_EQ(std::get<3>(field).GetData(), (proto::test::dataType*)data);
        EXPECT_EQ(std::get<4>(field).GetData(), (uint16_t*)crc);
    }
}

TEST(FieldTest, CheckSizeInit) {
    proto::test::proto_fields field;
    auto& id = std::get<0>(field);
    auto& len = std::get<1>(field);
    auto& alen = std::get<2>(field);
    auto& data = std::get<3>(field);
    auto& crc = std::get<4>(field);
    EXPECT_EQ(id.GetSize(), 3);
    EXPECT_EQ(id.TemplateSize(), 3);
    EXPECT_EQ(len.GetSize(), sizeof(uint8_t ));
    EXPECT_EQ(len.TemplateSize(), sizeof(uint8_t ));
    EXPECT_EQ(alen.GetSize(), sizeof(uint8_t ));
    EXPECT_EQ(alen.TemplateSize(), sizeof(uint8_t ));
    EXPECT_EQ(data.GetSize(), sizeof(proto::test::dataType));
    EXPECT_EQ(data.TemplateSize(), sizeof(proto::test::dataType));
    EXPECT_EQ(crc.GetSize(), sizeof(uint16_t));
    EXPECT_EQ(crc.TemplateSize(), sizeof(uint16_t));

    proto::test::proto_fields2 field2;
    auto& id2 = std::get<0>(field2);
    auto& len2 = std::get<1>(field2);
    auto& alen2 = std::get<2>(field2);
    auto& type2 = std::get<3>(field2);
    auto& data2 = std::get<4>(field2);
    auto& crc2 = std::get<5>(field2);
    EXPECT_EQ(id2.GetSize(), 3);
    EXPECT_EQ(len2.GetSize(), sizeof(uint8_t ));
    EXPECT_EQ(alen2.GetSize(), sizeof(uint8_t ));
    // complecated field sets to anySize by default
    EXPECT_EQ(data2.GetSize(), proto::kAnySize);
    EXPECT_EQ(type2.GetSize(), sizeof(uint8_t ));
    EXPECT_EQ(crc2.GetSize(), sizeof(uint16_t));
}

TEST(FieldTest, CheckConstValue) {
     proto::test::proto_fields fields[3];
    for(auto &field: fields)
    {
        EXPECT_EQ(std::get<0>(field).const_value_, proto::test::pref1);
    }
}

TEST(FieldTest, CheckApplyConstValue) {
    proto::test::proto_fields fields[3];
    for(auto &field: fields)
    {
        memset((uint8_t*)std::get<0>(field).GetData(), 0 , std::get<0>(field).size); // Clear the buffer before applying const value
        std::get<0>(field).TestApplyConst();
        EXPECT_EQ(memcmp((uint8_t*)std::get<0>(field).GetData(), std::get<0>(field).const_value_, std::get<0>(field).size), 0);
    }
}

TEST(FieldTest, CheckSet) {
     proto::test::proto_fields fields[3];
    for(auto &field: fields)
    {
        std::get<1>(field).TestSetOffset(proto::test::idFieldType::size);
        std::get<2>(field).TestSetOffset(proto::test::idFieldType::size + proto::test::lenFieldType::size);
        std::get<3>(field).TestSetOffset(proto::test::idFieldType::size + proto::test::lenFieldType::size + proto::test::alenFieldType::size);
        std::get<4>(field).TestSetOffset(proto::test::idFieldType::size + proto::test::lenFieldType::size + proto::test::alenFieldType::size + proto::test::dataFieldType::size);

        memset( proto::test::buffer1, 255, sizeof( proto::test::buffer1)); // Clear  proto::test::buffer before setting values

        std::get<0>(field).TestApplyConst();
        EXPECT_TRUE( memcmp( proto::test::buffer1,  proto::test::pref1, sizeof( proto::test::pref1)) == 0);

        std::get<1>(field).TestSet(125);
        auto len_value = *std::get<1>(field).GetData();
        EXPECT_TRUE(*std::get<1>(field).GetData() == len_value);
        EXPECT_TRUE(memcmp(  proto::test::buffer1 + std::get<1>(field).TestOffset(), std::get<1>(field).GetData(), std::get<1>(field).size) == 0);

        std::get<2>(field).TestSet(~125);
        auto alen_value = *std::get<2>(field).GetData();
        EXPECT_TRUE(*std::get<2>(field).GetData() == alen_value);
        EXPECT_TRUE(memcmp(  proto::test::buffer1 + std::get<2>(field).TestOffset(), std::get<2>(field).GetData(), std::get<2>(field).size) == 0);

        std::get<3>(field).TestSet( proto::test::dataType{});
        auto data = *std::get<3>(field).GetData();
        EXPECT_TRUE(*std::get<3>(field).GetData() == data);
        EXPECT_TRUE(memcmp(  proto::test::buffer1 + std::get<3>(field).TestOffset(), std::get<3>(field).GetData(), std::get<3>(field).size) == 0);

        std::get<4>(field).TestSet(12358);
        auto crc = *std::get<4>(field).GetData();
        EXPECT_TRUE(*std::get<4>(field).GetData() == crc);
        EXPECT_TRUE(memcmp(  proto::test::buffer1 + std::get<4>(field).TestOffset(), std::get<4>(field).GetData(), std::get<4>(field).size) == 0);

    }
}

TEST(FieldTest, CheckIterators) {
     proto::test::proto_fields packets[3];
    for(auto &packet: packets)
    {
        std::get<1>(packet).TestSetOffset(proto::test::idFieldType::size);
        std::get<2>(packet).TestSetOffset(proto::test::idFieldType::size + proto::test::lenFieldType::size);
        std::get<3>(packet).TestSetOffset(proto::test::idFieldType::size + proto::test::lenFieldType::size + proto::test::alenFieldType::size);
        std::get<4>(packet).TestSetOffset(proto::test::idFieldType::size + proto::test::lenFieldType::size + proto::test::alenFieldType::size + proto::test::dataFieldType::size);
        auto& id = std::get<0>(packet);
        auto& len = std::get<1>(packet);
        auto& alen = std::get<2>(packet);
        auto& data = std::get<3>(packet);
        auto& crc = std::get<4>(packet);
        const auto check = [&](const auto& field) {
            size_t count = 0;
            for(const auto &b: field)
            {
                EXPECT_EQ(b, ((uint8_t*) field.GetData())[count]);
                count++;
            }
        };
        check(id);
        check(len);
        check(alen);
        check(data);
        check(crc);

    }
}
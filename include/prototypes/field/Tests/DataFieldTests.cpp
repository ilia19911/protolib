
#include <gtest/gtest.h>
#include "FieldPrototypes.hpp"


TEST(DataFieldTest, CheckSizeInit) {

    proto::test::proto_fields2 field2;
    auto& id = std::get<0>(field2);
    auto& len = std::get<1>(field2);
    auto& alen = std::get<2>(field2);
    auto& type = std::get<3>(field2);
    auto& data = std::get<4>(field2);
    auto& crc = std::get<5>(field2);
    EXPECT_EQ(id.GetSize(), 3);
    EXPECT_EQ(id.TemplateSize(), 3);
    EXPECT_EQ(len.GetSize(), sizeof(uint8_t ));
    EXPECT_EQ(len.TemplateSize(), sizeof(uint8_t ));
    EXPECT_EQ(alen.GetSize(), sizeof(uint8_t ));
    EXPECT_EQ(alen.TemplateSize(), sizeof(uint8_t ));
    EXPECT_EQ(type.GetSize(), sizeof(uint8_t ));
    EXPECT_EQ(type.TemplateSize(), sizeof(uint8_t ));
    EXPECT_EQ(data.GetSize(), proto::kAnySize);
    EXPECT_EQ(data.TemplateSize(), proto::kAnySize);
    EXPECT_EQ(crc.GetSize(), sizeof(uint16_t));
    EXPECT_EQ(crc.TemplateSize(), sizeof(uint16_t));
}

TEST(DataFieldTest, CheckDetermineDataField) {
    proto::test::proto_fields2 fields;
    auto& data= std::get<4>(fields);
    auto& id = std::get<0>(fields);
    if( !proto::is_data_field_prototype<decltype(data)>::value){
        EXPECT_TRUE(false); // "data should be dataFieldPrototype"
    }
    if( proto::is_data_field_prototype<decltype(id)>::value){
        EXPECT_TRUE(false); // "id shouldn't be dataFieldPrototype"
    }
}

TEST(DataFieldTest, CheckSetId) {
    proto::test::proto_fields2 fields;
    auto& data= std::get<4>(fields);
    EXPECT_EQ(data.GetSize(), proto::kAnySize);
    data.SetId(0);
    EXPECT_EQ(data.GetSize(), proto::kAnySize);
    data.SetId(1);
    EXPECT_EQ(data.GetSize(), sizeof(proto::test::dataType));

    if (auto* value = data.GetIf<proto::test::dataType>()) {
        // теперь value — это dataType*
        EXPECT_TRUE(value != nullptr);
    } else {
        FAIL() << "Expected variant to hold dataType";
    }
    data.SetId(2);
    EXPECT_EQ(data.GetSize(), sizeof(proto::test::dataType2));
    if (auto* value = data.GetIf<proto::test::dataType2>()) {
        // теперь value — это dataType*
        EXPECT_TRUE(value != nullptr);
    } else {
        FAIL() << "Expected variant to hold dataType";
    }

    data.SetId(3);
    EXPECT_EQ(data.GetSize(), sizeof(proto::test::dataType3));
    if (auto* value = data.GetIf<proto::test::dataType3>()) {
        // теперь value — это dataType*
        EXPECT_TRUE(value != nullptr);
    } else {
        FAIL() << "Expected variant to hold dataType";
    }
    data.SetId(4);
    EXPECT_EQ(data.GetSize(), 0);
    if (nullptr != data.GetIf<proto::EmptyDataType>()) {
        // теперь value — это dataType*
        FAIL() << "Expected variant to hold dataType";
    }
}
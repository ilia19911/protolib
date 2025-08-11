
#include <gtest/gtest.h>
#include "Prototypes.hpp"


TEST(ContainerTest, CheckIndexing) {
    proto::FieldContainer<proto::test::proto_fields> Container;

    EXPECT_EQ(rxContainer.size, 5);

    auto &id = Container.Get<0>();
    EXPECT_EQ(id.name_, proto::FieldName::ID_FIELD);
    auto &len = Container.Get<1>();
    EXPECT_EQ(len.name_, proto::FieldName::LEN_FIELD);
    auto &alen = Container.Get<2>();
    EXPECT_EQ(alen.name_, proto::FieldName::ALEN_FIELD);
    auto &data = Container.Get<3>();
    EXPECT_EQ(data.name_, proto::FieldName::DATA_FIELD);
    auto &crc = Container.Get<4>();
    EXPECT_EQ(crc.name_, proto::FieldName::CRC_FIELD);

}

TEST(ContainerTest, CheckNameIndexing) {
    proto::FieldContainer<proto::test::proto_fields> Container;

    EXPECT_EQ(Container.size, 5);
    auto &id = Container.Get<proto::FieldName::ID_FIELD>();
    EXPECT_EQ(id.name_, proto::FieldName::ID_FIELD);
    auto &len = Container.Get<proto::FieldName::LEN_FIELD>();
    EXPECT_EQ(len.name_, proto::FieldName::LEN_FIELD);
    auto &alen = Container.Get<proto::FieldName::ALEN_FIELD>();
    EXPECT_EQ(alen.name_, proto::FieldName::ALEN_FIELD);
    auto &data = Container.Get<proto::FieldName::DATA_FIELD>();
    EXPECT_EQ(data.name_, proto::FieldName::DATA_FIELD);
    auto &crc = Container.Get<proto::FieldName::CRC_FIELD>();
    EXPECT_EQ(crc.name_, proto::FieldName::CRC_FIELD);
}

TEST(ContainerTest, CheckEqualIndexing) {
    proto::FieldContainer<proto::test::proto_fields> Container;

    EXPECT_EQ(Container.size, 5);

    auto &id_name = Container.Get<proto::FieldName::ID_FIELD>();
    auto &len_name = Container.Get<proto::FieldName::LEN_FIELD>();
    auto &alen_name = Container.Get<proto::FieldName::ALEN_FIELD>();
    auto &data_name = Container.Get<proto::FieldName::DATA_FIELD>();
    auto &crc_name = Container.Get<proto::FieldName::CRC_FIELD>();

    auto &id = Container.Get<0>();
    auto &len = Container.Get<1>();
    auto &alen = Container.Get<2>();
    auto &data = Container.Get<3>();
    auto &crc = Container.Get<4>();


    EXPECT_EQ(&id_name, &id);
    EXPECT_EQ(&len_name, &len);
    EXPECT_EQ(&alen_name, &alen);
    EXPECT_EQ(&data_name, &data);
    EXPECT_EQ(&crc_name, &crc);

    EXPECT_EQ(&id, &id);
    EXPECT_EQ(&len, &len);
    EXPECT_EQ(&alen, &alen);
    EXPECT_EQ(&data, &data);
    EXPECT_EQ(&crc, &crc);

}

TEST(ContainerTest, CheckForeach) {
    proto::FieldContainer<proto::test::proto_fields> Container;

    EXPECT_EQ(Container.size, 5);
    
    int i = 0;
    Container.for_each_type([&](auto& field){
        switch (i) {
            case 0:
                EXPECT_EQ(field.name_, proto::FieldName::ID_FIELD);
                break;
            case 1:
                EXPECT_EQ(field.name_, proto::FieldName::LEN_FIELD);
                break;
            case 2:
                EXPECT_EQ(field.name_, proto::FieldName::ALEN_FIELD);
                break;
            case 3:
                EXPECT_EQ(field.name_, proto::FieldName::DATA_FIELD);
                break;
            case 4:
                EXPECT_EQ(field.name_, proto::FieldName::CRC_FIELD);
                break;
            default:
                FAIL() << "Unexpected field index: " << i;
        }
        i++;
    });

}

TEST(ContainerTest, TypesTest) {
    proto::FieldContainer<proto::test::proto_fields> Container;

    EXPECT_EQ(Container.size, 5);

    auto id = Container.Get<proto::FieldName::ID_FIELD>().GetData();
    auto len = *Container.Get<proto::FieldName::LEN_FIELD>().GetData();
    auto alen = *Container.Get<proto::FieldName::ALEN_FIELD>().GetData();
    auto data = Container.Get<proto::FieldName::DATA_FIELD>().GetData();
    auto crc = *Container.Get<proto::FieldName::CRC_FIELD>().GetData();


    static_assert(std::is_same_v<decltype(id), const uint8_t* >, "ID_FIELD must be uint8_t*");
    static_assert(std::is_same_v<decltype(len), uint8_t >, "LEN_FIELD must be uint8_t");
    static_assert(std::is_same_v<decltype(alen), uint8_t >, "ALEN_FIELD must be uint8_t");
    static_assert(std::is_same_v<decltype(data), const proto::test::dataType*>, "ID_FIELD must be dataType*");
    static_assert(std::is_same_v<decltype(crc), uint16_t >, "ALEN_FIELD must be uint8_t");

}



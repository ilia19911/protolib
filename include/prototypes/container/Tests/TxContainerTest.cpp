
#include <gtest/gtest.h>
#include "Prototypes.hpp"

using namespace proto::test;

TEST(TxContainerTest, CheckSendSimplePacket){

    auto packSize = GetTestPack(TxBufferTest, testType);
    txContainer.SendPacket(
            proto::MakeFieldInfo< proto::FieldName::DATA_FIELD>(&testType)
    );
    auto ptr = txContainer.Get<proto::FieldName::ID_FIELD>().TestBase();
    EXPECT_EQ(memcmp(TxBufferTest, ptr, packSize ),  0);
}
const uint64_t packetNum = 0;
TEST(TxContainerTest, CheckSendComplexPacket){

    auto packSize = GetTestPack2(TxBufferTest, testType2);

    txContainer2.SendPacket(
            proto::MakeFieldInfo<proto::FieldName::DATA_FIELD>(&testType2)
    );
    size_t offset = 0;
    txContainer2.for_each_type([&](auto& field){
        auto val = field.GetData();
//        EXPECT_EQ(memcmp(proto::test::buffer[i], TxBufferTest + offset, field.Size()),  0);
//        EXPECT_EQ( memcmp(val, proto::test::buffer[i], field.Size()),  0);
//        offset += field.Size();
    });
    uint8_t *data = (uint8_t *)txContainer2.Get<proto::FieldName::DATA_FIELD>().GetData();
    EXPECT_EQ(memcmp(TxBufferTest + 6, data, sizeof(testType2) ),  0);


}

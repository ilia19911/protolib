#include <gtest/gtest.h>
#include "Prototypes.hpp"
#include "Echo.hpp"


using namespace proto::test;
using namespace std::chrono_literals;

static bool received = false;

TEST(PingPongContainerTest, SanyCaseType1){

    auto transmitHandler = [](Span<uint8_t> span, size_t &read){
        rxContainer.Fill(span, read);
    };
    auto receiveHandler = [](proto::RxContainer<proto_fields> &fields){
        received = true;
        auto &data = fields.Get<proto::FieldName::DATA_FIELD>();
        EXPECT_EQ(testType, *data.GetData());
    };
    proto::interface::echoInterface interface{};
    interface.Open();
    interface.AddReceiveCallback(transmitHandler);
    txContainer.SetInterface(interface);

    rxContainer.SetReceiveHandler(receiveHandler);
    received = false;
    txContainer.SendPacket(
            proto::MakeFieldInfo< proto::FieldName::DATA_FIELD>(&testType)
    );

    EXPECT_TRUE(received);

    received = false;
    testType.d = 0.234542;
    txContainer.SendPacket(
            proto::MakeFieldInfo< proto::FieldName::DATA_FIELD>(&testType)
    );
    EXPECT_TRUE(received);

}

TEST(PingPongContainerTest, NoiseType1){
    received = false;
    uint8_t noise[] = {4,2,6,7,34,67,44,255,255,255, pref1[0], pref1[1]};
    auto transmitHandler = [](Span<uint8_t> span, size_t &read){
        rxContainer.Fill(span, read);
    };
    auto receiveHandler = [](proto::RxContainer<proto_fields> &fields){
        received = true;
        auto &data = fields.Get<proto::FieldName::DATA_FIELD>();
        EXPECT_EQ(testType, *data.GetData());
    };
    proto::interface::echoInterface interface{};
    interface.Open();
    interface.AddReceiveCallback(transmitHandler);
    txContainer.SetInterface(interface);
    rxContainer.SetReceiveHandler(receiveHandler);

    auto task = [&](auto &noiseData){
        interface.Write(Span<uint8_t>{noiseData, sizeof(noiseData)}, 1s);
        received = false;

        txContainer.SendPacket(
                proto::MakeFieldInfo< proto::FieldName::DATA_FIELD>(&testType)
        );

        EXPECT_TRUE(received);
    };
    task(noise);
    testType.f = 322;
    uint8_t wrong_len_noise[] = {pref1[0], pref1[1], pref1[2],200,200};
    task(wrong_len_noise);
}


TEST(PingPongContainerTest, SanyCaseType2){

    auto transmitHandler = [](Span<uint8_t> span, size_t &read){
        rxContainer2.Fill(span, read);
    };
    auto receiveHandler = [](proto::RxContainer<proto_fields2> &fields){
        received = true;
        auto &data_field = fields.Get<proto::FieldName::DATA_FIELD>();
        auto *data = data_field.GetIf<dataType2>();
        EXPECT_EQ(testType2, *data);
    };
    proto::interface::echoInterface interface{};
    interface.Open();
    interface.AddReceiveCallback(transmitHandler);
    txContainer2.SetInterface(interface);
    //rxContainer2.SetDebug(true);

    rxContainer2.SetReceiveHandler(receiveHandler);
    received = false;
    txContainer2.SendPacket(
            proto::MakeFieldInfo< proto::FieldName::DATA_FIELD>(&testType2)
    );

    EXPECT_TRUE(received);

    received = false;
    testType.d = 0.234542;
    txContainer2.SendPacket(
            proto::MakeFieldInfo< proto::FieldName::DATA_FIELD>(&testType2)
    );
    EXPECT_TRUE(received);

}

TEST(PingPongContainerTest, NoiseType2){
    received = false;
//    rxContainer2.SetDebug(true);
    uint8_t noise[] = {4,2,6,7,34,67,44,255,255,255, pref1[0], pref1[1]};
    auto transmitHandler = [](Span<uint8_t> span, size_t &read){
        rxContainer2.Fill(span, read);
    };
    auto receiveHandler = [](proto::RxContainer<proto_fields2> &fields){
        received = true;
        auto &data_field = fields.Get<proto::FieldName::DATA_FIELD>();
        auto *data = data_field.GetIf<dataType2>();
        EXPECT_EQ(testType2, *data);
    };
    proto::interface::echoInterface interface{};
    interface.Open();
    interface.AddReceiveCallback(transmitHandler);
    txContainer2.SetInterface(interface);
    rxContainer2.SetReceiveHandler(receiveHandler);

    auto task = [&](auto &noiseData){
        interface.Write({noiseData, sizeof(noiseData)}, 1s);
        received = false;

        txContainer2.SendPacket(
                proto::MakeFieldInfo< proto::FieldName::DATA_FIELD>(&testType2)
        );

        EXPECT_TRUE(received);
    };
    task(noise);
    testType.f = 322;
    uint8_t wrong_len_noise[] = {pref1[0], pref1[1], pref1[2],200,200};
    task(wrong_len_noise);
}
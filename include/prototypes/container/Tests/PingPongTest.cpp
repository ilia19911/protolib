#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Prototypes.hpp"
#include "Echo.hpp"
#include <cstring>

namespace {
using namespace proto;
using namespace proto::test;

// RX/TX backing buffers for simple and complex layouts
static uint8_t rx_simple_[256]{};
static uint8_t tx_simple_[256]{};
static uint8_t rx_complex_[256]{};
static uint8_t tx_complex_[256]{};

// Simple layout aliases and containers
using SimpleFieldsRx    = SympleFields<rx_simple_>;
using SimpleFieldsTx    = SympleFields<tx_simple_>;
using proto_fields_rx    = typename SimpleFieldsRx::proto_fields;
using proto_fields_tx    = typename SimpleFieldsTx::proto_fields;
static RxContainer<proto_fields_rx> rxContainer{};
static TxContainer<proto_fields_tx> txContainer{};

// Complex layout aliases and containers
using ComplexFieldsRxT  = ComplexFields<rx_complex_>;
using ComplexFieldsTxT  = ComplexFields<tx_complex_>;

using proto_fields2Rx   = typename ComplexFieldsRxT::proto_fields;
using proto_fields2Tx   = typename ComplexFieldsTxT::proto_fields;
static RxContainer<proto_fields2Rx> rxContainer2{};
static TxContainer<proto_fields2Tx> txContainer2{};

// Test payloads
static proto::test::dataType  testType{1,2,3,4.f,2.718281828459045};
static proto::test::dataType2 testType2{}; // default-inited

// Protocol ID/prefix bytes for noise synthesis
static constexpr const uint8_t* kPref1 = SimpleFieldsRx::prefix;     // 3-byte prefix
static constexpr const uint8_t* kPref2 = ComplexFieldsTxT ::prefix;   // 3-byte prefix
} // namespace


using namespace proto::test;
using namespace std::chrono_literals;

static bool received = false;

TEST(PingPongContainerTest, SanyCaseType1){

    auto transmitHandler = [](Span<uint8_t> span, size_t &read){
        rxContainer.Fill(span, read);
    };
    auto receiveHandler = [](auto &fields){
        received = true;
        auto &data = fields.template Get<proto::FieldName::DATA_FIELD>();
        EXPECT_EQ(testType, *data.GetData());
    };

    proto::interface::echoInterface interface{};
    auto d = interface.AddReceiveCallback(transmitHandler);
    interface.Open();
    txContainer.SetInterface(interface);
    auto cd = rxContainer.AddReceiveCallback(receiveHandler);

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
    uint8_t noise[] = {4,2,6,7,34,67,44,255,255,255, kPref1[0], kPref1[1]};
    auto transmitHandler = [](Span<uint8_t> span, size_t &read){
        rxContainer.Fill(span, read);
    };
    auto receiveHandler = [](decltype(rxContainer)& fields){
        received = true;
        auto &data = fields.template Get<proto::FieldName::DATA_FIELD>();
        EXPECT_EQ(testType, *data.GetData());
    };
//    auto transmitHandlerPtr = proto::interface::CreateDelegate(transmitHandler);
//    auto receiveHandlerPtr = decltype(rxContainer)::CreateDelegate(receiveHandler);
    proto::interface::echoInterface interface{};
    interface.Open();
    auto d = interface.AddReceiveCallback(transmitHandler);
    txContainer.SetInterface(interface);
    auto cd = rxContainer.AddReceiveCallback(receiveHandler);

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
    uint8_t wrong_len_noise[] = {kPref1[0], kPref1[1], kPref1[2],200,200};
    task(wrong_len_noise);
}


TEST(PingPongContainerTest, SanyCaseType2){

    auto transmitHandler = [](Span<uint8_t> span, size_t &read){
        rxContainer2.Fill(span, read);
    };
    auto receiveHandler = [](decltype(rxContainer2)& fields){
        received = true;
        auto &data_field = fields.template Get<proto::FieldName::DATA_FIELD>();
        auto *data = data_field.GetIf<dataType2>();
        EXPECT_EQ(testType2, *data);
    };
//    auto transmitHandlerPtr = proto::interface::CreateDelegate(transmitHandler);
    proto::interface::echoInterface interface{};
    interface.Open();
    auto d = interface.AddReceiveCallback(transmitHandler);
    txContainer2.SetInterface(interface);
    //rxContainer2.SetDebug(true);

    auto cd = rxContainer2.AddReceiveCallback(receiveHandler);
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
    uint8_t noise[] = {4,2,6,7,34,67,44,255,255,255, kPref1[0], kPref1[1]};
    auto transmitHandler = [](Span<uint8_t> span, size_t &read){
        rxContainer2.Fill(span, read);
    };
    auto receiveHandler = [](decltype(rxContainer2)& fields){
        received = true;
        auto &data_field = fields.template Get<proto::FieldName::DATA_FIELD>();
        auto *data = data_field.GetIf<dataType2>();
        EXPECT_EQ(testType2, *data);
    };
//    auto transmitHandlerPtr = proto::interface::CreateDelegate(transmitHandler);
    proto::interface::echoInterface interface{};
    interface.Open();
    auto d = interface.AddReceiveCallback(transmitHandler);
    txContainer2.SetInterface(interface);
    auto cd = rxContainer2.AddReceiveCallback(receiveHandler);

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
    uint8_t wrong_len_noise[] = {kPref1[0], kPref1[1], kPref1[2],200,200};
    task(wrong_len_noise);
}

/**
 * @test Verify that RX emits debug output on broken frames (CRC mismatch).
 * We enable RX debug, then intercept TX->RX path via echoInterface and corrupt
 * the last byte (part of CRC) in each transmitted chunk. We expect RX to print
 * the standard diagnostics that contain "Mismatch in CRC field" and the
 * BROKEN PACKET dump markers.
 */
TEST(PingPongContainerTest, DebugOutput_Type2_CrcMismatch) {
    using namespace ::testing;

    auto transmitHandler = [](Span<uint8_t> span, size_t &read){
        // Aggregate all parts of one frame and send as a single buffer.
        // Complex layout always emits exactly 6 parts: ID, LEN, ALEN, TYPE, DATA, CRC.
        static std::array<uint8_t, 512> frame_buf{};
        static size_t acc = 0;       // total bytes accumulated for the current frame
        static int parts = 0;        // number of chunks seen for the current frame

        // Append current chunk
        const size_t n = span.size();
        ASSERT_LT(acc + n, frame_buf.size());
        std::memcpy(frame_buf.data() + acc, span.begin(), n);
        acc   += n;
        parts += 1;

        // When the last part (CRC) arrives, corrupt the last byte and deliver the whole frame
        if (parts == 6) {
            // Flip the very last byte in the aggregated frame → CRC mismatch
            if (acc >= 1) {
                frame_buf[acc - 1] ^= 0x5A;
            }

            // Feed the entire corrupted frame to RX in a single Fill call
            Span<uint8_t> full{frame_buf.data(), acc};
            rxContainer2.Fill(full, read);

            // Reset aggregation state for the next frame
            acc = 0;
            parts = 0;
        }
        // Note: for parts 1..5 we intentionally do NOT forward anything to RX yet.
    };

    // We still want to observe that RX tried to parse and delivered a callback
    // only for valid frames; for a corrupted frame the callback must NOT fire.
    bool got_callback = false;
    auto receiveHandler = [&](decltype(rxContainer2)& fields){
        got_callback = true; // should remain false for deliberately corrupted frame
        (void)fields;
    };

//    auto transmitHandlerPtr = proto::interface::CreateDelegate(transmitHandler);

    proto::interface::echoInterface interface{};
    interface.Open();
    auto d = interface.AddReceiveCallback(transmitHandler);

    // Wire TX/RX
    txContainer2.SetInterface(interface);
    auto cd = rxContainer2.AddReceiveCallback(receiveHandler);

    // Turn on RX debug to make it print diagnostics to stdout
    rxContainer2.SetDebug(true);

    // Capture stdout during a single send of a valid payload that we corrupt in-flight
    ::testing::internal::CaptureStdout();
    (void)txContainer2.SendPacket(
        proto::MakeFieldInfo< proto::FieldName::DATA_FIELD>(&testType2)
    );
    std::string out = ::testing::internal::GetCapturedStdout();

    // The RX should not report a successful callback for a corrupted frame
    EXPECT_FALSE(got_callback) << "RX callback must not fire on CRC-mismatched frame";

    // And it should log the expected debug markers
    EXPECT_THAT(out, HasSubstr("Mismatch in CRC field"));
    EXPECT_THAT(out, HasSubstr("BROKEN PACKET START"));
    EXPECT_THAT(out, HasSubstr("BROKEN PACKET STOP"));

    // Clean up: disable debug so other tests are quiet
    rxContainer2.SetDebug(false);
}
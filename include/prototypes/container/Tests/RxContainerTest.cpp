
#include <gtest/gtest.h>
#include "Prototypes.hpp"
#include "CrcSoft.hpp"

bool received = false;

using namespace proto::test;


TEST(RxContainerTest, CheckMatchers){
    EXPECT_EQ(rxContainer.Get<proto::FieldName::LEN_FIELD>().matcher_, proto::RxContainer<proto::test::proto_fields>::SetDataLen);
    EXPECT_EQ(rxContainer.Get<proto::FieldName::ALEN_FIELD>().matcher_, proto::RxContainer<proto::test::proto_fields>::CheckAlen);
    EXPECT_EQ(rxContainer.Get<proto::FieldName::CRC_FIELD>().matcher_, proto::RxContainer<proto::test::proto_fields>::CheckCrc);
}

TEST(RxContainerTest, CheckReset){

    //check if reset sets offsets to zero
    size_t packetSize =  GetTestPack(RxBufferTest, testType);
    rxContainer.Reset();
    Span<std::uint8_t> rxSpan(RxBufferTest, packetSize-1);
    size_t read = 0;
    rxContainer.Fill( rxSpan, read);
    EXPECT_EQ(rxContainer.Get<proto::FieldName::ID_FIELD>().GetOffset(), 0);
    EXPECT_EQ(rxContainer.Get<proto::FieldName::LEN_FIELD>().GetOffset(), 3);
    EXPECT_EQ(rxContainer.Get<proto::FieldName::ALEN_FIELD>().GetOffset(), 4);
    EXPECT_EQ(rxContainer.Get<proto::FieldName::DATA_FIELD>().GetOffset(), 5);

    rxContainer.Reset();
    rxContainer.for_each_type([&](auto& field){
        EXPECT_EQ(field.GetOffset(), 0);
    });
}



TEST(RxContainerTest, CheckOffsetsSameBuffer){

    received = false;
    TestRxContainer<proto::test::proto_fields>::Delegate receive_handler = [](auto& container) {
        received = true;
        EXPECT_EQ(memcmp( (uint8_t*)container.template Get<proto::FieldName::DATA_FIELD>().GetData(), (uint8_t*)&testType, sizeof(testType)), 0);
        size_t packetSize =  GetTestPack(RxBufferTest, testType);
        EXPECT_EQ(memcmp((uint8_t*)container.template Get<proto::FieldName::ID_FIELD>().GetData(),RxBufferTest, packetSize), 0);
    };
    auto receive_handler_ptr = std::make_shared<TestRxContainer<proto::test::proto_fields>::Delegate>(receive_handler);
    rxContainer.SetReceiveHandler(receive_handler_ptr);
    size_t offset = 0;
    rxContainer.Reset();

    // Статическая проверка с index_sequence
    auto checkRxContainerImpl = [](auto& container, auto seq) {
        size_t offset = 0;

        std::apply([&](auto... I) {
            (..., [&] {
                constexpr size_t Index = decltype(I)::value;
                auto& field = container.template Get<Index>();
                size_t read = 0;
                Span<std::uint8_t> rxSpan(RxBufferTest + offset, field.GetSize());
                container.Fill(rxSpan, read);
                offset += read;

//                if constexpr (Index + 1 < std::remove_reference_t<decltype(container)>::size) {
//                    auto& next = container.template Get<Index + 1>();
//                    EXPECT_EQ(next.GetOffset(), offset);
//                }
            }());
        }, seq);
    };

    size_t packetSize =  GetTestPack(RxBufferTest, testType);

    checkRxContainerImpl(rxContainer, std::make_tuple(
        std::integral_constant<size_t, 0>{},
        std::integral_constant<size_t, 1>{},
        std::integral_constant<size_t, 2>{},
        std::integral_constant<size_t, 3>{},
        std::integral_constant<size_t, 4>{}
    ));



//    //check if receiving process sets ofsets in the same buffers at right positions
//    for(int i = 0; i < rxContainer.size; i++){
//        size_t read = 0;
//        auto* field = rxContainer.Get<i>();
//        Span<std::uint8_t> rxSpan(RxBufferTest + field->GetOffset(), field->GetSize());
//        rxContainer.Fill( rxSpan, read);
//        offset+=read;
//        if(i < rxContainer.size - 1) {
//            field = rxContainer.Get<i + 1>;
//            EXPECT_EQ(field->GetOffset(), offset);
//        }
//    }
    EXPECT_TRUE(received);
}

TEST(RxContainerTest, CheckDebugOut){
    received = false;
    TestRxContainer<proto::test::proto_fields>::Delegate receive_handler = [](auto& container) {
        received = true;
    };
    auto receive_handler_ptr = std::make_shared<TestRxContainer<proto::test::proto_fields>::Delegate>(receive_handler);

    rxContainer.SetReceiveHandler(receive_handler_ptr);
    rxContainer.Reset();
    rxContainer.SetDebug(true);

    size_t read = 0;


    //check len
    GetTestPack(RxBufferTest, testType);
    Span<std::uint8_t> id(RxBufferTest , rxContainer.Get<proto::FieldName::ID_FIELD>().GetSize());
    Span<std::uint8_t> len(RxBufferTest  + id.size(), rxContainer.Get<proto::FieldName::LEN_FIELD>().GetSize());
    Span<std::uint8_t> alen(RxBufferTest  + id.size() + len.size(), rxContainer.Get<proto::FieldName::ALEN_FIELD>().GetSize());
    Span<std::uint8_t> data(RxBufferTest + id.size() + len.size() + alen.size(), rxContainer.Get<proto::FieldName::DATA_FIELD>().GetSize());
    Span<std::uint8_t> crc(RxBufferTest  + id.size() + len.size() +alen.size() + data.size(), rxContainer.Get<proto::FieldName::CRC_FIELD>().GetSize());

    rxContainer.Fill(id, read);
    len[0]+=1;
    rxContainer.Fill(len, read);

    GetTestPack(RxBufferTest, testType);
    rxContainer.Fill(id, read);
    rxContainer.Fill(len, read);
    alen[0]+=1;
    rxContainer.Fill(alen, read);

    GetTestPack(RxBufferTest, testType);
    rxContainer.Fill(id, read);
    rxContainer.Fill(len, read);
    rxContainer.Fill(alen, read);
    rxContainer.Fill(data, read);
    crc[0]+=1;
    rxContainer.Fill(crc, read);

    rxContainer.Reset();
}

TEST(RxContainerTest, CheckDebugOutComplex){
    received = false;
    TestRxContainer<proto::test::proto_fields2>::Delegate receive_handler = [](auto& container) {
        received = true;
    };

    auto receive_handler_ptr = std::make_shared<TestRxContainer<proto::test::proto_fields2>::Delegate>(receive_handler);

    rxContainer2.SetReceiveHandler(receive_handler_ptr);
    rxContainer2.Reset();
    rxContainer2.SetDebug(true);

    size_t read = 0;

    GetTestPack2(RxBufferTest, testType2);
    Span<std::uint8_t> id(RxBufferTest , rxContainer2.Get<proto::FieldName::ID_FIELD>().GetSize());
    Span<std::uint8_t> len(RxBufferTest  + id.size(), rxContainer2.Get<proto::FieldName::LEN_FIELD>().GetSize());
    Span<std::uint8_t> alen(RxBufferTest  + id.size() + len.size(), rxContainer2.Get<proto::FieldName::ALEN_FIELD>().GetSize());
    Span<std::uint8_t> type(RxBufferTest  + id.size() + len.size() + alen.size(), rxContainer2.Get<proto::FieldName::TYPE_FIELD>().GetSize());
    Span<std::uint8_t> data(RxBufferTest + id.size() + len.size() + alen.size() + type.size(), sizeof(testType2));
    Span<std::uint8_t> crc(RxBufferTest  + id.size() + len.size() +alen.size() + type.size() + data.size(), rxContainer2.Get<proto::FieldName::CRC_FIELD>().GetSize());

    GetTestPack2(RxBufferTest, testType2);
    rxContainer2.Fill(id, read);
    rxContainer2.Fill(len, read);
    alen[0]+=1;
    rxContainer2.Fill(alen, read);

    GetTestPack2(RxBufferTest, testType2);
    rxContainer2.Fill(id, read);
    rxContainer2.Fill(len, read);
    rxContainer2.Fill(alen, read);
    type[0] = 0;
    rxContainer2.Fill(type, read);


    GetTestPack2(RxBufferTest, testType2);
    rxContainer2.Fill(id, read);
    rxContainer2.Fill(len, read);
    rxContainer2.Fill(alen, read);
    rxContainer2.Fill(type, read);
    rxContainer2.Fill(data, read);
    crc[0]+=1;
    rxContainer2.Fill(crc, read);

    rxContainer2.Reset();
}

//TEST(RxContainerTest, CheckOffsetsDifferentBuffer){
//    received = false;
//    TestRxContainer<proto::test::proto_fields2>::delegate receive_handler = [](auto& container) {
//        // Set the received flag to true to indicate that the data has been received
//        received = true;
//        // Check if the data received matches the expected data
//        EXPECT_EQ(memcmp( (uint8_t*)container.template Get<proto::FieldName::DATA_FIELD>().Value(), (uint8_t*)&testType2, sizeof(testType2)), 0);
//        size_t packetSize =  GetTestPack2(RxBufferTest, testType2);
//        EXPECT_NE(memcmp(container.template Get<proto::FieldName::ID_FIELD>().Value(),RxBufferTest, packetSize), 0);
//        // Check if the data received matches the expected data in each field
//        size_t offset = 0;
//        for(int i = 0; i < rxContainer2.size; i++){
//            EXPECT_EQ( memcmp( rxContainer2[i]->GetRaw(), RxBufferTest + offset, rxContainer2[i]->GetSize()), 0);
//            offset += rxContainer2[i]->GetSize();
//        }
//        auto type = container.template Get<proto::FieldName::DATA_FIELD>().GetData();
//        auto opt_variant = container.template Get<proto::FieldName::DATA_FIELD>().GetData();
//        EXPECT_TRUE(opt_variant.has_value());
//        std::visit([](auto&& pkt) {
//            std::cout << "Visiting variant" << std::endl;
//            using T = std::decay_t<decltype(pkt)>;
//            if constexpr (not std::is_same_v<T, proto::test::dataType2>) {
//                EXPECT_TRUE(false);
//            }
//        }, *opt_variant); // передаём variant по значению или ссылке
//    };
//    rxContainer2.SetReceiveHandler(receive_handler);
//    size_t packetSize =  GetTestPack2(RxBufferTest, testType2);
//    size_t offset = 0;
//    rxContainer2.Reset();
//    //check if receiving process sets ofsets in the same buffers at right positions
//    for(int i = 0; i < rxContainer2.size; i++){
//        size_t read = 0;
//        Span<std::uint8_t> rxSpan(RxBufferTest + offset, rxContainer2[i]->GetSize());
//        EXPECT_EQ(proto::MatchStatus::MATCH, rxContainer2.Fill( rxSpan, read));
//        offset+=read;
//        EXPECT_EQ(rxContainer2[i]->GetOffset(), 0);
//    }
//    EXPECT_EQ(offset, packetSize );
//    EXPECT_TRUE(received);
//}

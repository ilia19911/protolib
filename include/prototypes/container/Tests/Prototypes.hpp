#pragma once

#include "Field.hpp"
#include "FieldContainer.hpp"
#include "RxContainer.hpp"
#include "TxContainer.hpp"
#include "FieldPrototypes.hpp"


template<typename Fields>
class TestRxContainer : public proto::RxContainer<Fields> {
public:
//    void SetOffset
    void Reset() {
        proto::FieldContainer<Fields>::Reset();
    }
};

template<typename Fields>
class TestTxContainer : public proto::TxContainer<Fields> {

};

extern TestRxContainer<proto::test::proto_fields> rxContainer;
extern TestTxContainer<proto::test::proto_fields> txContainer;
extern TestRxContainer<proto::test::proto_fields2> rxContainer2;
extern TestTxContainer<proto::test::proto_fields2> txContainer2;



extern uint8_t TxBufferTest[100];
extern uint8_t RxBufferTest[100];
extern proto::test::dataType testType;
extern proto::test::dataType2 testType2;
extern proto::test::dataType3 testType3;

extern size_t GetTestPack(uint8_t* ptr, proto::test::dataType& obj);
extern size_t GetTestPack2(uint8_t* ptr, proto::test::dataType2& obj);

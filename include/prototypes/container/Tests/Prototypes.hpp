#pragma once

#include "FieldPrototype.hpp"
#include "FieldContainer.hpp"
#include "RxContainer.hpp"
#include "TxContainer.hpp"
#include "TestFieldPrototypes.hpp"
#include "TestFields.hpp"

namespace proto::test{


    template<uint8_t *RX_BASE, uint8_t *TX_BASE>
    class SympleProtocol{
    public:
        // Public aliases to use in tests (and elsewhere)
        using RxFields      = typename SympleFields<RX_BASE>::proto_fields;
        using TxFields      = typename SympleFields<TX_BASE>::proto_fields;
        using RxContainerT  = proto::RxContainer<RxFields>;
        using TxContainerT  = proto::TxContainer<TxFields>;

        // Accessors (non-const/const) to reach the underlying containers in tests
        RxContainerT& rx() { return rxContainer_; }
        TxContainerT& tx() { return txContainer_; }
        const RxContainerT& rx() const { return rxContainer_; }
        const TxContainerT& tx() const { return txContainer_; }

    private:
        proto::RxContainer<typename SympleFields<RX_BASE>::proto_fields> rxContainer_{};
        proto::TxContainer<typename SympleFields<TX_BASE>::proto_fields> txContainer_{};
    };

    template<uint8_t *RX_BASE, uint8_t *TX_BASE>
    class ComplexProtocol{
    public:
        // Public aliases to use in tests (and elsewhere)
        using RxFields      = typename ComplexFields<RX_BASE>::proto_fields;
        using TxFields      = typename ComplexFields<TX_BASE>::proto_fields;
        using RxContainerT  = proto::RxContainer<RxFields>;
        using TxContainerT  = proto::TxContainer<TxFields>;

        // Accessors (non-const/const) to reach the underlying containers in tests
        RxContainerT& rx() { return rxContainer_; }
        TxContainerT& tx() { return txContainer_; }
        const RxContainerT& rx() const { return rxContainer_; }
        const TxContainerT& tx() const { return txContainer_; }

    private:
        proto::RxContainer<typename ComplexFields<RX_BASE>::proto_fields> rxContainer_{};
        proto::TxContainer<typename ComplexFields<TX_BASE>::proto_fields> txContainer_{};
    };
}

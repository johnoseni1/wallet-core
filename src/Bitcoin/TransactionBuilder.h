// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "Transaction.h"
#include "TransactionPlan.h"
#include "UnspentSelector.h"
#include "../proto/Bitcoin.pb.h"
#include <TrustWalletCore/TWCoinType.h>

#include <algorithm>

namespace TW::Bitcoin {

class TransactionBuilder {
public:
    /// Plans a transaction by selecting UTXOs and calculating fees.
    static TransactionPlan plan(const SigningInput& input);

    /// Builds a transaction by selecting UTXOs and calculating fees.
    template <typename Transaction>
    static Transaction build(const TransactionPlan& plan, const std::string& toAddress,
                             const std::string& changeAddress, enum TWCoinType coin) {
        auto lockingScriptTo = Script::lockScriptForAddress(toAddress, coin);
        if (lockingScriptTo.empty()) {
            return {};
        }

        Transaction tx;
        tx.outputs.push_back(TransactionOutput(plan.amount, lockingScriptTo));

        if (plan.change > 0) {
            auto lockingScriptChange = Script::lockScriptForAddress(changeAddress, coin);
            tx.outputs.push_back(TransactionOutput(plan.change, lockingScriptChange));
        }

        const auto emptyScript = Script();
        for (auto& utxo : plan.utxos) {
            tx.inputs.add(TransactionInput(utxo.outPoint, emptyScript, utxo.outPoint.sequence));
        }

        return tx;
    }
};

} // namespace TW::Bitcoin

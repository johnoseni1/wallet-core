// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "Data.h"
#include "DerivationPath.h"
#include "Hash.h"
#include "PrivateKey.h"
#include "PublicKey.h"

#include <TrustWalletCore/TWCoinType.h>
#include <TrustWalletCore/TWCurve.h>
#include <TrustWalletCore/TWHDVersion.h>
#include <TrustWalletCore/TWPurpose.h>

#include <array>
#include <optional>
#include <string>

namespace TW {

class HDWallet {
  public:
    static constexpr size_t seedSize = 64;
    static constexpr size_t maxMnemomincSize = 240;
    static constexpr size_t maxExtendedKeySize = 128;

  public:
    /// Wallet seed, derived one-way from the mnemonic and password
    std::array<byte, seedSize> seed;

    /// Mnemonic word list (aka. recovery phrase).
    std::string mnemonic;

    /// Password for mnemonic encryption.
    std::string password;

    /// Entropy is the binary 1-to-1 representation of the mnemonic (11 bits from each word)
    TW::Data entropy;

  public:
    /// Initializes a new random HDWallet with the provided strength in bits.  Throws on invalid strength.
    HDWallet(int strength, const std::string& password);

    /// Initializes an HDWallet from a mnemonic.  Throws on invalid mnemonic.
    HDWallet(const std::string& mnemonic, const std::string& password);

    /// Initializes an HDWallet from an entropy.  Throws on invalid data.
    HDWallet(const Data& entropy, const std::string& password);

    HDWallet(const HDWallet& other) = default;
    HDWallet(HDWallet&& other) = default;
    HDWallet& operator=(const HDWallet& other) = default;
    HDWallet& operator=(HDWallet&& other) = default;

    virtual ~HDWallet();

    void updateSeedAndEntropy();

    /// Returns master key.
    PrivateKey getMasterKey(TWCurve curve) const;

    /// Returns the master private key extension (32 byte).
    PrivateKey getMasterKeyExtension(TWCurve curve) const;

    /// Returns the private key at the given derivation path.
    PrivateKey getKey(const TWCoinType coin, const DerivationPath& derivationPath) const;

    /// Derives the address for a coin.
    std::string deriveAddress(TWCoinType coin) const;

    /// Returns the extended private key.
    std::string getExtendedPrivateKey(TWPurpose purpose, TWCoinType coin, TWHDVersion version) const;

    /// Returns the exteded public key.
    std::string getExtendedPublicKey(TWPurpose purpose, TWCoinType coin, TWHDVersion version) const;

    /// Computes the public key from an exteded public key representation.
    static std::optional<PublicKey> getPublicKeyFromExtended(const std::string& extended, TWCoinType coin, const DerivationPath& path);

    /// Computes the private key from an exteded private key representation.
    static std::optional<PrivateKey> getPrivateKeyFromExtended(const std::string& extended, TWCoinType coin, const DerivationPath& path);

  public:
    // Private key type (later could be moved out of HDWallet)
    enum PrivateKeyType {
      PrivateKeyTypeDefault32 = 0, // 32-byte private key
      PrivateKeyTypeExtended96 = 1, // 3*32-byte extended private key
    };
    
    // obtain privateKeyType used by the coin/curve
    static PrivateKeyType getPrivateKeyType(TWCurve curve);
};

} // namespace TW

/// Wrapper for C interface.
struct TWHDWallet {
    TW::HDWallet impl;
};

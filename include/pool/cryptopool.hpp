// Abdulkadir U. - 2026/01/26
#pragma once

/**
 * Crypto Pool (Kripto Havuzu)
 * 
 * Şifreleme yöntemlerini tek bir dosyada tutmayı
 * sağlıyoruz bu sayede karmaşıklık daha az oluyor
 */

#include <core/algorithm.hpp>

#include <crypto/crypto.hpp>
#include <crypto/xor/xor.hpp>

// Namespace
namespace pool::cryptopool
{
    // Using Namespace
    using namespace core::algorithm;
    using namespace crypto;
    using namespace crypto::stream;
}
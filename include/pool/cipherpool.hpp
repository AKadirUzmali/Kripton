// Abdulkadir U. - 2026/01/26
#pragma once

/**
 * Cipher Pool (Şifre Havuzu)
 * 
 * Şifreleme yöntemlerini tek bir dosyada tutmayı
 * sağlıyoruz bu sayede karmaşıklık daha az oluyor
 */

#include <core/algorithm.hpp>

#include <cipher/xor/xor.hpp>

// Namespace
namespace pool::cipherpool
{
    // Using Namespace
    using namespace core::algorithm;
    using namespace cipher::stream;
}
// Abdulkadir U. - 2026/01/26
#pragma once

/**
 * Cipher Pool (Şifre Havuzu)
 * 
 * Şifreleme yöntemlerini tek bir dosyada tutmayı
 * sağlıyoruz bu sayede karmaşıklık daha az oluyor
 */

#include <unordered_map>

#include <core/algorithm.hpp>
#include <cipher/xor/xor.hpp>

// Namespace
namespace pool::cipherpool
{
    // Using Namespace
    using namespace core::algorithm;
    using namespace cipher::stream;

    // Cipher Type Enum
    enum class ecipher_t
    {
        Null = 0,
        Xor
    };

    // Cipher Name Type
    using cipher_name_t = std::string;

    // Cipher Information
    struct CipherInfo
    {
        ecipher_t m_type;
        cipher_name_t m_name;
    };

    // Cipher Name List
    static const std::array<CipherInfo, 1> st_cipher_list
    {
        { ecipher_t::Xor, "Xor" }
    };
}
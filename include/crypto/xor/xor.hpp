// Abdulkadir U. - 2026/01/26
#pragma once

/**
 * Xor
 * 
 * Şifreleme yöntemlerinden en bilindiklerinden biri olan
 * Xor şifreleme şu şekilde çalışmaktadır:
 * 
 * Örnek:
 *  -> 1 ^ 0 = 1
 *  -> 0 ^ 1 = 1
 *  -> 1 ^ 1 = 0
 *  -> 0 ^ 0 = 0
 */

// Include
#include <core/algorithm.hpp>

// Namespace
namespace crypto::stream
{
    // Using Namespace
    using namespace core::algorithm;

    // Class
    class Xor final : public Algorithm
    {
        public:
            explicit Xor(
                const std::string& ar_name,
                const std::u32string& ar_key
            );

            bool encrypt(std::string& ar_text) noexcept override;
            bool decrypt(std::string& ar_text) noexcept override;
    };

    /**
     * @brief Xor
     * 
     * Sınıfın kurucu yapısıdır. Bir isim ve
     * anahtar alır. Ek bir şeye ihtiyaç duymadan
     * ana kurucu olan Algorithm'i kullanır.
     * 
     * @param string& Name
     * @param u32string& Key
     */
    Xor::Xor(
        const std::string& ar_name,
        const std::u32string& ar_key
    )
    : Algorithm(ar_name, ar_key)
    {}

    /**
     * @brief Encrypt
     * 
     * Xor şifreleme işlemini yapar.
     * Verilen metini alır ve şifreleme anahtarı
     * ile karakter karakter şifreler. Eğer
     * işlem başarılı ise true, değilse false
     * dönerek işlemin başarıyla yapılıp yapılmadığını
     * bildirir
     * 
     * @param string& Text
     * @return bool
     */
    bool Xor::encrypt(
        std::string& ar_text
    ) noexcept
    {
        if( this->has_error() )
            return false;

        const std::string& tm_key = this->get_key();    
        const uint16_t tm_keylen = tm_key.size();

        for( uint16_t tm_count = 0; tm_count < ar_text.size(); ++tm_count)
            ar_text[tm_count] ^= tm_key[tm_count & tm_keylen];

        return true;
    }

    /**
     * @brief Decrypt
     * 
     * Xor şifre çözme işlemini yapar ama
     * şifreleme ile aynı şekilde çalıştığından
     * direk şifreleme fonksiyonunu kullanır
     * 
     * @param string& Text
     * @return bool
     */
    bool Xor::decrypt(
        std::string& ar_text
    ) noexcept
    {
        return this->encrypt(ar_text);
    }
}
// Abdulkadir U. - 28/10/2025
#pragma once

/**
 * Algorithm Xor (Xor Algoritması)
 * 
 * Xor algoritması veriyi karakter karakter
 * ^ ile ve anahtara göre şifreler ya da çözer.
 */

// Include:
#include <Algorithm/Algorithm.h>
using namespace core::virbase;

// Namespace: Core::Crypt::Algorithm
namespace core::crypt::algorithm
{
    // Class: Xor
    class Xor : public virtual Algorithm
    {
        public:
            static inline const std::string NAME = "Xor";

            explicit Xor(const std::u32string&);

            virtual bool encrypt(std::u32string&) noexcept override;
            virtual bool decrypt(std::u32string&) noexcept override;
    };

    /**
     * @brief [Public] Constructor
     * 
     * Xor şifreleme/çözme işlemleri için
     * gerekli anahtarı alsın ve normal algoritma
     * sınıf oluşturmasını tamamlasın
     * 
     * @param u32string& Key
     */
    Xor::Xor(const std::u32string& _key)
    : Algorithm(Xor::NAME, _key)
    {}

    /**
     * @brief [Public] Encrypt
     * 
     * Xor şifreleme de anahtar alınır ve
     * her turda anahtar ile şifreleme yapılır
     * 
     * @param u32string& Text
     */
    bool Xor::encrypt(std::u32string& _text) noexcept
    {
        // anahtar bilgisini geçici olarak tutsun
        const std::u32string& tmp_key = this->getKey();
        const size_t tmp_key_len = tmp_key.length();

        // geçerli anahtar veya metin uzunluğu yok
        if( !tmp_key_len || !_text.length() )
            return false;

        // anahtarı döndürerek kullansın
        for( size_t counter = 0; counter < _text.length(); ++counter ) {
            _text.at(counter) ^= tmp_key.at(counter % tmp_key_len);
        }

        return true;
    }

    /**
     * @brief [Public] Decrypt
     * 
     * Xor şifreleme ile aynı işlemi yapar
     * fakat bu sefer tam aksine şifre çözer
     * 
     * @param u32string& Text
     */
    bool Xor::decrypt(std::u32string& _text) noexcept
    {
        // anahtar bilgisini geçici olarak tutsun
        const std::u32string& tmp_key = this->getKey();
        const size_t tmp_key_len = tmp_key.length();

        // geçerli anahtar veya metin uzunluğu yok
        if( !tmp_key_len || !_text.length() )
            return false;

        // anahtarı döndürerek kullansın
        for( size_t counter = 0; counter < _text.length(); ++counter ) {
            _text.at(counter) ^= tmp_key.at(counter % tmp_key_len);
        }

        return true;
    }
}
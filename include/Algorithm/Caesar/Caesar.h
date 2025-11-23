// Abdulkadir U. - 31/10/2025
#pragma once

/**
 * Algorithm Caesar (Caesar Algoritması)
 * 
 * Caesar algoritması veriyi ileri ya da geri
 * kaydırarak şifreleme ya da şifre çözme yaparız
 */

// Include:
#include <Algorithm/Algorithm.h>
using namespace core::virbase;

// Namespace: Core::Crypt::Algorithm
namespace core::crypt::algorithm
{
    // Class: Xor
    class Caesar : public virtual Algorithm
    {
        public:
            static inline const std::string NAME = "Caesar";

            explicit Caesar(const std::u32string&);

            virtual bool encrypt(std::u32string&) noexcept override;
            virtual bool decrypt(std::u32string&) noexcept override;
    };

    /**
     * @brief [Public] Constructor
     * 
     * Caesar şifreleme/çözme işlemleri için
     * gerekli anahtarı alsın ve normal algoritma
     * sınıf oluşturmasını tamamlasın
     * 
     * @param u32string& Key
     */
    Caesar::Caesar(const std::u32string& _key)
    : Algorithm(Caesar::NAME, _key)
    {}

    /**
     * @brief [Public] Encrypt
     * 
     * Caesar şifreleme de anahtar alınır ve
     * her turda anahtar ile şifreleme yapılır
     * 
     * @param u32string& Text
     */
    bool Caesar::encrypt(std::u32string& _text) noexcept
    {
        // anahtar bilgisini geçici olarak tutsun
        const std::u32string& tmp_key = this->getKey();
        const size_t tmp_key_len = tmp_key.length();

        // geçerli anahtar veya metin uzunluğu yok
        if( !tmp_key_len || !_text.length() )
            return false;

        // anahtarı döndürerek kullansın
        for( size_t counter = 0; counter < _text.length(); ++counter ) {
            _text.at(counter) += tmp_key.at(counter % tmp_key_len);
        }

        return true;
    }

    /**
     * @brief [Public] Decrypt
     * 
     * Caesar şifreleme ile aynı işlemi yapar
     * fakat bu sefer tam aksine şifre çözer
     * 
     * @param u32string& Text
     */
    bool Caesar::decrypt(std::u32string& _text) noexcept
    {
        // anahtar bilgisini geçici olarak tutsun
        const std::u32string& tmp_key = this->getKey();
        const size_t tmp_key_len = tmp_key.length();

        // geçerli anahtar veya metin uzunluğu yok
        if( !tmp_key_len || !_text.length() )
            return false;

        // anahtarı döndürerek kullansın
        for( size_t counter = 0; counter < _text.length(); ++counter ) {
            _text.at(counter) -= tmp_key.at(counter % tmp_key_len);
        }

        return true;
    }
}
// Abdulkadir U. - 28/10/2025

// Include:
#include <string>

#include <Algorithm/Xor/Xor.h>
using namespace core::crypt::algorithm;

/**
 * @brief [Public] Constructor
 * 
 * Xor şifreleme/çözme işlemleri için
 * gerekli anahtarı alsın ve normal algoritma
 * sınıf oluşturmasını tamamlasın
 * 
 * @param u32string Key
 */
Xor::Xor(const std::u32string& _key)
: algo::Algorithm(Xor::NAME, _key)
{}

/**
 * @brief [Public] Encrypt
 * 
 * Xor şifreleme de anahtar alınır ve
 * her turda anahtar ile şifreleme yapılır
 * 
 * @param u32string Text
 */
void Xor::encrypt(std::u32string& _text) noexcept
{
    // anahtar bilgisini geçici olarak tutsun
    const std::u32string& tmp_key = this->getKey();
    const size_t tmp_key_len = tmp_key.length();
    
    // geçerli anahtar uzunluğu yok
    if( !tmp_key_len ) return;

    // anahtarı döndürerek kullansın
    for( size_t counter = 0; counter < _text.length(); ++counter ) {
        _text.at(counter) ^= tmp_key.at(counter % tmp_key_len);
    }
}

/**
 * @brief [Public] Decrypt
 * 
 * Xor şifreleme ile aynı işlemi yapar
 * fakat bu sefer tam aksine şifre çözer
 * 
 * @param u32string Text
 */
void Xor::decrypt(std::u32string& _text) noexcept
{
    // anahtar bilgisini geçici olarak tutsun
    const std::u32string& tmp_key = this->getKey();
    const size_t tmp_key_len = tmp_key.length();
    
    // geçerli anahtar uzunluğu yok
    if( !tmp_key_len ) return;

    // anahtarı döndürerek kullansın
    for( size_t counter = 0; counter < _text.length(); ++counter ) {
        _text.at(counter) ^= tmp_key.at(counter % tmp_key_len);
    }
}
// Abdulkadir U. - 28/10/2025
#pragma once

/**
 * Algorithm (Algoritma)
 * 
 * Kriptoloji de birçok algoritma olabileceği için
 * bu algoritmaların barınabileceği sanal bir sınıf.
 * Şifreleme yöntemi kendi algoritma yöntemine göre
 * veriyi şifreleyecek veya çözecek.
 */

// Include:
#include <cstdint>
#include <string>

#include <Flag/Flag.h>
using namespace core::flag;

// Namespace: Core::VirtualBase
namespace core::virbase
{
    // Namespace: Algorithm
    namespace algorithm
    {
        // Enum Class: Algorithm Code
        enum class e_algorithm : size_t
        {
            unknown = 0,
            error,
            warning,
            success,

            err_key_length_invalid = 1000,
            err_name_length_invalid,
            err_flag_notchanged,

            succ_flag_default = 2000,
            succ_flag_changed
        };

        // Flag: Algorithm Status
        inline constexpr flag_t flag_free           { 0 << 0 };
        inline constexpr flag_t flag_notfree        { 1 << 0 };
        inline constexpr flag_t flag_valid_name     { 0 << 1 };
        inline constexpr flag_t flag_valid_key      { 0 << 2 };
        inline constexpr flag_t flag_err_name       { 1 << 1 };
        inline constexpr flag_t flag_err_key        { 1 << 2 };
        inline constexpr flag_t flag_decrypt        { 0 << 3 };
        inline constexpr flag_t flag_encrypt        { 1 << 3 };
        inline constexpr flag_t flag_idle           { 0 << 4 };
        inline constexpr flag_t flag_active         { 1 << 4 };
    }

    // Class: Algorithm
    class Algorithm
    {
        private:
            std::string name;
            std::u32string key;

            Flag flag;

            virtual void setIdle()      noexcept;
            virtual void setActive()    noexcept;
            virtual void setEncrypt()   noexcept;
            virtual void setDecrypt()   noexcept;

        public:
            static inline constexpr size_t MIN_NAME_SIZE = 1;
            static inline constexpr size_t MAX_NAME_SIZE = 32;

            static inline constexpr size_t MIN_KEY_SIZE = 1;
            static inline constexpr size_t MAX_KEY_SIZE = 128;

            explicit Algorithm(const std::string&, const std::u32string&) noexcept;
            ~Algorithm() noexcept;

            virtual bool hasError()     const noexcept;
            virtual bool hasName()      const noexcept;
            virtual bool hasKey()       const noexcept;

            virtual bool isEncrypt()    const noexcept;
            virtual bool isDecrypt()    const noexcept;
            virtual bool isIdle()       const noexcept;
            virtual bool isActive()     const noexcept;
            
            virtual const std::string& getName()    const noexcept;
            virtual const std::u32string& getKey()  const noexcept;

            virtual algorithm::e_algorithm setName(const std::string&)     noexcept;
            virtual algorithm::e_algorithm setKey(const std::u32string&)   noexcept;

            inline bool encrypt(std::u32string&) noexcept;
            inline bool decrypt(std::u32string&) noexcept;

            static bool checkValidName(const std::string&)     noexcept;
            static bool checkValidKey(const std::u32string&)   noexcept;

        protected:
            virtual bool doEncrypt(std::u32string&) noexcept { return false; };
            virtual bool doDecrypt(std::u32string&) noexcept { return false; };
    };

    using namespace algorithm;

    /**
     * @brief [Public] Encrypt
     * 
     * Şifreleme yapmayı sağlayacak fonksiyon
     * ve güvenlik kontrollü olabilmesi için
     * ana sınıf içinde değiştirilemez olacak
     * ve güvenlik kısmından sonra asıl şifreleme
     * çalışması için ayrılmış olan fonksiyon çalışacak
     * 
     * @param u32string& Text
     * @return bool
     */
    bool Algorithm::encrypt(std::u32string& _text) noexcept
    {
        if( !Algorithm::checkValidName(this->getName()) )
            this->flag.change(flag_valid_name, flag_err_name);

        if( !Algorithm::checkValidKey(this->getKey()) )
            this->flag.change(flag_valid_key, flag_err_key);

        this->flag.change(flag_decrypt, flag_encrypt);

        return this->hasError() ?
            false :
            this->doEncrypt(_text) && !this->hasError();
    }

    /**
     * @brief [Public] Decrypt
     * 
     * Şifre çözmeyi sağlayacak fonksiyon
     * ve güvenlik kontrollü olabilmesi için
     * ana sınıf içinde değiştirilemez olacak
     * ve güvenlik kısmından sonra asıl şifre çözücünün
     * çalışması için ayrılmış olan fonksiyon çalışacak
     * 
     * @param u32string& Text
     * @return bool
     */
    bool Algorithm::decrypt(std::u32string& _text) noexcept
    {
        if( !Algorithm::checkValidName(this->getName()) )
            this->flag.change(flag_valid_name, flag_err_name);

        if( !Algorithm::checkValidKey(this->getKey()) )
            this->flag.change(flag_valid_key, flag_err_key);

        this->flag.change(flag_encrypt, flag_decrypt);

        return this->hasError() ?
            false :
            this->doDecrypt(_text) && !this->hasError();
    }
}
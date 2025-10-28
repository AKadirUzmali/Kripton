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
#include <string>
#include <stdexcept>

// Namespace: Core::VirtualBase
namespace core::virbase
{
    // Enum Class: Algorithm Code
    enum class e_algorithm : size_t
    {
        UNKNOWN                         = 0,
        ERROR,
        WARNING,
        SUCCESS,

        ERR_KEY_LENGTH_INVALID          = 1000,
        ERR_NAME_LENGTH_INVALID
    };

    // Class: Algorithm
    class Algorithm
    {
        private:
            std::string name;
            std::u32string key;

        public:
            static inline constexpr size_t MIN_NAME_SIZE = 1;
            static inline constexpr size_t MAX_NAME_SIZE = 32;

            static inline constexpr size_t MIN_KEY_SIZE = 1;
            static inline constexpr size_t MAX_KEY_SIZE = 128;

            explicit Algorithm(const std::string&, const std::u32string&);
            
            static bool isValidName(const std::string&) noexcept;
            virtual const std::string& getName() const noexcept;
            virtual e_algorithm setName(const std::string&) noexcept;
            
            static bool isValidKey(const std::u32string&) noexcept;
            virtual const std::u32string& getKey() const noexcept;
            virtual e_algorithm setKey(const std::u32string&) noexcept;

            virtual void encrypt(std::u32string&) noexcept {};
            virtual void decrypt(std::u32string&) noexcept {};            
    };
}
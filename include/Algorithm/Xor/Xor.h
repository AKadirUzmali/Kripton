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

            virtual void encrypt(std::u32string&) noexcept override;
            virtual void decrypt(std::u32string&) noexcept override;
    };
}
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

        protected:
            virtual bool doEncrypt(std::u32string&) noexcept override;
            virtual bool doDecrypt(std::u32string&) noexcept override;
    };
}
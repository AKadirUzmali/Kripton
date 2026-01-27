// Abdulkadir U. - 2026/01/26
#pragma once

/**
 * Crypto (Kripto)
 * 
 * Algoritmayı kullanan şifreleme yöntemleri
 * tek tek ayrışıp uyumlulukda sorun yaşamamak ve
 * diğerleri ile uyumlu çalışabilecek tek yapı yapmak adına
 * bunların olduğu genel bir yapı tasarlıyoruz
 */

// Include
#include <type_traits>
#include <utility>
#include <string>

#include <core/algorithm.hpp>

// Namespace
namespace crypto::stream
{
    // Using Namespace
    using namespace core::algorithm;

    // Class
    template<typename Algo>
    class Crypto final
    {
        static_assert(std::is_base_of<Algorithm, Algo>::value, "Crypto<T>: T must derive from Algorithm");

        private:
            Algo m_algorithm;

        public:
            template<typename... Args>
            explicit Crypto(
                Args&&... ar_args
            )
            noexcept ( std::is_nothrow_constructible<Algo, Args...>::value)
                : m_algorithm(std::forward<Args>(ar_args)...)
            {}

            bool encrypt(std::string& ar_text) noexcept {
                return this->m_algorithm.encrypt(ar_text);
            }

            bool decrypt(std::string& ar_text) noexcept {
                return this->m_algorithm.decrypt(ar_text);
            }

            Algo& algorithm() noexcept {
                return this->m_algorithm;
            }

            const Algo& algorithm() const noexcept {
                return this->m_algorithm;
            }
    };
}
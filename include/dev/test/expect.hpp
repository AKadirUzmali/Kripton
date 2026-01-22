// Abdulkadir U. - 12/01/2026
#pragma once

/**
 * Expect
 * 
 * Test yaparak beklenen eşitliğin olup olmadığına
 * göre sonuç dönerek test aşamasını kolaylaştırır
 */

// Include
#include <dev/config/config.hpp>

// Namespace
namespace dev::test
{
    /**
     * @brief Expect Equal
     * 
     * İki adet bilinmeyen tür değişkeni alınır ve
     * karşılaştırılır. Eğer geliştirici modunda ise
     * sonuç döner, aksi halde hep başarı döner çünkü
     * test modu kapalıdır.
     * 
     * @note Sadece geliştirici modu için
     * 
     * @tparam L Left
     * @tparam R Right
     * 
     * @return bool
     */
    template<typename L, typename R>
    inline bool expect_eq(const L& l, const R& r)
    {
        if constexpr (dev::config::test)
            return l == r;
        else
            return true;
    }
}
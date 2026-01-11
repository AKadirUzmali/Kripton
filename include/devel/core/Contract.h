// Abdulkadir U. - 11/01/2026
#pragma once

/**
 * Contract
 * 
 * Geliştirici modu aktifliği durumunda
 * gerekli olanları işleme alabilmek için
 * gereken bir sözleşme yapısı
 */

// Include:
#include <devel/config/DevelFeatures.h>

// Namespace:
namespace devel::contract
{
    /**
     * @brief Require
     * 
     * Geliştirici modu aktif ise eğer
     * fonksiyonu çalıştırmayı sağlar
     * 
     * @tparam Fn 
     * @param fn 
     */
    template<typename Fn>
    constexpr void require(Fn&& fn)
    {
        if constexpr (devel::enabled) {
            fn();
        }
    }
}
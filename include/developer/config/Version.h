// Abdulkadir U. - 12/01/2026
#pragma once

/**
 * Version
 * 
 * Derleme sürümünü belirtmek için
 * geçerli bir sürüm belirleyici
 * 
 * Major = Büyük Yenilik
 * Minor = Küçük Yenilikler
 * Patch = Hata Düzeltme
 */

// Include:
#include <cstdint>

// Namespace:
namespace dev::version
{
    // Define:
    #define __VERSION_MAJOR__ 0
    #define __VERSION_MINOR__ 0
    #define __VERSION_PATCH__ 0

    // Struct:
    struct Version
    {
        uint8_t major;
        uint8_t minor;
        uint16_t patch;
    };

    inline constexpr Version app {
        .major = __VERSION_MAJOR__,
        .minor = __VERSION_MINOR__,
        .patch = __VERSION_PATCH__
    };
}
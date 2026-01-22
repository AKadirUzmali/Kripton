// Abdulkadir U. - 21/01/2026
#pragma once

/**
 * Status (Durum)
 * 
 * Programda birden çok yapı çalışmakta ve bunlar işlem yaparken
 * belirli bir cevap döndürmekte. Döndürülen cevabı yorumlayabilmek
 * için oluşturulan durum sınıfı yapısı bu ihtiyacı gidermektedir.
 */

// Include
#include <cstdint>

// Namespace
namespace core::status
{
    // Enum
    enum class status_kind : std::uint8_t
    {
        none = 0,
        ok,
        err,
        info,
        warn
    };
    
    // Struct
    struct status_t
    {
        status_kind kind { status_kind::none };
        std::uint8_t domain { 0 };
        std::uint16_t code { 0 };
    
        [[maybe_unused]] [[nodiscard]] constexpr bool none()   const noexcept { return kind == status_kind::none; }
        [[maybe_unused]] [[nodiscard]] constexpr bool ok()     const noexcept { return kind == status_kind::ok; }
        [[maybe_unused]] [[nodiscard]] constexpr bool err()    const noexcept { return kind == status_kind::err; }
        [[maybe_unused]] [[nodiscard]] constexpr bool warn()   const noexcept { return kind == status_kind::warn; }
        [[maybe_unused]] [[nodiscard]] constexpr bool info()   const noexcept { return kind == status_kind::info; }
    };
}
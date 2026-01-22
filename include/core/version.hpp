// Abdulkadir U. - 21/01/2026
#pragma once

/**
 * Version (Sürüm)
 * 
 * Derleme sürümünü belirtmek için gerekli
 * sürüm kayıt kontrolü sistemi
 * 
 * Major = Büyük Yenilik
 * Minor = Küçük Yenilikler
 * Patch = Hata Düzeltme
 * compatible = Aynı Sürüm Zorunlu mu Değil mi?
 */

// Include
#include <cstdint>

// Namespace
namespace core::version
{
    // Struct
    struct version_t
    {
        private:
            std::uint16_t m_major;
            std::uint16_t m_minor;
            std::uint32_t m_patch : 31;
            bool m_compatible  : 1;

        public:
            constexpr explicit version_t(
                const std::uint16_t ar_major,
                const std::uint16_t ar_minor,
                const std::uint32_t ar_patch,
                const bool ar_compatible
            ) noexcept
            : m_major(ar_major), m_minor(ar_minor), m_patch(ar_patch), m_compatible(ar_compatible)
            {}

            [[maybe_unused]] [[nodiscard]]
            constexpr bool operator==(const version_t& other) const noexcept
            {
                if( m_major != other.m_major ) return false;
                if( m_minor != other.m_minor ) return false;
                if( m_patch != other.m_patch ) return false;

                if( m_compatible )
                    return m_compatible == other.m_compatible;
                return true;
            }

            [[maybe_unused]] [[nodiscard]]
            constexpr bool operator!=(const version_t& other) const noexcept 
            { return !(*this == other); }
    };
}
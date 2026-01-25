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
    struct Version
    {
        private:
            std::uint16_t m_major;
            std::uint16_t m_minor;
            std::uint32_t m_patch;

        public:
            constexpr explicit Version(
                const std::uint16_t ar_major,
                const std::uint16_t ar_minor,
                const std::uint32_t ar_patch
            ) noexcept
            : m_major(ar_major), m_minor(ar_minor), m_patch(ar_patch)
            {}

            [[maybe_unused]] [[nodiscard]]
            constexpr bool operator==(const Version& other) const noexcept
            {
                return
                    this->m_major == other.m_major &&
                    this->m_minor == other.m_minor &&
                    this->m_patch == other.m_patch;
            }

            [[maybe_unused]] [[nodiscard]]
            constexpr bool operator!=(const Version& other) const noexcept 
            { return !(*this == other); }
    };
}
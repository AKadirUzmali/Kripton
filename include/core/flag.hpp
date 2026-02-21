// Abdulkadir U. - 21/01/2026
#pragma once

/**
 * Flag (Bayrak)
 * 
 * Daha tasaruflu ve performanslı olması için bazı basit değerleri
 * bayrak yardımıyla tutarız. Açık ya da kapalı olduğu bilgisi bize
 * yeterli olur ve bunu da tek 1 bit ile sağlayabiliriz. Bu sayede
 * fazlaca bellek alanı kullanacak değişkenler oluşturmak yerine
 * tek bir değişkenin bit değerlerini kullanarak yapabiliriz
 */

// Include
#include <cstdint>

// Namespace
namespace core::flag
{
    // Using
    using flag_t = std::uint32_t;

    // Struct
    struct Flag
    {
        private:
            flag_t m_value {};

        public:
            constexpr explicit Flag(const flag_t ar_value = 0) noexcept
            : m_value(ar_value)
            {}

            [[maybe_unused]] [[nodiscard]]
            constexpr flag_t operator|(const flag_t ar_value) const noexcept
            { return m_value | ar_value; }

            [[maybe_unused]] [[nodiscard]]
            constexpr flag_t operator|=(const flag_t ar_value) noexcept
            { m_value |= ar_value; return m_value; }

            [[maybe_unused]] [[nodiscard]]
            constexpr bool operator&(const flag_t ar_value) const noexcept
            { return m_value & ar_value; }

            [[maybe_unused]] [[nodiscard]]
            constexpr bool has(const flag_t ar_value) const noexcept
            { return (m_value & ar_value) != 0; }

            [[maybe_unused]]
            constexpr void set(const flag_t ar_value) noexcept
            { m_value = ar_value; }

            [[maybe_unused]] [[nodiscard]]
            constexpr flag_t get() const noexcept
            { return m_value; }

            [[maybe_unused]]
            constexpr void add(const flag_t ar_value) noexcept
            { m_value |= ar_value; }

            [[maybe_unused]]
            constexpr void unset(const flag_t ar_value) noexcept
            { m_value &= ~ar_value; }

            [[maybe_unused]]
            constexpr void change(const flag_t ar_remove, const flag_t ar_add) noexcept
            { m_value = (m_value & ~ar_remove) | ar_add; }

            [[maybe_unused]]
            constexpr void clear() noexcept
            { m_value = 0; }
    };
}
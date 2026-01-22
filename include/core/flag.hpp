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
#include <type_traits>

// Namespace
namespace core::flag
{
    // Struct
    template<typename I>
    struct Flag
    {
        static_assert(std::is_integral<I>::value, "Flag<I>: <I> must be an integral type");
        static_assert(sizeof(I) >= sizeof(std::uint8_t) && sizeof(I) <= sizeof(std::uint64_t), "Flag<I>: <I> must be between uint8_t and uint64_t");
        static_assert(!std::is_same<I, bool>::value, "Flag<I>: <bool> is not allowed");

        private:
            I m_value {};

        public:
            constexpr explicit Flag(const I ar_value) noexcept
            : m_value(ar_value)
            {}

            [[maybe_unused]] [[nodiscard]]
            constexpr Flag operator|(const I ar_value) const noexcept
            { return m_value | ar_value; }

            [[maybe_unused]] [[nodiscard]]
            constexpr Flag operator|=(const I ar_value) noexcept
            { m_value |= ar_value; return *this; }

            [[maybe_unused]] [[nodiscard]]
            constexpr bool operator&(const I ar_value) const noexcept
            { return m_value & m_value; }

            [[maybe_unused]]
            constexpr void set(const I ar_value) noexcept
            { m_value = ar_value; }

            [[maybe_unused]] [[nodiscard]]
            constexpr I get() const noexcept
            { return m_value; }

            [[maybe_unused]]
            constexpr void add(const I ar_value) noexcept
            { m_value |= ar_value; }

            [[maybe_unused]]
            constexpr void unset(const I ar_value) noexcept
            { m_value &= ~ar_value; }

            [[maybe_unused]]
            constexpr void change(const I ar_remove, const I ar_add) noexcept
            { m_value = (m_value & ~ar_remove) | ar_add; }

            [[maybe_unused]]
            constexpr void clear() noexcept
            { m_value = 0; }
    };
}
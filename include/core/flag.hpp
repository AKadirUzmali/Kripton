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
            constexpr explicit Flag(flag_t ar_value = 0) noexcept
            : m_value(ar_value)
            {}

            [[maybe_unused]] [[nodiscard]]
            constexpr flag_t operator|(flag_t ar_value) const noexcept
            { return m_value | ar_value; }

            [[maybe_unused]] [[nodiscard]]
            constexpr flag_t operator|=(flag_t ar_value) noexcept
            { m_value |= ar_value; return m_value; }

            [[maybe_unused]] [[nodiscard]]
            constexpr bool operator&(flag_t ar_value) const noexcept
            { return m_value & ar_value; }

            [[maybe_unused]] [[nodiscard]]
            constexpr bool has(flag_t ar_value) const noexcept
            { return (m_value & ar_value) != 0; }

            template<typename... Args>
            [[maybe_unused]] [[nodiscard]]
            constexpr bool has_any(Args... ar_values) const noexcept
            {
                static_assert((std::is_same_v<Args, flag_t> && ...), "All arguments must be flag_t");
                return (((m_value & ar_values) != 0) || ...);
            }
            
            template<typename... Args>
            [[maybe_unused]] [[nodiscard]]
            constexpr bool has_all(Args... ar_values) const noexcept
            {
                static_assert((std::is_same_v<Args, flag_t> && ...), "All arguments must be flag_t");
                return (((m_value & ar_values) != 0) && ...);
            }

            [[maybe_unused]]
            constexpr void set(flag_t ar_value) noexcept
            { m_value = ar_value; }

            [[maybe_unused]] [[nodiscard]]
            constexpr flag_t get() const noexcept
            { return m_value; }

            template<typename... Args>
            [[maybe_unused]]
            constexpr void add(Args... ar_values) noexcept
            {
                static_assert((std::is_same_v<Args, flag_t> && ...), "All arguments must be flag_t");
                (( m_value |= ar_values ), ...);
            }

            template<typename... Args>
            [[maybe_unused]]
            constexpr void unset(Args... ar_values) noexcept
            {
                static_assert((std::is_same_v<Args, flag_t> && ...), "All arguments must be flag_t");
                (( m_value &= ~ar_values ), ...);
            }

            [[maybe_unused]]
            constexpr void change(flag_t ar_remove, flag_t ar_add) noexcept
            { m_value = (m_value & ~ar_remove) | ar_add; }

            [[maybe_unused]]
            constexpr void clear() noexcept
            { m_value = 0; }
    };
}
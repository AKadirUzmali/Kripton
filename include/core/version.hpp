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
#include <chrono>

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
            constexpr bool operator==(const Version& ar_other) const noexcept
            {
                return
                    this->m_major == ar_other.m_major &&
                    this->m_minor == ar_other.m_minor &&
                    this->m_patch == ar_other.m_patch;
            }

            [[maybe_unused]] [[nodiscard]]
            constexpr bool operator!=(const Version& ar_other) const noexcept 
            { return !(*this == ar_other); }
    };

    // Struct
    struct TimeVersion
    {
        private:
            static constexpr std::size_t s_reset_interval = 1000; // 1sn = 1000ms
            
            std::atomic<std::size_t> m_counter { 0 };
            std::atomic<std::size_t> m_timestamp { 0 };

        public:
            [[maybe_unused]] [[nodiscard]]
            static inline size_t get_timestamp() noexcept
            {
                using namespace std::chrono;

                return static_cast<std::size_t>(
                    duration_cast<milliseconds>(
                        steady_clock::now().time_since_epoch()
                    ).count()
                );
            }

        public:
            TimeVersion(
                const std::size_t ar_counter = 0,
                const std::size_t ar_timestamp = TimeVersion::get_timestamp()
            ) noexcept
            : m_counter(ar_counter), m_timestamp(ar_timestamp)
            {}

            [[maybe_unused]] [[nodiscard]]
            constexpr bool operator==(const TimeVersion& ar_other) const noexcept
            {
                return this->m_counter.load(std::memory_order_acquire) == ar_other.m_counter.load(std::memory_order_acquire)
                    && this->m_timestamp.load(std::memory_order_acquire) == ar_other.m_timestamp.load(std::memory_order_acquire);
            }

            [[maybe_unused]] [[nodiscard]]
            constexpr bool operator!=(const TimeVersion& ar_other) const noexcept
            { return !(*this == ar_other); }

            [[maybe_unused]]
            void operator++() noexcept
            {
                const std::size_t tm_now = TimeVersion::get_timestamp();
                const std::size_t tm_last = this->m_timestamp.load(std::memory_order_relaxed);

                if( (tm_now - tm_last) > s_reset_interval ) {
                    this->m_counter.store(1, std::memory_order_relaxed);
                    this->m_timestamp.store(tm_now, std::memory_order_release);
                }
                else {
                    this->m_counter.fetch_add(1, std::memory_order_release);
                }
            }

            [[maybe_unused]]
            void operator++(int) noexcept
            { ++(*this); }
    };
}
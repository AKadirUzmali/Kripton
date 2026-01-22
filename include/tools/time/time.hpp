// Abdulkadir U. - 16/01/2026
// Abdulkadir U. - 21/01/2026
#pragma once

/**
 * Time (Zaman)
 * 
 * Zaman ile ilgili bilgileri alabilmek için
 * bazı fonksiyonları barındıracak yardımcı yapı
 */

// Include
#include <string_view>
#include <sstream>
#include <iomanip>
#include <chrono>

#include <core/platform.hpp>

// Namespace
namespace tools::time
{
    [[maybe_unused]] [[nodiscard]] std::string_view current_date();
    [[maybe_unused]] [[nodiscard]] std::string_view current_time();
    [[maybe_unused]] [[nodiscard]] std::string_view current_timestamp();

    /**
     * @brief Current Date
     * 
     * Sistemin o an bulunduğu tarihi
     * gün-ay-yıl olarak alabilmeyi sağlar.
     * Bu sayede kayıt işlemlerinin
     * ne zaman yapıldığı bilgisini okumayı sağlar
     * 
     * @return string_view
     */
    [[maybe_unused]] [[nodiscard]]
    std::string_view current_date()
    {
        const auto tm_now = std::chrono::system_clock::now();
        const auto tm_time = std::chrono::system_clock::to_time_t(tm_now);

        std::tm tm_tm {};

        #if __OS_POSIX__
            localtime_r(&tm_time, &tm_tm);
        #elif __OS_WINDOWS__
            localtime_s(&tm_tm, &tm_time);
        #else
            tm_tm = *std::localtime(&tm_time);
        #endif

        constexpr size_t tm_time_buffer_size = 16;
        char tm_buffer[tm_time_buffer_size]; // YYYY-MM-DD + '\0'
        std::strftime(tm_buffer, sizeof(tm_buffer), "%Y-%m-%d", &tm_tm);

        return std::string_view{ tm_buffer };
    }

    /**
     * @brief Current Time
     * 
     * Sistemin o an bulunduğu zamanı
     * almayı sağlamak için gereklidir.
     * Kayıt işlemlerinde yapılan işlem zamanını
     * belirtmede işe yarar
     * 
     * @return string_view
     */
    [[maybe_unused]] [[nodiscard]]
    std::string_view current_time()
    {
        auto tm_now = std::chrono::system_clock::now();
        auto tm_time = std::chrono::system_clock::to_time_t(tm_now);
        auto tm_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tm_now.time_since_epoch()) % 1000;

        std::tm tm_tm {};

        #if __OS_POSIX__
            localtime_r(&tm_time, &tm_tm);
        #elif __OS_WINDOWS__
            localtime_s(&tm_tm, &tm_time);
        #else
            tm_tm = *std::localtime(&tm_time);
        #endif

        constexpr size_t tm_time_buffer_size = 32;
        char tm_buffer[tm_time_buffer_size]; // HH:MM:SS:MMM + '\0'

        std::snprintf(
            tm_buffer, sizeof(tm_buffer),
            "%02d:%02d:%02d:%03lld",
            tm_tm.tm_hour,
            tm_tm.tm_min,
            tm_tm.tm_sec,
            static_cast<long long>(tm_ms.count())
        );

        return std::string_view { tm_buffer };
    }

    /**
     * @brief Current Time Stamp
     * 
     * Sistemin o an bulunduğu gün-ay-yıl ve zamanı
     * almayı sağlamak için gereklidir.
     * Kayıt işlemlerinde yapılan işlem zamanını
     * belirtmede işe yarar
     * 
     * @return string_view
     */
    [[maybe_unused]] [[nodiscard]]
    std::string_view current_timestamp()
    {
        auto tm_now = std::chrono::system_clock::now();
        auto tm_time = std::chrono::system_clock::to_time_t(tm_now);
        auto tm_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tm_now.time_since_epoch()) % 1000;

        std::tm tm_tm {};

        #if __OS_POSIX__
            localtime_r(&tm_time, &tm_tm);
        #elif __OS_WINDOWS__
            localtime_s(&tm_tm, &tm_time);
        #else
            tm_tm = *std::localtime(&tm_time);
        #endif

        constexpr size_t tm_time_buffer_size = 64;
        static thread_local char tm_buffer[tm_time_buffer_size]; // YYYY-MM-DD + HH:MM:SS:MMM + '\0'

        std::snprintf(
            tm_buffer, sizeof(tm_buffer),
            "%04d-%02d-%02d %02d:%02d:%02d:%03lld",
            tm_tm.tm_year + 1900,
            tm_tm.tm_mon + 1,
            tm_tm.tm_mday,
            tm_tm.tm_hour,
            tm_tm.tm_min,
            tm_tm.tm_sec,
            static_cast<long long>(tm_ms.count())
        );

        return std::string_view { tm_buffer };
    }
}
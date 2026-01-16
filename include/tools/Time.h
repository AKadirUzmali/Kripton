// Abdulkadir U. - 16/01/2026
#pragma once

/**
 * Time
 * 
 * Zaman ile ilgili bilgileri alabilmek için
 * bazı fonksiyonları barındıracak yardımcı yapı
 */

// Include:
#include <string_view>
#include <sstream>
#include <iomanip>
#include <chrono>

#include <tools/Platform.h>

// Namespace:
namespace tools::time
{
    inline std::string current_date();
    inline std::string current_time();
    inline std::string current_timestamp();

    /**
     * @brief Current Date
     * 
     * Sistemin o an bulunduğu tarihi
     * gün-ay-yıl olarak alabilmeyi sağlar.
     * Bu sayede kayıt işlemlerinin
     * ne zaman yapıldığı bilgisini okumayı sağlar
     * 
     * @return string
     */
    [[maybe_unused]]
    inline std::string current_date()
    {
        const auto now = std::chrono::system_clock::now();
        const auto time = std::chrono::system_clock::to_time_t(now);

        std::tm tm {};

        #if __OS_POSIX__
            localtime_r(&time, &tm);
        #elif __OS_WINDOWS__
            localtime_s(&tm, &time);
        #else
            tm = *std::localtime(&time);
        #endif

        constexpr size_t time_buffer_size = 16;
        char buffer[time_buffer_size]; // YYYY-MM-DD + '\0'
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", &tm);

        return std::string(buffer);
    }

    /**
     * @brief Current Time
     * 
     * Sistemin o an bulunduğu zamanı
     * almayı sağlamak için gereklidir.
     * Kayıt işlemlerinde yapılan işlem zamanını
     * belirtmede işe yarar
     * 
     * @return string
     */
    [[maybe_unused]]
    inline std::string current_time()
    {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        std::tm tm {};

        #if __OS_POSIX__
            localtime_r(&time, &tm);
        #elif __OS_WINDOWS__
            localtime_s(&tm, &time);
        #else
            tm = *std::localtime(&time);
        #endif

        constexpr size_t time_buffer_size = 32;
        char buffer[time_buffer_size]; // HH:MM:SS:MMM + '\0'

        std::snprintf(
            buffer, sizeof(buffer),
            "%02d:%02d:%02d:%03lld",
            tm.tm_hour,
            tm.tm_min,
            tm.tm_sec,
            static_cast<long long>(ms.count())
        );

        return std::string(buffer);
    }

    /**
     * @brief Current Time Stamp
     * 
     * Sistemin o an bulunduğu gün-ay-yıl ve zamanı
     * almayı sağlamak için gereklidir.
     * Kayıt işlemlerinde yapılan işlem zamanını
     * belirtmede işe yarar
     * 
     * @return string
     */
    [[maybe_unused]]
    inline std::string current_timestamp()
    {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        std::tm tm {};

        #if __OS_POSIX__
            localtime_r(&time, &tm);
        #elif __OS_WINDOWS__
            localtime_s(&tm, &time);
        #else
            tm = *std::localtime(&time);
        #endif

        constexpr size_t time_buffer_size = 64;
        char buffer[time_buffer_size]; // YYYY-MM-DD + HH:MM:SS:MMM + '\0'

        std::snprintf(
            buffer, sizeof(buffer),
            "%04d-%02d-%02d %02d:%02d:%02d:%03lld",
            tm.tm_year + 1900,
            tm.tm_mon + 1,
            tm.tm_mday,
            tm.tm_hour,
            tm.tm_min,
            tm.tm_sec,
            static_cast<long long>(ms.count())
        );

        return std::string(buffer);
    }
}
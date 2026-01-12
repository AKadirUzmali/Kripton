// Abdulkadir U. - 24/10/2025
// Abdulkadir U. - 12/01/2026
#pragma once

/**
 * Platform
 *
 * Platform yani işletim sistemine göre
 * işlem yapabilmemi kolaylaştıracak bir yapı
 * 
 * POSIX -> BSD/LINUX
 * DOS   -> WINDOWS
 */

// Define:
#if defined(_WIN32) || defined(_WIN64)
    #define __PLATFORM_DOS__ 1
    #define __PLATFORM_WINDOWS__ 1

    #define __PLATFORM_POSIX__ 0
    #define __PLATFORM_LINUX__ 0
    #define __PLATFORM_BSD__  0

    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
#elif defined(__linux__)
    #define __PLATFORM_DOS__ 0
    #define __PLATFORM_WINDOWS__ 0

    #define __PLATFORM_POSIX__ 1
    #define __PLATFORM_LINUX__ 1
    #define __PLATFORM_BSD__  0
#elif defined(__FreeBSD__) || defined(__unix__)
    #define __PLATFORM_DOS__ 0
    #define __PLATFORM_WINDOWS__ 0

    #define __PLATFORM_POSIX__ 1
    #define __PLATFORM_LINUX__ 0
    #define __PLATFORM_BSD__  1
#else
    #error "[ERROR] PLATFORM: BSD/LINUX/WINDOWS"
#endif

// Include:
#include <cstdint>
#include <string_view>
#include <sstream>
#include <iomanip>
#include <chrono>

// Namespace:
namespace tools::os
{
    // Operating System:
    inline constexpr bool os_posix = __PLATFORM_POSIX__;
    inline constexpr bool os_dos = __PLATFORM_DOS__;
    inline constexpr bool os_bsd = __PLATFORM_BSD__;
    inline constexpr bool os_windows = __PLATFORM_WINDOWS__;
    inline constexpr bool os_linux = __PLATFORM_LINUX__;

    // Enum:
    enum class Platform : uint8_t {
        Unknown = 0,
        Bsd,
        Linux,
        Windows
    };

    // Function Define:
    inline constexpr Platform current() noexcept;
    inline constexpr std::string_view current_name() noexcept;

    inline std::string current_date();
    inline std::string current_time();
    inline std::string current_timestamp();

    /**
     * @brief Current
     * 
     * Derleme zamanında işletim sistemi tespiti
     * Bu sayede işletim sistemi hakkında düzgün
     * ve kullanışlı bir bilgiye sahip olacağız
     * 
     * @return Platform
     */
    [[maybe_unused]]
    inline constexpr Platform current() noexcept
    {
        if constexpr (os_windows)
            return Platform::Windows;
        else if constexpr (os_linux)
            return Platform::Linux;
        else if constexpr (os_bsd)
            return Platform::Bsd;
        
        return Platform::Unknown;
    }

    /**
     * @brief Current Name
     * 
     * İşletim sisteminin insan okunabilir hali
     * fakat bunu derleme zamanında yaparak
     * performans tasarufu sağlar
     * 
     * @return string_view
     */
    [[maybe_unused]]
    inline constexpr std::string_view current_name() noexcept
    {
        if constexpr (os_windows)
            return "windows";
        else if constexpr (os_linux)
            return "linux";
        else if constexpr (os_bsd)
            return "bsd";
        
        return "unknown";
    }

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

        #if __PLATFORM_POSIX__
            localtime_r(&time, &tm);
        #elif __PLATFORM_DOS__
            localtime_s(&tm, &time);
        #else
            tm = *std::localtime(&time);
        #endif

        char buffer[11]; // YYYY-MM-DD + '\0'
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

        #if __PLATFORM_POSIX__
            localtime_r(&time, &tm);
        #elif __PLATFORM_DOS__
            localtime_s(&tm, &time);
        #else
            tm = *std::localtime(&time);
        #endif

        char buffer[16]; // HH:MM:SS:MMM + '\0'

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

        #if __PLATFORM_POSIX__
            localtime_r(&time, &tm);
        #elif __PLATFORM_DOS__
            localtime_s(&tm, &time);
        #else
            tm = *std::localtime(&time);
        #endif

        char buffer[32]; // YYYY-MM-DD + HH:MM:SS:MMM + '\0'

        std::snprintf(
            buffer, sizeof(buffer),
            "%04d-%02d-%02d %02d:%02d:%02d.%03lld",
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

    // Platform:
    inline constexpr bool is_posix()    noexcept { return os_posix; }
    inline constexpr bool is_dos()      noexcept { return os_dos; }
    inline constexpr bool is_bsd()      noexcept { return os_bsd;  }
    inline constexpr bool is_linux()    noexcept { return os_linux; }
    inline constexpr bool is_windows()  noexcept { return os_windows; }
}
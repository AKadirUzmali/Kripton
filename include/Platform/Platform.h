// Abdulkadir U. - 24/10/2025
#pragma once

/**
 * Platform
 *
 * Platform yani işletim sistemine göre
 * işlem yapabilmemi kolaylaştıracak bir yapı
 */

// Include:
#include <string_view>
#include <chrono>
#include <sstream>
#include <iomanip>

// PreProcessor Directives:
#if defined(_WIN32) || defined(_WIN64)
    #define __PLATFORM_WINDOWS__
    #define __PLATFORM_DOS__

    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
#elif defined(__linux__)
    #define __PLATFORM_LINUX__
#elif defined(__FreeBSD__) || defined(__unix__)
    #define __PLATFORM_UNIX__
#else
    #define __PLATFORM_UNKNOWN__
#endif

#if defined(__PLATFORM_LINUX__) || defined(__PLATFORM_UNIX__)
    #define __PLATFORM_POSIX__
#endif

// Namespace: Platform
namespace platform
{
    // Enum Class: Os Code
    enum class e_os : int {
        Unknown = 0,
        Windows,
        Linux,
        Unix
    };

    /**
     * @brief [Constexpr] Current Operating System
     * 
     * Derleme zamanında işletim sistemi tespiti
     * 
     * @return Os
     */
    constexpr e_os current() noexcept
    {
        #if defined(__PLATFORM_WINDOWS__)
            return e_os::Windows;
        #elif defined(__PLATFORM_LINUX__)
            return e_os::Linux;
        #elif defined(__PLATFORM_UNIX__)
            return e_os::Unix;
        #else
            return e_os::Unknown;
        #endif
    }

    /**
     * @brief [Constexpr] Current Operating System Text
     * 
     * İşletim sisteminin insan okunabilir hali
     * 
     * @return std::string
     */
    [[maybe_unused]]
    static inline std::string name() noexcept
    {
        #if defined(__PLATFORM_WINDOWS__)
            return "Windows";
        #elif defined(__PLATFORM_LINUX__)
            return "Linux";
        #elif defined(__PLATFORM_UNIX__)
            return "Unix";
        #else
            return "Unknown";
        #endif
    }

    /**
     * @brief [Static] Current Date
     * 
     * Sistemin o an bulunduğu tarihi
     * gün-ay-yıl olarak alabilmeyi sağlar
     * 
     * @return string
     */
    [[maybe_unused]]
    static std::string current_date() noexcept
    {
        const auto tmp__now = std::chrono::system_clock::now();
        const auto tmp__time = std::chrono::system_clock::to_time_t(tmp__now);

        std::tm tmp__tm {};

        // Os: Windows
        #if defined(__PLATFORM_WINDOWS__)
            localtime_s(&tmp__tm, &tmp__time);
        #elif defined(__PLATFORM_LINUX__) || defined(__PLATFORM_UNIX__)
            localtime_r(&tmp__time, &tmp__tm);
        #endif

        std::ostringstream tmp__oss;
        tmp__oss << std::put_time(&tmp__tm, "%Y-%m-%d");

        return tmp__oss.str();
    }

    /**
     * @brief [Static] Current Time
     * 
     * Sistemin o an bulunduğu zamanı
     * almayı sağlamak için
     * 
     * @return string
     */
    [[maybe_unused]]
    static std::string current_time() noexcept
    {
        using namespace std::chrono;

        auto now = system_clock::now();
        auto in_time_t = system_clock::to_time_t(now);
        auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%H:%M:%S")
            << ':' << std::setw(3) << std::setfill('0') << ms.count();

        return ss.str();
    }

    // Platform
    constexpr bool is_windows()  noexcept { return current() == e_os::Windows; }
    constexpr bool is_linux()    noexcept { return current() == e_os::Linux; }
    constexpr bool is_unix()     noexcept { return current() == e_os::Unix;  }
    constexpr bool is_posix()    noexcept { return is_linux() || is_unix(); }
    constexpr bool is_platform() noexcept { return is_posix() || is_windows(); }
}
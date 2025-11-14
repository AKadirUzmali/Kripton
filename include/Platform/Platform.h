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

// Os: Windows
#if defined(_WIN32) || defined(_WIN64)
    #include <Windows.h>
    #include <time.h>
// Os: Linux | Unix
#elif defined(__linux__) || defined(__unix__) || defined(__unix)
    #include <ctime>
#endif

// Namespace: Platform
namespace platform
{
    // Enum Class: Os Code
    enum class e_os {
        Windows,
        Linux,
        Unix,
        Unknown
    };

    /**
     * @brief Current Operating System
     * 
     * Derleme zamanında işletim sistemi tespiti
     * 
     * @return Os
     */
    constexpr e_os current() noexcept
    {
        #if defined(_WIN32) || defined(_WIN64)
            return e_os::Windows;
        #elif defined(__linux__)
            return e_os::Linux;
        #elif defined(__unix__)
            return e_os::Unix;
        #else
            return e_os::Unknown;
        #endif
    }

    /**
     * @brief Current Operating System Text
     * 
     * İşletim sisteminin insan okunabilir hali
     * 
     * @return std::string_view
     */
    constexpr std::string_view name() noexcept
    {
        if constexpr (current() == e_os::Windows)
            return "Windows";
        else if constexpr (current() == e_os::Linux)
            return "Linux";
        else if constexpr(current() == e_os::Unix)
            return "Unix";
        else
            return "Unknown";
    }

    /**
     * @brief Current Time
     * 
     * Sistemin o an bulunduğu zamanı
     * almayı sağlamak için
     * 
     * @return string
     */
    [[maybe_unused]]
    static std::string current_time() noexcept
    {
        const auto tmp__now = std::chrono::system_clock::now();
        const auto tmp__time = std::chrono::system_clock::to_time_t(tmp__now);

        std::tm tmp__tm {};

        // Os: Windows
        #if defined(_WIN32)
            localtime_s(&tmp__tm, &tmp__time);
        #elif defined(__linux__) || defined(__unix__) || defined(__unix)
            localtime_r(&tmp__time, &tmp__tm);
        #endif
            return "0000-00-00 00:00:00";
        
        std::ostringstream tmp__oss;
        tmp__oss << std::put_time(&tmp__tm, "%Y-%m-%d %H:%M:%S");

        return tmp__oss.str();
    }

    // Platform
    constexpr bool is_windows()  noexcept { return current() == e_os::Windows; }
    constexpr bool is_linux()    noexcept { return current() == e_os::Linux; }
    constexpr bool is_unix()     noexcept { return current() == e_os::Unix;  }
    constexpr bool is_posix()    noexcept { return is_linux() || is_unix(); }
    constexpr bool is_platform() noexcept { return is_posix() || is_windows(); }
}
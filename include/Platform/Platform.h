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

// Namespace: Platform
namespace platform
{
    // Enum Class: OS
    enum class Os {
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
     * @return constexpr Os
     */
    constexpr Os current() noexcept
    {
        #if defined(_WIN32) || defined(_WIN64)
            return Os::Windows;
        #elif defined(__linux__)
            return Os::Linux;
        #elif defined(__unix__)
            return Os::Unix;
        #else
            return Os::Unknown;
        #endif
    }

    /**
     * @brief Current Operating System Text
     * 
     * İşletim sisteminin insan okunabilir hali
     * 
     * @return constexpr std::string_view
     */
    constexpr std::string_view name() noexcept
    {
        constexpr Os operatingsystem = current();

        switch ( operatingsystem )
        {
            case Os::Windows: return "Windows";
            case Os::Linux:   return "Linux";
            case Os::Unix:    return "Unix";
            default:          return "Unknown";
        }
    }

    // Yardımcı Fonksiyonlar
    constexpr inline bool is_windows()  noexcept { return current() == Os::Windows; }
    constexpr inline bool is_linux()    noexcept { return current() == Os::Linux; }
    constexpr inline bool is_unix()     noexcept { return current() == Os::Unix;  }
    constexpr inline bool is_posix()    noexcept { return is_linux() || is_unix(); }
    constexpr inline bool is_platform() noexcept { return is_posix() || is_windows(); }

    // Os: Windows
    #if defined(_WIN32) || defined(_WIN64)
        #include <Windows.h>
    #endif
}
// Abdulkadir U. - 24/10/2025
// Abdulkadir U. - 12/01/2026
#pragma once

/**
 * Os
 *
 * Os yani işletim sistemine göre
 * işlem yapabilmemi kolaylaştıracak bir yapı
 * 
 * UNIX -> BSD
 * UNIX-LIKE -> LINUX
 * DOS -> WINDOWS
 * 
 * POSIX -> BSD/LINUX
 */

// Define:
#if defined(_WIN32) || defined(_WIN64)
    #define __PFM_DOS__ 1
    #define __OS_WINDOWS__ 1

    #define __PFM_UNIX__ 0
    #define __PFM_UNIX_LIKE__ 0
    #define __PFM_DOS__ 1

    #define __OS_POSIX__ 0
    #define __OS_LINUX__ 0
    #define __OS_BSD__  0

    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
#elif defined(__linux__)
    #define __PFM_DOS__ 0
    #define __OS_WINDOWS__ 0

    #define __PFM_UNIX__ 0
    #define __PFM_UNIX_LIKE__ 1
    #define __PFM_DOS__ 0

    #define __OS_POSIX__ 1
    #define __OS_LINUX__ 1
    #define __OS_BSD__  0
#elif defined(__FreeBSD__) || defined(__unix__)
    #define __PFM_DOS__ 0
    #define __OS_WINDOWS__ 0

    #define __PFM_UNIX__ 1
    #define __PFM_UNIX_LIKE__ 0
    #define __PFM_DOS__ 0

    #define __OS_POSIX__ 1
    #define __OS_LINUX__ 0
    #define __OS_BSD__  1
#else
    #error "[ERROR] Platform: UNIX/UNIX-LIKE/DOS"
    #error "[ERROR] Os: BSD/LINUX/WINDOWS"
#endif

// Include:
#include <cstdint>
#include <string_view>

// Namespace:
namespace tools::os
{
    // Operating System:
    inline constexpr bool pfm_unix = __PFM_UNIX__;
    inline constexpr bool pfm_unix_like = __PFM_UNIX_LIKE__;
    inline constexpr bool pfm_dos = __PFM_DOS__;

    inline constexpr bool os_posix = __OS_POSIX__;
    inline constexpr bool os_bsd = __OS_BSD__;
    inline constexpr bool os_windows = __OS_WINDOWS__;
    inline constexpr bool os_linux = __OS_LINUX__;

    // Enum:
    enum class Platform : uint8_t {
        Unknown = 0,
        Unix,
        Unix_Like,
        Dos
    };

    // Enum:
    enum class Os : uint8_t {
        Unknown = 0,
        Bsd,
        Linux,
        Windows
    };

    // String:
    inline constexpr std::string_view name_unknown = "Unknown";

    inline constexpr std::string_view name_unix = "Unix";
    inline constexpr std::string_view name_unix_like = "Unix-Like";
    inline constexpr std::string_view name_dos = "Dos";

    inline constexpr std::string_view name_bsd = "Bsd";
    inline constexpr std::string_view name_linux = "Linux";
    inline constexpr std::string_view name_windows = "Windows";

    // Function Define:
    inline constexpr Platform current_pfm() noexcept;
    inline constexpr Os current_os() noexcept;

    inline constexpr std::string_view current_pfm_name() noexcept;
    inline constexpr std::string_view current_os_name() noexcept;

    /**
     * @brief Current Platform
     * 
     * Derleme zamanında işletim sisteminin bağlı
     * olduğu aileyi tespit etmek için kullanılır
     * 
     * @return Platform
     */
    [[maybe_unused]]
    inline constexpr Platform current_pfm() noexcept
    {
        if constexpr (os_bsd)
            return Platform::Unix;
        else if constexpr (os_linux)
            return Platform::Unix_Like;
        else if constexpr (os_windows)
            return Platform::Dos;
        
        return Platform::Unknown;
    }

    /**
     * @brief Current Operating System
     * 
     * Derleme zamanında işletim sistemi tespiti
     * Bu sayede işletim sistemi hakkında düzgün
     * ve kullanışlı bir bilgiye sahip olacağız
     * 
     * @return Os
     */
    [[maybe_unused]]
    inline constexpr Os current_os() noexcept
    {
        if constexpr (os_windows)
            return Os::Windows;
        else if constexpr (os_linux)
            return Os::Linux;
        else if constexpr (os_bsd)
            return Os::Bsd;
        
        return Os::Unknown;
    }

    /**
     * @brief Current Platform Name
     * 
     * İşletim sisteminin bağlı olduğu platformun
     * adını insan okunabilir halde olması için
     * döndürüyoruz
     * 
     * @return string_view
     */
    inline constexpr std::string_view current_pfm_name() noexcept
    {
        if constexpr (pfm_unix)
            return name_unix;
        else if constexpr (pfm_unix_like)
            return name_unix_like;
        else if constexpr (pfm_dos)
            return name_dos;

        return name_unknown;
    }

    /**
     * @brief Current Operating System Name
     * 
     * İşletim sisteminin insan okunabilir hali
     * fakat bunu derleme zamanında yaparak
     * performans tasarufu sağlar
     * 
     * @return string_view
     */
    [[maybe_unused]]
    inline constexpr std::string_view current_os_name() noexcept
    {
        if constexpr (os_windows)
            return name_windows;
        else if constexpr (os_linux)
            return name_linux;
        else if constexpr (os_bsd)
            return name_bsd;
        
        return name_unknown;
    }

    // Is Function:
    inline constexpr bool is_pfm_unix()      noexcept { return pfm_unix; }
    inline constexpr bool is_pfm_unix_like() noexcept { return pfm_unix_like; }
    inline constexpr bool is_pfm_dos()       noexcept { return pfm_dos; }
    
    inline constexpr bool is_os_posix()     noexcept { return os_posix; }
    inline constexpr bool is_os_bsd()       noexcept { return os_bsd;  }
    inline constexpr bool is_os_linux()     noexcept { return os_linux; }
    inline constexpr bool is_os_windows()   noexcept { return os_windows; }
}
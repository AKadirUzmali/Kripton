// Abdulkadir U. - 21/01/2026
#pragma once

/**
 * Platform (Platform)
 * 
 * Proje tek bir işletim sistemi ve platform için değil,
 * birden çok platform ve işletim sisteminde çalışacak şekilde
 * geliştirilmektedir. Bunun için de her platformu ayırt
 * edebilmemiz gerekmekte. Ön tanımlamalar platformları ayırt
 * edebilmekte ve işlemleri özelleştirebilmekteyiz.
 * Ek bir gereklilik duymadığı için çekirdek sınıfına eklenmiştir
 * 
 * UNIX -> BSD
 * UNIX-LIKE -> LINUX
 * MSWIN -> WINDOWS
 * 
 * POSIX -> BSD/LINUX
 */

// Define
#if defined(_WIN32) || defined(_WIN64)
    #define __OS_WINDOWS__ 1

    #define __PFM_UNIX__ 0
    #define __PFM_UNIX_LIKE__ 0
    #define __PFM_MSWIN__ 1

    #define __OS_POSIX__ 0
    #define __OS_LINUX__ 0
    #define __OS_BSD__  0

    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
#elif defined(__linux__)
    #define __OS_WINDOWS__ 0

    #define __PFM_UNIX__ 0
    #define __PFM_UNIX_LIKE__ 1
    #define __PFM_MSWIN__ 0

    #define __OS_POSIX__ 1
    #define __OS_LINUX__ 1
    #define __OS_BSD__  0
#elif defined(__FreeBSD__) || defined(__unix__)
    #define __OS_WINDOWS__ 0

    #define __PFM_UNIX__ 1
    #define __PFM_UNIX_LIKE__ 0
    #define __PFM_MSWIN__ 0

    #define __OS_POSIX__ 1
    #define __OS_LINUX__ 0
    #define __OS_BSD__  1
#else
    #error "[ERROR] Platform: UNIX/UNIX-LIKE/MSWIN"
    #error "[ERROR] Os: BSD/LINUX/WINDOWS"
#endif

// Include:
#include <cstdint>

// Namespace
namespace core::platform
{
    // Operating System
    constexpr bool pfm_unix = __PFM_UNIX__;
    constexpr bool pfm_unix_like = __PFM_UNIX_LIKE__;
    constexpr bool pfm_mswin = __PFM_MSWIN__;

    constexpr bool os_posix = __OS_POSIX__;
    constexpr bool os_bsd = __OS_BSD__;
    constexpr bool os_windows = __OS_WINDOWS__;
    constexpr bool os_linux = __OS_LINUX__;

    // Enum
    enum class Platform : std::uint8_t {
        Unknown = 0,
        Unix,
        Unix_Like,
        MsWin
    };

    // Enum
    enum class Os : std::uint8_t {
        Unknown = 0,
        Bsd,
        Linux,
        Windows
    };

    // String
    constexpr const char* const name_unknown = "Unknown";

    constexpr const char* const name_unix = "Unix";
    constexpr const char* const name_unix_like = "Unix-Like";
    constexpr const char* const name_mswin = "MsWin";

    constexpr const char* const name_bsd = "Bsd";
    constexpr const char* const name_linux = "Linux";
    constexpr const char* const name_windows = "Windows";

    // Function Define
    constexpr Platform current_pfm() noexcept;
    constexpr Os current_os() noexcept;

    constexpr const char* current_pfm_name() noexcept;
    constexpr const char* current_os_name() noexcept;

    /**
     * @brief Current Platform
     * 
     * Derleme zamanında işletim sisteminin bağlı
     * olduğu aileyi tespit etmek için kullanılır
     * 
     * @return Platform
     */
    [[maybe_unused]] [[nodiscard]]
    constexpr Platform current_pfm() noexcept
    {
        if constexpr (pfm_unix)
            return Platform::Unix;
        else if constexpr (pfm_unix_like)
            return Platform::Unix_Like;
        else if constexpr (pfm_mswin)
            return Platform::MsWin;
        
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
    [[maybe_unused]] [[nodiscard]]
    constexpr Os current_os() noexcept
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
     * @return const char*
     */
    [[maybe_unused]] [[nodiscard]]
    constexpr const char* current_pfm_name() noexcept
    {
        if constexpr (pfm_unix)
            return name_unix;
        else if constexpr (pfm_unix_like)
            return name_unix_like;
        else if constexpr (pfm_mswin)
            return name_mswin;

        return name_unknown;
    }

    /**
     * @brief Current Operating System Name
     * 
     * İşletim sisteminin insan okunabilir hali
     * fakat bunu derleme zamanında yaparak
     * performans tasarufu sağlar
     * 
     * @return const char*
     */
    [[maybe_unused]]
    constexpr const char* current_os_name() noexcept
    {
        if constexpr (os_windows)
            return name_windows;
        else if constexpr (os_linux)
            return name_linux;
        else if constexpr (os_bsd)
            return name_bsd;
        
        return name_unknown;
    }

    // Is Function
    constexpr bool is_pfm_unix()      noexcept { return pfm_unix; }
    constexpr bool is_pfm_unix_like() noexcept { return pfm_unix_like; }
    constexpr bool is_pfm_mswin()     noexcept { return pfm_mswin; }
    
    constexpr bool is_os_posix()     noexcept { return os_posix; }
    constexpr bool is_os_bsd()       noexcept { return os_bsd;  }
    constexpr bool is_os_linux()     noexcept { return os_linux; }
    constexpr bool is_os_windows()   noexcept { return os_windows; }
}
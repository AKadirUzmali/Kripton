// Abdulkadir U. - 2026/01/26
#pragma once

/**
 * Console (Konsol)
 * 
 * Konsol ekranında yapılabilecek bazı şeylerin
 * yapılması için gerekli olan araçları sunar
 */

// Include
#include <core/platform.hpp>

#if __OS_WINDOWS__
    #include <windows.h>
#endif

// Namespace
namespace tools::console
{
    // Function Define
    [[maybe_unused]] inline void enable_utf8_console() noexcept;

    /**
     * @brief Enable Utf-8 Console
     * 
     * Windows ortamlar için konsol komut ekranı
     * utf-8 olmayabiliyor ve biz bunu bu alternatif
     * yöntem ile etkinleştiriyoruz
     */
    [[maybe_unused]]
    inline void enable_utf8_console() noexcept
    {
        #if __OS_WINDOWS__
            static std::atomic<bool> is_console_utf8 { false };

            if( is_console_utf8.load(std::memory_order_relaxed))
                return;
                
            is_console_utf8.store(true);

            SetConsoleOutputCP(CP_UTF8);
            SetConsoleCP(CP_UTF8);
        #endif
    }
}
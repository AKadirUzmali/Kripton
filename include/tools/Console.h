// Abdulkadir U. - 12/01/2026
#pragma once

/**
 * Console
 * 
 * Konsol komut ekranı ile ilgili yapabileceğimiz
 * bazı şeyleri araç takımını kullanarak yapacağız
 * ve bu sayede daha düzenli, okunabilir ve kullanılabilir
 * kod elde edeceğiz. Bunun yanında çoklu işletim sistemi
 * desteği sayesinde desteklenen işletim sistemlerinde
 * rahatça çalışıyor olmasını sağlayacağız
 */

// Include:
#include <iostream>
#include <string_view>
#include <atomic>
#include <array>

#include <tools/Platform.h>
#include <developer/log/Levels.h>

#if __OS_WINDOWS__
    #include <windows.h>
#endif

// Namespace:
namespace tools::console
{
    // Define:
    #if __OS_WINDOWS__
        using color = int;

        inline constexpr color color_black      = 0;
        inline constexpr color color_red        = 4;
        inline constexpr color color_green      = 2;
        inline constexpr color color_yellow     = 6;
        inline constexpr color color_blue       = 1;
        inline constexpr color color_purple     = 5;
        inline constexpr color color_turquoise  = 3;
        inline constexpr color color_white      = 7;
	inline constexpr color color_reset	= color_white;
    #elif __OS_POSIX__
        using color = std::string_view;

        inline constexpr color color_black      = "\033[30m";
        inline constexpr color color_red        = "\033[31m";
        inline constexpr color color_green      = "\033[32m";
        inline constexpr color color_yellow     = "\033[33m";
        inline constexpr color color_blue       = "\033[34m";
        inline constexpr color color_purple     = "\033[35m";
        inline constexpr color color_turquoise  = "\033[36m";
        inline constexpr color color_white      = "\033[37m";
	inline constexpr color color_reset	= "\033[0m";
    #endif

    inline constexpr std::array<color, dev::level::size_levels>
    colors { color_red, color_green, color_yellow, color_blue, color_purple, color_reset };

    // Function Define:
    inline void enable_utf8_console() noexcept;
    inline color get_color(size_t) noexcept;
    inline void set_color(color) noexcept;
    inline void reset_color() noexcept;

    /**
     * @brief Enable Utf-8 Console
     * 
     * Windows ortamlar için konsol komut ekranı
     * utf-8 olmayabiliyor ve biz bunu bu alternatif
     * yöntem ile etkinleştiriyoruz
     */
    [[maybe_unused]]
    inline void enable_utf8_console(
    ) noexcept
    {
        #if __OS_WINDOWS__
            static std::atomic<bool> is_console_utf8 { false };
            bool expected = false;

            if( !is_console_utf8.compare_exchange_strong(expected, true) )
                return;

            SetConsoleOutputCP(CP_UTF8);
            SetConsoleCP(CP_UTF8);
        #endif
    }

    /**
     * @brief Get Color
     * 
     * Verilen durum seviyesine göre ait olan rengi
     * listeden index numarasına göre seçip getirecek
     * 
     * @param size_t Index
     * @return color
     */
    [[maybe_unused]]
    inline color get_color(
        size_t index
    ) noexcept
    {
        return colors[dev::level::get_valid_index(index)];
    }

    /**
     * @brief Set Color
     * 
     * Konsol ya da terminal ekranında
     * yazılacak yazıların rengini değiştirmeyi
     * sağlar ve çoklu platform desteği ile
     * çalışır
     * 
     * @param color Color
     */
    [[maybe_unused]]
    inline void set_color(
        color console_color
    ) noexcept
    {
        #if __OS_WINDOWS__
            static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            FlushConsoleInputBuffer(hConsole);
            SetConsoleTextAttribute(hConsole, console_color);
        #elif __OS_POSIX__
            std::cout << console_color << std::flush;
        #endif
    }
        
    /**
     * @brief Reset Color
     * 
     * Konsol veya terminal ekranının yazı rengini
     * değiştirmek yerine tam aksine eski haline çevirir
     */
    [[maybe_unused]]
    inline void reset_color(
    ) noexcept
    {
        set_color(color_reset);
    }
}
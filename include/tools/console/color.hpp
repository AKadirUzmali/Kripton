// Abdulkadir U. - 21/01/2026
#pragma once

/**
 * Color (Renk)
 * 
 * Konsol komut ekranı rengi ile ilgili yapabileceğimiz
 * bazı şeyleri araç takımını kullanarak yapacağız
 * ve bu sayede daha düzenli, okunabilir ve kullanılabilir
 * kod elde edeceğiz. Bunun yanında çoklu işletim sistemi
 * desteği sayesinde desteklenen işletim sistemlerinde
 * rahatça çalışıyor olmasını sağlayacağız
 */

// Include
#include <iostream>
#include <string_view>
#include <atomic>
#include <array>

#include <core/platform.hpp>

#if __OS_WINDOWS__
    #include <windows.h>
#endif

// Namespace
namespace tools::console::color
{
    // Enum
    enum class color_t : std::uint8_t
    {
        Red = 0,
        Green,
        Yellow,
        Blue,
        Purple,
        Black,
        Turquoise,
        White,
        Reset
    };

    #if __OS_WINDOWS__
        [[maybe_unused]] [[nodiscard]]
        constexpr int get_color(color_t ar_c) noexcept
        {
            switch(ar_c)
            {
                case color_t::Black: return 0;
                case color_t::Red: return 4;
                case color_t::Green: return 2;
                case color_t::Yellow: return 6;
                case color_t::Blue: return 1;
                case color_t::Purple: return 5;
                case color_t::Turquoise: return 3;
                case color_t::White: return 7;
                case color_t::Reset: return 7;
            }
            return 7;
        }
    #elif __OS_POSIX__
        [[maybe_unused]] [[nodiscard]]
        constexpr const char* get_color(color_t ar_c) noexcept
        {
            switch(ar_c)
            {
                case color_t::Black: return "\033[30m";
                case color_t::Red: return "\033[31m";
                case color_t::Green: return "\033[32m";
                case color_t::Yellow: return "\033[33m";
                case color_t::Blue: return "\033[34m";
                case color_t::Purple: return "\033[35m";
                case color_t::Turquoise: return "\033[36m";
                case color_t::White: return "\033[37m";
                case color_t::Reset: return "\033[0m";
            }

            return "\033[0m";
        }
    #endif

    // Array
    inline constexpr color_t console_colors[] =
    {
        color_t::Red,
        color_t::Green,
        color_t::Yellow,
        color_t::Blue,
        color_t::Purple,
        color_t::Black,
        color_t::Turquoise,
        color_t::White,
        color_t::Reset
    };
    inline constexpr std::size_t size_console_colors = sizeof(console_colors) / sizeof(console_colors[0]);

    // Function Define
    [[maybe_unused]] [[nodiscard]] inline color_t get_console_color(size_t ar_index) noexcept;
    [[maybe_unused]] inline void set_color(color_t ar_console_color) noexcept;
    [[maybe_unused]] inline void reset_color() noexcept;

    /**
     * @brief Get Console Color
     * 
     * Verilen durum seviyesine göre ait olan konsol rengini
     * listeden index numarasına göre seçip getirecek
     * 
     * @param size_t Index
     * @return color
     */
    [[maybe_unused]] [[nodiscard]]
    inline color_t get_console_color(size_t ar_index) noexcept
    {
        return ar_index < size_console_colors ? console_colors[ar_index] : color_t::Reset;
    }

    /**
     * @brief Set Color
     * 
     * Konsol ya da terminal ekranında
     * yazılacak yazıların rengini değiştirmeyi
     * sağlar ve çoklu platform desteği ile
     * çalışır
     * 
     * @param color_t Color
     */
    [[maybe_unused]]
    inline void set_color(color_t ar_console_color) noexcept
    {
        #if __OS_WINDOWS__
            static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            FlushConsoleInputBuffer(hConsole);
            SetConsoleTextAttribute(hConsole, get_color(ar_console_color));
        #elif __OS_POSIX__
            std::cout << get_color(ar_console_color) << std::flush;
        #endif
    }
        
    /**
     * @brief Reset Color
     * 
     * Konsol veya terminal ekranının yazı rengini
     * değiştirmek yerine tam aksine eski haline çevirir
     */
    [[maybe_unused]]
    inline void reset_color() noexcept
    {
        set_color(color_t::Reset);
    }
}
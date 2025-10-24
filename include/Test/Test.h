// Abdulkadir U. - 24/10/2025
#pragma once

/**
 * Test
 *
 * Fonksiyonların doğru sonuçlar verip vermediğini
 * anlayabilmek adına basit bir test yapısı.
 * Bu sayede renkli ve yazılı şekilde hata olup olmadığını
 * görebileceğiz ve buna göre önlem alabileceğiz.
 */

// C++
#ifndef __cplusplus
    #error "[PRE ERROR] C++ Required"
#endif

// Include:
#include <iostream>
#include <string>
#include <string_view>

// Namespace: Test
namespace test
{
    // Text: Passed | Failed
    using status = bool;

    static inline constexpr status failed = false;
    static inline constexpr status passed = true;

    static inline const char* text_failed = "[FAILED]";
    static inline const char* text_passed = "[PASSED]";

    // Color: Windows
    #if defined(_WIN32) || defined(_WIN64)
        #include <Windows.h>

        using color = int;

        static inline constexpr color color_black      = 0;
        static inline constexpr color color_red        = 4;
        static inline constexpr color color_green      = 2;
        static inline constexpr color color_yellow     = 6;
        static inline constexpr color color_blue       = 1;
        static inline constexpr color color_purple     = 5;
        static inline constexpr color color_turquoise  = 3;
        static inline constexpr color color_white      = 7;
        static inline constexpr color color_reset      = white;
    // Color: Linux & Unix
    #elif defined(__linux__) || defined(__unix__) || defined(__unix)
        using color = std::string_view;

        static inline constexpr color color_black      = "\033[30m";
        static inline constexpr color color_red        = "\033[31m";
        static inline constexpr color color_green      = "\033[32m";
        static inline constexpr color color_yellow     = "\033[33m";
        static inline constexpr color color_blue       = "\033[34m";
        static inline constexpr color color_purple     = "\033[35m";
        static inline constexpr color color_turquoise  = "\033[36m";
        static inline constexpr color color_white      = "\033[37m";
        static inline constexpr color color_reset      = "\033[0m";
    #endif

    // Set Color
    static void set_color(color _color)
    {
        // Windows
        #if defined(_WIN32) || defined(_WIN64)
            static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            FlushConsoleInputBuffer(hConsole);
            SetConsoleTextAttribute(hConsole, color);
        // Linux & Unix
        #elif defined(__linux__) || defined(__unix__) || defined(__unix)
            std::cout << _color;
        // Unknown
        #else
            (void)color;
        #endif
    }
        
    // Reset Color
    static void reset_color()
    {
        // Windows
        #if defined(_WIN32) || defined(_WIN64)
            static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            SetConsoleTextAttribute(hConsole, reset);
        // Linux & Unix
        #elif defined(__linux__) || defined(__unix__) || defined(__unix)
            std::cout << color_reset;
        #endif
    }

    /**
     * @brief Expect Equal
     * 
     * @tparam First Value
     * @tparam Second Value
     * 
     * @return Is Equal?
     */
    template <typename First, typename Second>
    static bool expect_eq(const First& _first, const Second& _second) noexcept
    {
        return (_first == _second);
    }

    /**
     * @brief Expect Equal With Message
     * 
     * @tparam First Value
     * @tparam Second Value
     * @param Message
     * 
     * @return Is Equal?
     */
    template <typename First, typename Second>
    static bool expect_eq(const First& _first, const Second& _second, const std::string& _message) noexcept
    {
        const bool result = (_first == _second);
        
        set_color(result ? color_green : color_red);
        std::cout << (result ? text_passed : text_failed) << ' ' << _message << std::endl;
        reset_color();

        return result;
    }
}
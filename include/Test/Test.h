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

// Include:
#include <cstdlib>

#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <codecvt>
#include <locale>

// Namespace: Test
namespace test
{
    // Text: Passed | Failed
    using status = bool;

    static inline constexpr status fail = false;
    static inline constexpr status pass = true;

    static inline const char* text_fail = "[ FAIL ]";
    static inline const char* text_pass = "[ PASS ]";
    static inline const char* text_info = "[ INFO ]";
    static inline const char* text_warn = "[ WARN ]";

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
        static inline constexpr color color_reset      = color_white;
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

    // UTF-32 -> UTF8
    static inline std::string to_utf8(const std::u32string& input) {
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
        return conv.to_bytes(input);
    }

    // UTF32 -> UTF8
    static inline std::string to_string(const std::u32string& input) {
        return to_utf8(input);
    }

    // Görünebilir
    static inline std::string to_visible(const std::u32string& text) {
        std::string result;
        for (auto ch : text) {
            if (ch >= 32 && ch <= 126) // ASCII
                result += static_cast<char>(ch);
            else {
                std::stringstream ss;
                ss << "\\x" << std::hex << static_cast<int>(ch);
                result += ss.str();
            }
        }
        return result;
    }

    // Set Color
    static void set_color(color _color)
    {
        // Windows
        #if defined(_WIN32) || defined(_WIN64)
            static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            FlushConsoleInputBuffer(hConsole);
            SetConsoleTextAttribute(hConsole, _color);
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
            SetConsoleTextAttribute(hConsole, color_reset);
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
        std::cout << (result ? text_pass : text_fail) << ' ' << _message << std::endl;
        reset_color();

        return result;
    }

    /**
     * @brief Exit Equal With Message
     * 
     * @tparam First Value
     * @tparam Second Value
     * @param Message
     */
    template <typename First, typename Second>
    static void exit_eq(const First& _first, const Second& _second, const std::string& _message) noexcept
    {
        if( test::expect_eq(_first, _second, _message) ) return;
        std::exit(EXIT_FAILURE);
    }

    /**
     * @brief Info Message
     * 
     * @param Message
     */
    static void info_msg(const std::string& _message) noexcept
    {
        set_color(color_blue);
        std::cout << text_info << ' ' << _message << std::endl;
        reset_color();
    }

    /**
     * @brief Warning Message
     * 
     * @param Message
     */
    static void warn_msg(const std::string& _message) noexcept
    {
        set_color(color_yellow);
        std::cout << text_warn << ' ' << _message << std::endl;
        reset_color();
    }
}
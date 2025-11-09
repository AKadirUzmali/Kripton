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

    // Enum Class: Status
    enum class e_status : unsigned short
    {
        unknown = 0x0,
        error   = 0x1,
        warning = 0x2,
        success = 0x3,
        information = 0x4,
        base
    };

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

    /**
     * @brief To UTF-8
     * 
     * UTF-32 karakter kümesindeki karakterleri
     * değişken boyutlu UTF-8 karakter kümesindeki
     * karakterlere uygunlağa göre ayarlamak
     * 
     * @param u32string& Input
     * @return string
     */
    [[maybe_unused]]
    static std::string to_utf8(const std::u32string& input) {
        std::string output;
        output.reserve(input.size() * 4);
        for (char32_t c : input) {
            if (c <= 0x7F)
                output.push_back(static_cast<char>(c));
            else if (c <= 0x7FF) {
                output.push_back(static_cast<char>(0xC0 | ((c >> 6) & 0x1F)));
                output.push_back(static_cast<char>(0x80 | (c & 0x3F)));
            } else if (c <= 0xFFFF) {
                output.push_back(static_cast<char>(0xE0 | ((c >> 12) & 0x0F)));
                output.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
                output.push_back(static_cast<char>(0x80 | (c & 0x3F)));
            } else {
                output.push_back(static_cast<char>(0xF0 | ((c >> 18) & 0x07)));
                output.push_back(static_cast<char>(0x80 | ((c >> 12) & 0x3F)));
                output.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
                output.push_back(static_cast<char>(0x80 | (c & 0x3F)));
            }
        }
        return output;
    }

    /**
     * @brief To String
     * 
     * 4 bayt (32 bit) karakter kümesindeki karakterleri
     * 1 bayt (8 bit) karakter kümesindeki karakterlere dönüştürecek
     * fakat bu karakter kümesindeki karakterlerin hepsi aynı
     * boyuta sahip değil. 1 bayt da olabilir 4 bayt da olabilir.
     * 
     * @param u32string& Input
     * @return string
     */
    [[maybe_unused]]
    static std::string to_string(const std::u32string& input) {
        return to_utf8(input);
    }

    /**
     * @brief To Visible
     * 
     * 4 bayt (32 bit) karakter kümesindeki karakterler
     * konsol ya da terminal ekranında gösterilemeyebilir
     * bu yüzden o karakterleri 16'lık (hexadecimal)
     * şekilde bile olsa görünebilir kılmak için var
     * 
     * @param ustring32& Text
     * @return string
     */
    [[maybe_unused]]
    static std::string to_visible(const std::u32string& text) {
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
    /**
     * @brief Set Color
     * 
     * Konsol ya da terminal ekranında
     * yazılacak yazıların rengini değiştirmeyi
     * sağlar ve çoklu platform desteğine sahiptir
     * 
     * @param color Color
     */
    [[maybe_unused]]
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
        
    /**
     * @brief Reset Color
     * 
     * Konsol veya terminal ekranının yazı rengini
     * değiştirmek yerine tam aksine eski haline çevirir
     */
    [[maybe_unused]]
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
    [[maybe_unused]]
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
    [[maybe_unused]]
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
    [[maybe_unused]]
    static void exit_eq(const First& _first, const Second& _second, const std::string& _message) noexcept
    {
        if( test::expect_eq(_first, _second, _message) ) return;
        std::exit(EXIT_FAILURE);
    }

    /**
     * @brief Message
     * 
     * Ekrana mesaj çıktısı vermek
     * 
     * @param Message
     */
    [[maybe_unused]]
    static void message
    (
        const e_status _status = e_status::base,
        const std::string& _message = ""
    )
    {
        switch( _status )
        {
            case e_status::error:       set_color(color_red);       std::cout << text_fail << ' '; break;
            case e_status::success:     set_color(color_green);     std::cout << text_pass << ' '; break;
            case e_status::warning:     set_color(color_yellow);    std::cout << text_warn << ' '; break;
            case e_status::information: set_color(color_blue);      std::cout << text_info << ' '; break;
            default:                    set_color(color_reset);     break;
        }

        std::cout <<  _message << std::endl;
        reset_color();
    }
}
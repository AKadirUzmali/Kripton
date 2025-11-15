// Abdulkadir U. - 15/11/2025
#pragma once

/**
 * Utf
 * 
 * Çoklu karakter setleri arasında dönüşüm işlemlerini sağlar.
 * Genel olarak UTF-8, UTF-16 ve UTF-32 formatları arasında dönüşümler yapılır.
 */

// Include:
#include <string>
#include <sstream>
#include <codecvt>
#include <locale>

// Namespace: Tool
namespace tool
{
    /**
     * @brief [Static Private] To UTF-8
     * 
     * UTF-32 metini UTF-8 metine çevirme işlemi
     * 
     * @param u32string& Text
     * @return string
     */
    [[maybe_unused]]
    std::string to_utf8(const std::u32string& _text) noexcept
    {
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> tmp__conv;
        return tmp__conv.to_bytes(_text);
    }

    /**
     * @brief [Static Private] To UTF-16
     * 
     * UTF-32 metini UTF-16 metine çevirme işlemi
     * 
     * @param u32string& Text
     * @return string
     */
    [[maybe_unused]]
    std::wstring to_utf16(const std::u32string& _text) noexcept
    {
        std::wstring tmp__result;
        for( char32_t c32 : _text )
            tmp__result.push_back(static_cast<wchar_t>(c32));

        return tmp__result;
    }

    /**
     * @brief [Static Private] To UTF-32
     * 
     * UTF-8 metini UTF-32 metine çevirme işlemi
     * 
     * @param string& Text
     * @return u32string
     */
    [[maybe_unused]]
    std::u32string to_utf32(const std::string& _text) noexcept
    {
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> tmp__conv;
        return tmp__conv.from_bytes(_text);
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
        std::string tmp__result;
        for (auto ch : text) {
            if (ch >= 32 && ch <= 126) // ASCII
                tmp__result += static_cast<char>(ch);
            else {
                std::stringstream tmp__ss;
                tmp__ss << "\\x" << std::hex << static_cast<int>(ch);
                tmp__result += tmp__ss.str();
            }
        }
        return tmp__result;
    }

    // Os: Windows
    #if defined(_WIN32) || defined(_WIN64)
        using os_utf = std::wstring(*)(const std::u32string&) noexcept;
        static constexpr os_utf to_os_utf = to_utf16;
    // Os: Linux / Bsd
    #elif defined(__linux__) || defined(__unix__) || defined(__unix)
        using os_utf = std::string(*)(const std::u32string&) noexcept;
        static constexpr os_utf to_os_utf = to_utf8;
    // Os: Unknown
    #else
        #error "Unsupported Operating System for UTF conversion!"
    #endif
}
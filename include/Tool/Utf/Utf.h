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
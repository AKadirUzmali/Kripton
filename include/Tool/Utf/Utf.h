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

// Namespace: Tool::Utf
namespace tool::utf
{
    /**
     * @brief [Static] To UTF-8
     * 
     * UTF-32 metini UTF-8 metine çevirme işlemi
     * 
     * @param u32string& Text
     * @return string
     */
    [[maybe_unused]]
    static std::string to_utf8(const std::u32string& _text) noexcept
    {
        std::string out;
        out.reserve(_text.size() * sizeof(char32_t));

        for (char32_t c32 : _text)
        {
            if (c32 <= 0x7F)
                out.push_back(static_cast<char>(c32));
            else if (c32 <= 0x7FF)
            {
                out.push_back(static_cast<char>(0xC0 | (c32 >> 6)));
                out.push_back(static_cast<char>(0x80 | (c32 & 0x3F)));
            }
            else if (c32 <= 0xFFFF)
            {
                out.push_back(static_cast<char>(0xE0 | (c32 >> 12)));
                out.push_back(static_cast<char>(0x80 | ((c32 >> 6) & 0x3F)));
                out.push_back(static_cast<char>(0x80 | (c32 & 0x3F)));
            }
            else
            {
                out.push_back(static_cast<char>(0xF0 | (c32 >> 18)));
                out.push_back(static_cast<char>(0x80 | ((c32 >> 12) & 0x3F)));
                out.push_back(static_cast<char>(0x80 | ((c32 >> 6) & 0x3F)));
                out.push_back(static_cast<char>(0x80 | (c32 & 0x3F)));
            }
        }

        return out;
    }

    /**
     * @brief [Static] To UTF-16
     * 
     * UTF-32 metini UTF-16 metine çevirme işlemi
     * 
     * @param u32string& Text
     * @return string
     */
    [[maybe_unused]]
    static std::wstring to_utf16(const std::u32string& _text) noexcept
    {
        std::wstring out;

        for (char32_t c32 : _text)
        {
            if (c32 <= 0xFFFF)
                out.push_back(static_cast<wchar_t>(c32));
            else
            {
                c32 -= 0x10000;
                wchar_t high = static_cast<wchar_t>((c32 >> 10) + 0xD800);
                wchar_t low  = static_cast<wchar_t>((c32 & 0x3FF) + 0xDC00);
                out.push_back(high);
                out.push_back(low);
            }
        }

        return out;
    }

    /**
     * @brief [Static] To UTF-32
     * 
     * UTF-8 metini UTF-32 metine çevirme işlemi
     * 
     * @param string& Text
     * @return u32string
     */
    [[maybe_unused]]
    static std::u32string to_utf32(const std::string& _text) noexcept
    {
        std::u32string out;
        out.reserve(_text.size());

        size_t i = 0;
        const size_t n = _text.size();

        while (i < n)
        {
            unsigned char c = _text[i];

            // 1 byte: 0xxxxxxx
            if (c < 0x80)
            {
                out.push_back(c);
                i++;
                continue;
            }

            // 2 byte: 110xxxxx 10xxxxxx
            if ((c >> 5) == 0x6)
            {
                if (i + 1 >= n) break;

                unsigned char c1 = _text[i + 1];
                if ((c1 >> 6) != 0x2) break;

                char32_t code =
                    ((c & 0x1F) << 6) |
                    (c1 & 0x3F);

                out.push_back(code);
                i += 2;
                continue;
            }

            // 3 byte: 1110xxxx 10xxxxxx 10xxxxxx
            if ((c >> 4) == 0xE)
            {
                if (i + 2 >= n) break;

                unsigned char c1 = _text[i + 1];
                unsigned char c2 = _text[i + 2];
                if ((c1 >> 6) != 0x2 || (c2 >> 6) != 0x2) break;

                char32_t code =
                    ((c & 0x0F) << 12) |
                    ((c1 & 0x3F) << 6) |
                    (c2 & 0x3F);

                out.push_back(code);
                i += 3;
                continue;
            }

            // 4 byte: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            if ((c >> 3) == 0x1E)
            {
                if (i + 3 >= n) break;

                unsigned char c1 = _text[i + 1];
                unsigned char c2 = _text[i + 2];
                unsigned char c3 = _text[i + 3];
                if ((c1 >> 6) != 0x2 || (c2 >> 6) != 0x2 || (c3 >> 6) != 0x2) break;

                char32_t code =
                    ((c & 0x07) << 18) |
                    ((c1 & 0x3F) << 12) |
                    ((c2 & 0x3F) << 6) |
                    (c3 & 0x3F);

                out.push_back(code);
                i += 4;
                continue;
            }

            i++;
        }

        return out;
    }

    /**
     * @brief [Static] To Lower
     * 
     * 4 bayt (32 bit) karakter kümesindeki karakterleri
     * küçük harfe çevirmek için var
     * 
     * @param ustring32& Text
     * @return ustring32&
     */
    [[maybe_unused]]
    static std::u32string to_lower(const std::u32string& text) noexcept
    {
        std::u32string result;
        for (auto ch : text) {
            if (ch >= U'A' && ch <= U'Z')
                result += ch + (U'a' - U'A');
            else
                result += ch;
        }
        return result;
    }

    /**
     * @brief [Static] To Lower
     * 
     * 1 bayt (8 bit) karakter kümesindeki karakterleri
     * küçük harfe çevirmek için var
     * 
     * @param string& Text
     * @return string&
     */
    [[maybe_unused]]
    static std::string to_lower(const std::string& text) noexcept
    {
        std::string result;
        for (auto ch : text)
        {
            if (ch >= 'A' && ch <= 'Z') result += ch + ('a' - 'A');
            else result += ch;
        }
        return result;
    }

    /**
     * @brief [Static] To Upper
     * 
     * 4 bayt (32 bit) karakter kümesindeki karakterleri
     * büyük harfe çevirmek için var
     * 
     * @param string& Text
     * @return string&
     */
    [[maybe_unused]]
    std::u32string to_upper(const std::u32string& text) {
        std::u32string result;
        result.reserve(text.size());

        for (auto c : text)
        {
            if (c >= U'a' && c <= U'z') result.push_back(c - 32);
            else result.push_back(c);
        }

        return result;
    }

    /**
     * @brief [Static] To Upper
     * 
     * 1 bayt (8 bit) karakter kümesindeki karakterleri
     * büyük harfe çevirmek için var
     * 
     * @param string& Text
     * @return string&
     */
    [[maybe_unused]]
    std::string to_upper(const std::string& text) {
        std::string result;
        result.reserve(text.size());
    
        for (auto c : text)
        {
            if (c >= 'a' && c <= 'z') result.push_back(c - 32);
            else result.push_back(c);
        }
    
        return result;
    }

    /**
     * @brief [Static] To Visible
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
// Abdulkadir U. - 21/01/2026
#pragma once

/**
 * Utf
 * 
 * Çoklu karakter setleri arasında dönüşüm işlemlerini sağlar.
 * Genel olarak UTF-8, UTF-32 formatları arasında dönüşümler yapılır.
 */

// Include
#include <string>
#include <sstream>
#include <codecvt>
#include <locale>

// Namespace
namespace tools::charset::utf
{
    // Function Define:
    [[maybe_unused]] [[nodiscard]] std::string to_utf8(const std::u32string&) noexcept;
    [[maybe_unused]] [[nodiscard]] std::u32string to_utf32(const std::string&) noexcept;

    [[maybe_unused]] [[nodiscard]] std::string to_lower(const std::string&) noexcept;
    [[maybe_unused]] [[nodiscard]] std::u32string to_lower(const std::u32string&) noexcept;

    [[maybe_unused]] [[nodiscard]] std::string to_upper(const std::string&) noexcept;
    [[maybe_unused]] [[nodiscard]] std::u32string to_upper(const std::u32string&) noexcept;

    [[maybe_unused]] [[nodiscard]] std::string to_visible(const std::u32string&) noexcept;

    [[maybe_unused]] [[nodiscard]] bool cmp_utf8(const std::string&, const std::string&) noexcept;
    [[maybe_unused]] [[nodiscard]] bool cmp_utf32(const std::u32string&, const std::u32string&) noexcept;

    /**
     * @brief To UTF-8
     * 
     * UTF-32 metini UTF-8 metine çevirme işlemi
     * 
     * @param u32string& Text
     * @return string
     */
    [[maybe_unused]] [[nodiscard]]
    std::string to_utf8(
        const std::u32string& text
    ) noexcept
    {
        std::string out;
        out.reserve(text.size() * sizeof(char32_t));

        for (char32_t c32 : text)
        {
            if (c32 <= 0x7F)
                out.push_back(static_cast<unsigned char>(c32));
            else if (c32 <= 0x7FF)
            {
                out.push_back(static_cast<unsigned char>(0xC0 | (c32 >> 6)));
                out.push_back(static_cast<unsigned char>(0x80 | (c32 & 0x3F)));
            }
            else if (c32 <= 0xFFFF)
            {
                out.push_back(static_cast<unsigned char>(0xE0 | (c32 >> 12)));
                out.push_back(static_cast<unsigned char>(0x80 | ((c32 >> 6) & 0x3F)));
                out.push_back(static_cast<unsigned char>(0x80 | (c32 & 0x3F)));
            }
            else
            {
                out.push_back(static_cast<unsigned char>(0xF0 | (c32 >> 18)));
                out.push_back(static_cast<unsigned char>(0x80 | ((c32 >> 12) & 0x3F)));
                out.push_back(static_cast<unsigned char>(0x80 | ((c32 >> 6) & 0x3F)));
                out.push_back(static_cast<unsigned char>(0x80 | (c32 & 0x3F)));
            }
        }

        return out;
    }

    /**
     * @brief To UTF-32
     * 
     * UTF-8 metini UTF-32 metine çevirme işlemi
     * 
     * @param string& Text
     * @return u32string
     */
    [[maybe_unused]] [[nodiscard]]
    std::u32string to_utf32(const std::string& ar_text) noexcept
    {
        std::u32string tm_out;
        tm_out.reserve(ar_text.size());

        size_t tm_count = 0;
        const size_t tm_n = ar_text.size();

        while (tm_count < tm_n)
        {
            unsigned char tm_c = ar_text[tm_count];

            // 1 byte: 0xxxxxxx
            if (tm_c < 0x80)
            {
                tm_out.push_back(tm_c);
                tm_count++;
                continue;
            }

            // 2 byte: 110xxxxx 10xxxxxx
            if ((tm_c >> 5) == 0x6)
            {
                if (tm_count + 1 >= tm_n) break;

                unsigned char tm_c1 = ar_text[tm_count + 1];
                if ((tm_c1 >> 6) != 0x2) break;

                char32_t tm_code =
                    ((tm_c & 0x1F) << 6) |
                    (tm_c1 & 0x3F);

                tm_out.push_back(tm_code);
                tm_count += 2;
                continue;
            }

            // 3 byte: 1110xxxx 10xxxxxx 10xxxxxx
            if ((tm_c >> 4) == 0xE)
            {
                if (tm_count + 2 >= tm_n) break;

                unsigned char tm_c1 = ar_text[tm_count + 1];
                unsigned char tm_c2 = ar_text[tm_count + 2];
                if ((tm_c1 >> 6) != 0x2 || (tm_c2 >> 6) != 0x2) break;

                char32_t tm_code =
                    ((tm_c & 0x0F) << 12) |
                    ((tm_c1 & 0x3F) << 6) |
                    (tm_c2 & 0x3F);

                tm_out.push_back(tm_code);
                tm_count += 3;
                continue;
            }

            // 4 byte: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            if ((tm_c >> 3) == 0x1E)
            {
                if (tm_count + 3 >= tm_n) break;

                unsigned char tm_c1 = ar_text[tm_count + 1];
                unsigned char tm_c2 = ar_text[tm_count + 2];
                unsigned char tm_c3 = ar_text[tm_count + 3];
                if ((tm_c1 >> 6) != 0x2 || (tm_c2 >> 6) != 0x2 || (tm_c3 >> 6) != 0x2) break;

                char32_t tm_code =
                    ((tm_c & 0x07) << 18) |
                    ((tm_c1 & 0x3F) << 12) |
                    ((tm_c2 & 0x3F) << 6) |
                    (tm_c3 & 0x3F);

                tm_out.push_back(tm_code);
                tm_count += 4;
                continue;
            }

            tm_count++;
        }

        return tm_out;
    }

    /**
     * @brief To Lower
     * 
     * 1 bayt (8 bit) karakter kümesindeki karakterleri
     * küçük harfe çevirmek için var
     * 
     * @param string& Text
     * @return string&
     */
    [[maybe_unused]] [[nodiscard]]
    std::string to_lower(const std::string& ar_text) noexcept
    {
        std::string tm_result;
        for (auto tm_ch : ar_text)
        {
            if (tm_ch >= 'A' && tm_ch <= 'Z') tm_result += tm_ch + ('a' - 'A');
            else tm_result += tm_ch;
        }
        return tm_result;
    }

    /**
     * @brief To Lower
     * 
     * 4 bayt (32 bit) karakter kümesindeki karakterleri
     * küçük harfe çevirmek için var
     * 
     * @param ustring32& Text
     * @return ustring32&
     */
    [[maybe_unused]] [[nodiscard]]
    std::u32string to_lower(const std::u32string& ar_text) noexcept
    {
        std::u32string tm_result;
        for (auto tm_ch : ar_text) {
            if (tm_ch >= U'A' && tm_ch <= U'Z')
                tm_result += tm_ch + (U'a' - U'A');
            else
                tm_result += tm_ch;
        }
        return tm_result;
    }

    /**
     * @brief To Upper
     * 
     * 1 bayt (8 bit) karakter kümesindeki karakterleri
     * büyük harfe çevirmek için var
     * 
     * @param string& Text
     * @return string&
     */
    [[maybe_unused]] [[nodiscard]]
    std::string to_upper(const std::string& ar_text) noexcept
    {
        std::string tm_result;
        tm_result.reserve(ar_text.size());
    
        for (auto tm_c : ar_text)
        {
            if (tm_c >= 'a' && tm_c <= 'z') tm_result.push_back(tm_c - 32);
            else tm_result.push_back(tm_c);
        }
    
        return tm_result;
    }

    /**
     * @brief To Upper
     * 
     * 4 bayt (32 bit) karakter kümesindeki karakterleri
     * büyük harfe çevirmek için var
     * 
     * @param string& Text
     * @return string&
     */
    [[maybe_unused]] [[nodiscard]]
    std::u32string to_upper(const std::u32string& ar_text) noexcept
    {
        std::u32string tm_result;
        tm_result.reserve(ar_text.size());

        for (auto tm_c : ar_text)
        {
            if (tm_c >= U'a' && tm_c <= U'z') tm_result.push_back(tm_c - 32);
            else tm_result.push_back(tm_c);
        }

        return tm_result;
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
    [[maybe_unused]] [[nodiscard]]
    std::string to_visible(const std::u32string& ar_text) noexcept
    {
        std::string tm_result;
        for (auto tm_ch : ar_text) {
            if (tm_ch >= 32 && tm_ch <= 126) // ASCII
                tm_result += static_cast<unsigned char>(tm_ch);
            else {
                std::ostringstream tm_oss;
                tm_oss << "\\x" << std::hex << static_cast<int>(tm_ch);
                tm_result += tm_oss.str();
            }
        }
        return tm_result;
    }

    /**
     * @brief Compare UTF-8
     * 
     * İki adet utf-8 metin alır ve bu metinleri ilk önce
     * boyutuna göre karşılaştırır. Boyutları uyuşmazsa eğer
     * metinler aynı olamaz. Sonrasında döngü ile karakter karakter
     * xor işlemine tabi tutar. Xor işlemine göre eğer iki karakter de
     * aynı ise 0, farklı ise 1 olur ve işlem sonucunda eğer
     * 1 oluşmuş ise hata döndürsün, oluşmamış ise aynılardır ve
     * bu yüzden doğru döndürsün
     * 
     * @param string& First
     * @param string& Second
     * @return bool
     */
    [[maybe_unused]]
    bool cmp_utf8(const std::string& ar_first, const std::string& ar_second) noexcept
    {
        if( ar_first.size() != ar_second.size() )
            return false;

        for( size_t tm_counter = 0; tm_counter < ar_first.size(); ++tm_counter )
            if( ar_first.at(tm_counter) ^ ar_second.at(tm_counter)) return false;

        return true;
    }

    /**
     * @brief Compare UTF-32
     * 
     * İki adet utf-32 metin alır ve bu metinleri ilk önce
     * boyutuna göre karşılaştırır. Boyutları uyuşmazsa eğer
     * metinler aynı olamaz. Sonrasında döngü ile karakter karakter
     * xor işlemine tabi tutar. Xor işlemine göre eğer iki karakter de
     * aynı ise 0, farklı ise 1 olur ve işlem sonucunda eğer
     * 1 oluşmuş ise hata döndürsün, oluşmamış ise aynılardır ve
     * bu yüzden doğru döndürsün
     * 
     * @param u32string& First
     * @param u32string& Second
     * @return bool
     */
    [[maybe_unused]] [[nodiscard]]
    bool cmp_utf32(const std::u32string& ar_first, const std::u32string& ar_second) noexcept
    {
        if( ar_first.size() != ar_second.size() )
            return false;

        for( size_t tm_counter = 0; tm_counter < ar_first.size(); ++tm_counter )
            if( ar_first[tm_counter] ^ ar_second[tm_counter]) return false;

        return true;
    }
}
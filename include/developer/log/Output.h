// Abdulkadir U. - 12/01/2026
#pragma once

/**
 * Output
 * 
 * Oluşan durum hakkında bir başlık ve metin ile
 * insanın okuyabileceği basit bir çıktı vermeyi
 * yöntemini kullanıyoruz ve konsol ekranından
 * bunu okuyarak ne olduğunu daha rahat anlayabileceğiz
 */

// Include:
#include <iostream>
#include <string>

#include <kits/ToolKit.h>
#include <developer/log/Levels.h>

// Namespace:
namespace dev::output
{
    /**
     * @brief Output Title
     * 
     * Lambda ifadelerden faydalanarak basit bir şekilde
     * başlık ayarlayan ve bunu çıktı veren bir ifade
     * Okunurluğu arttırmak için yapıldı
     * 
     * @param string& Title
     * @return string
     */
    auto output_title = [](const std::string& title){
        return std::string("[ " + title + " ] ");
    };

    // Struct:
    struct Console
    {
        /**
         * @brief Write
         * 
         * Duruma göre çıktı verecek ve bu sayede
         * ne olduğunun anlaşılması daha rahat olacak
         * 
         * @note Durumda çıktı verilecek çünkü ne olduğunu anlamalıyız
         * 
         * @param Level Status
         * @param string_view Message
         */
        inline static void write(
            dev::level::Level lvl,
            const std::string& msg
        ) noexcept
        {
            const size_t arr_size = dev::level::to_index(lvl);
            arr_size < dev::level::tests.size() ? ++dev::level::tests[arr_size] : ++dev::level::tests[dev::level::to_index(dev::level::Level::Null)];

            tools::console::set_color(tools::console::get_color(dev::level::to_index(lvl)));
            std::cout << output_title(level::to_string(lvl)) << msg << '\n';
            tools::console::reset_color();
        }

        /**
         * @brief Write
         * 
         * Çıktı verecek ve bu sayede
         * ne olduğunun anlaşılması daha rahat olacak
         * 
         * @param string& Message
         */
        inline static void write(
            const std::string& title,
            const std::string& msg
        ) noexcept
        {
            tools::console::reset_color();
            std::cout << output_title(title) << msg << '\n';
        }

        /**
         * @brief Write
         * 
         * Çıktı verecek ve bu sayede
         * ne olduğunun anlaşılması daha rahat olacak
         * 
         * @param string& Message
         */
        inline static void write(
            const std::string& msg
        ) noexcept
        {
            tools::console::reset_color();
            std::cout << msg << '\n';
        }
    };
}
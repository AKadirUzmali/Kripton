// Abdulkadir U. - 11/01/2026
#pragma once

/**
 * Sinks
 * 
 * Oluşan durum hakkında bir başlık ve metin ile
 * insanın okuyabileceği basit bir çıktı vermeyi
 * yöntemini kullanıyoruz ve konsol ekranından
 * bunu okuyarak ne olduğunu daha rahat anlayabileceğiz
 */

// Include:
#include <iostream>
#include <string_view>

#include <devel/log/Levels.h>

// Namespace:
namespace devel::log
{
    // Struct:
    struct ConsoleSink
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
        static void write(
            Level lvl,
            std::string_view msg
        )
        {
            std::cout << "[ " << level::to_string(lvl) << " ] " << msg << '\n';
        }

        /**
         * @brief Write
         * 
         * Çıktı verecek ve bu sayede
         * ne olduğunun anlaşılması daha rahat olacak
         * 
         * @param string_view Message
         */
        static void write(
            std::string_view msg
        )
        {
            std::cout << msg << '\n';
        }
    };
}
// Abdulkadir U. - 12/01/2026
#pragma once

/**
 * Source
 * 
 * Test sırasında oluşan durumun hangi dosya, satır
 * ve fonksiyon da oluştuğunu anlayabilmek için
 * gerekli yapı. Hem daha kolay hata ayıklama hem de
 * daha okunabilir bir yapı sağlar
 */

// Include:
#include <string_view>

// Namespace:
namespace dev::source
{
    // Struct:
    struct Source
    {
        std::string_view file;
        std::string_view func;
        size_t line;
    };
}
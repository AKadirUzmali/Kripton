// Abdulkadir U. - 12/01/2026
// Abdulkadir U. - 21/01/2026
#pragma once

/**
 * Source (Kaynak)
 * 
 * Test sırasında oluşan durumun hangi dosya, satır
 * ve fonksiyon da oluştuğunu anlayabilmek için
 * gerekli yapı. Hem daha kolay hata ayıklama hem de
 * daha okunabilir bir yapı sağlar
 */

// Include
#include <cstdint>

// Namespace
namespace dev::source
{
    // Struct
    struct Source
    {
        const char* const m_file;
        const char* const m_func;
        std::size_t m_line;
    };
}
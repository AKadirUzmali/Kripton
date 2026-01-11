// Abdulkadir U. - 11/01/2026
#pragma once

/**
 * Event
 * 
 * Yapılan işlemler sırasında olayın kategorisini
 * ve dosya adı, fonksiyonu ve satır bilgisini öğrenmeyi
 * kolaylaştırmak için diğer sistemlerle birlikte
 * çalışan bir olay yapısı
 */

// Include:
#include <devel/core/Category.h>
#include <devel/core/Source.h>

// Namespace:
namespace devel
{
    // Struct:
    struct Event
    {
        Category category;
        Source source;
    };
}
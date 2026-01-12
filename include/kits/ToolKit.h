// Abdulkadir U. - 24/12/2025
// Abdulkadir U. - 12/01/2026
#pragma once

/**
 * ToolKit
 * 
 * Tüm dosyalarda kullanma ihtiyacı doğabilecek
 * şeyler için gerekli olan sınıf, araç sınıfı
 */

// Include:
#include <tools/Platform.h>
#include <tools/Utf.h>
#include <tools/Status.h>
#include <tools/Console.h>

// Namespace:
namespace kits::toolkit
{
    // Using Namespace:
    using namespace tools::status;

    // Enum:
    enum class Status : status_t
    {
        unknwn = 0,
        file = 10000,
        logger = file + static_cast<status_t>(StatusLevel::Null),
        crypto = 20000,
        socket = 30000,
        server = socket + static_cast<status_t>(StatusLevel::Null),
        client = server + static_cast<status_t>(StatusLevel::Null),
        policy = 40000
    };
}
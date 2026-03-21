// Abdulkadir U. - 2026/03/21
#pragma once

/**
 * Debugger (Hata Ayıklayıcı)
 * 
 * Daha çok Derleme zamanı çalışması için gerekli olan
 * hata ayıklama yöntemlerini içermesi için tasarlanmıştır.
 * Derleme sonrası ana sürüme yük oluşturmaması için vardır
 */

// Include
#include <core/buildtype.hpp>

// Using Namespace
using namespace core::buildtype;

// Build Type
#if defined __BUILD_DEBUG__ || defined __BUILD_DEV__
    #define DEBUG_ONLY(x) x
#else
    #define DEBUG_ONLY(x)
#endif
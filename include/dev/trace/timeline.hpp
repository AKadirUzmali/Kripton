// Abdulkadir U. - 12/01/2026
// Abdulkadir U. - 21/01/2026
#pragma once

/**
 * Timeline (Zaman Çizelgesi)
 * 
 * Geçen zaman ile ilgili işlemleri daha kolay
 * yapabilmek için basit bir kısaltma yapısı
 */

// Include
#include <chrono>

// Namespace
namespace dev::trace::timeline
{
    using clock = std::chrono::steady_clock;
    using time_point = clock::time_point;
}
// Abdulkadir U. - 11/01/2026
#pragma once

/**
 * Timeline
 * 
 * Geçen zaman ile ilgili işlemleri daha kolay
 * yapabilmek için basit bir kısaltma yapısı
 */

// Include:
#include <chrono>

// Namespace:
namespace devel::trace
{
    using clock = std::chrono::steady_clock;
    using time_point = clock::time_point;
}
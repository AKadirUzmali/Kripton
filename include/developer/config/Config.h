// Abdulkadir U. - 12/01/2026
#pragma once

/**
 * Config
 * 
 * Özellik bazlı açma / kapama
 * Derleme zamanı sabitlerindendir
 */

// Include:
#include <developer/config/Build.h>

// Namespace:
namespace dev::config
{
    inline constexpr bool logger = dev::build::enabled;
    inline constexpr bool trace = dev::build::enabled;
    inline constexpr bool test = dev::build::enabled;
}
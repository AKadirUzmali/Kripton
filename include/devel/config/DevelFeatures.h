// Abdulkadir U. - 11/01/2026
#pragma once

/**
 * Devel Features
 * 
 * Özellik bazlı açma / kapama
 * Derleme zamanı sabitlerindendir
 */

// Include:
#include <devel/config/DevelBuild.h>

// Namespace:
namespace devel::features
{
    inline constexpr bool logger = devel::enabled;
    inline constexpr bool trace = devel::enabled;
    inline constexpr bool test = devel::enabled;
}
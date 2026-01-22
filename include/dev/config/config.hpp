// Abdulkadir U. - 12/01/2026
// Abdulkadir U. - 21/01/2026
#pragma once

/**
 * Config (Yapılandırma)
 * 
 * Özellik bazlı açma / kapama
 * Derleme zamanı sabitlerindendir
 */

// Include
#include <dev/config/build.hpp>

// Namespace
namespace dev::config
{
    static constexpr bool logger = dev::build::enabled;
    static constexpr bool trace = dev::build::enabled;
    static constexpr bool test = dev::build::enabled;
}
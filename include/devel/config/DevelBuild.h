// Abdulkadir U. - 11/01/2026
#pragma once

/**
 * Devel Build
 * 
 * Build türünü tek merkezden belirler
 * Release modda developer sistemi tamamen yok olur
 */

// Define:
#ifndef __DEVELOPER__
    #if defined(NDEBUG) || defined(__DEVEL__) || defined(__DEBUG__)
        #define __DEVELOPER__ 1
    #else
        #define __DEVELOPER__ 0
    #endif
#endif

// Namespace:
namespace devel
{
    inline constexpr bool enabled = __DEVELOPER__;
}
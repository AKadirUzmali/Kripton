// Abdulkadir U. - 12/01/2026
// Abdulkadir U. - 21/01/2026
#pragma once

/**
 * Build (Derleme)
 * 
 * Build türünü tek merkezden belirler
 * Release modda developer sistemi tamamen yok olur
 */

// Define
#ifndef __DEVELOPER__
    #if defined(__DEVEL__) || defined(__DEBUG__)
        #define __DEVELOPER__ 1
    #else
        #define __DEVELOPER__ 0
    #endif
#endif

// Namespace
namespace dev::build
{
    inline constexpr bool enabled = __DEVELOPER__;
}
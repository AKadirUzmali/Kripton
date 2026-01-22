// Abdulkadir U. - 05/11/2025
#pragma once

/**
 * Algorithm Pool (Algoritma Havuzu)
 * 
 * Birden çok şifreleme yönteminin tutulup
 * kullanılabileceği bir şifreleme havuzu
 */

// Include:
#include <Algorithm/Algorithm.h>
#include <Algorithm/Xor/Xor.h>
#include <Algorithm/Caesar/Caesar.h>

// Namespace: Core
namespace core
{
    // Namespace: Algorithm Pool
    namespace algorithmpool
    {
        using Xor = core::crypt::algorithm::Xor;
        using Caesar = core::crypt::algorithm::Caesar;
    }
}
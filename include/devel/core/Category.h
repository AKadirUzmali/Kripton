// Abdulkadir U. - 11/01/2026
#pragma once

/**
 * Category
 * 
 * Durumları kategoriye ayırarak daha rahat
 * bir şekilde ayrım yapılmasını ve durumun
 * anlaşılmasını sağlamak
 */

// Namespace:
namespace devel
{
    // Enum:
    enum class Category : size_t
    {
        Log,
        Trace,
        Test,
        Contract,
        Internal
    };
}
// Abdulkadir U. - 12/01/2026
#pragma once

/**
 * Status
 * 
 * Durum kodları ile işlem yapmak için kullanacağımız
 * basit bir yapı. Daha çok ToolKit içinde bulunduğunda
 * anlamlı olacak.
 */

// Include:
#include <cstdint>

// Namespace:
namespace tools::status
{
    // Using Type:
    using status_t = std::size_t;

    // Enum:
    enum class StatusLevel : status_t
    {
        Unkwn   = 0,
        Err     = 0001,
        Succ    = 1000,
        Warn    = 2000,
        Info    = 3000,
        Null    = 4000
    };

    /**
     * @brief To Status
     * 
     * Verilen türün dönüşümünü yapacak
     * Verilen türdeki veriyi status_t tipine dönüştürecek
     * 
     * @note En = Enum
     * 
     * @tparam En Value
     * @return status_t
     */
    template<typename En>
    constexpr status_t to_status(En _value) noexcept {
        return static_cast<status_t>(_value);
    }

    /**
     * @brief To Status
     * 
     * İstenilen türe dönüşümünü yapacak
     * status_t tipindeki veriyi istenilen türe dönüştürecek
     * 
     * @note En = Enum
     * 
     * @param status_t Value
     * @return E
     */
    template<typename En>
    constexpr En to_status(status_t _value) noexcept {
        return static_cast<En>(_value);
    }
}
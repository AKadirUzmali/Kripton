// Abdulkadir U. - 24/12/2025
#pragma once

/**
 * Global (Küresel)
 * 
 * Tüm dosyalarda kullanma ihtiyacı doğabilecek
 * şeyler için gerekli olan sınıf
 */

// Include:
#include <Platform/Platform.h>
#include <Tool/Utf/Utf.h>

// Namespace: glo (Global)
namespace glo
{
    // Using Type:
    using status_t = size_t;

    // Enum Class: Status Code
    enum class e_status : status_t
    {
        unkwn = 0,
        err = 0001,
        succ = 1000,
        warn = 2000,
        info = 3000,
        _end = 4000
    };

    // Enum Class: Status Type
    enum class e_status_t : status_t
    {
        unknwn = 0,
        file = 10000,
        logger = file + static_cast<status_t>(e_status::_end),
        crypto = 20000,
        socket = 30000,
        server = socket + static_cast<status_t>(e_status::_end),
        client = server + static_cast<status_t>(e_status::_end)
    };

    // Operator:
    constexpr bool operator==(status_t _first, e_status_t _second) noexcept { return _first == static_cast<status_t>(_second); }
    constexpr bool operator!=(status_t _first, e_status_t _second) noexcept { return _first != static_cast<status_t>(_second); }

    /**
     * @brief [Static] To Status
     * 
     * Verilen türün dönüşümünü yapacak
     * 
     * @tparam _Etype Value
     * @return status_t
     */
    template<typename _Etype>
    constexpr status_t to_status(_Etype _value) noexcept {
        return static_cast<status_t>(_value);
    }

    /**
     * @brief [Static] To Status
     * 
     * İstenilen türe dönüşümünü yapacak
     * 
     * @param status_t Value
     * @return _Etype
     */
    template<typename _Etype>
    constexpr _Etype to_status(status_t _value) noexcept {
        return static_cast<_Etype>(_value);
    }
}
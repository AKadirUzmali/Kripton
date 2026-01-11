// Abdulkadir U. - 11/01/2026
#pragma once

/**
 * Levels
 * 
 * Test durumlarında oluşabilecek durumlar vardır:
 * 
 * 1) Hata
 * 2) Başarı
 * 3) Uyarı
 * 4) Bilgilendirme
 * 5) Hata Ayıklama
 * 
 * Bu durumları bulunduraraktan test işlemlerinde
 * geliştiriciye durumun ne olduğunu anlatabiliyoruz
 */

// Include:
#include <string_view>
#include <array>
#include <atomic>

// Namespace:
namespace devel::log
{
    // Enum:
    enum class Level : size_t
    {
        Err = 0,
        Succ,
        Warn,
        Info,
        Debug,
        Null
    };

    // Array:
    constexpr std::array<std::string_view, static_cast<size_t>(Level::Null) + 1>
    names = {
        "FAIL", "PASS", "WARN", "INFO", "TEST", "NULL"
    };

    // Array:
    constexpr std::array<std::string_view, static_cast<size_t>(Level::Null) + 1>
    status_names = {
        "Error", "Success", "Warning", "Information", "Debug", "Null"
    };

    // Array:
    std::array<std::atomic<size_t>, static_cast<size_t>(Level::Null) + 1>
    tests = { 0, 0, 0, 0, 0, 0 };
}

// Namespace:
namespace devel::level
{
    // Using Namespace:
    using namespace log;

    /**
     * @brief To Index
     * 
     * Durum seviyesini index numarasına çevirir
     * 
     * @param Level Status
     * @return size_t
     */
    constexpr size_t to_index(
        Level lvl
    ) noexcept
    {
        return static_cast<size_t>(static_cast<std::underlying_type_t<Level>>(lvl));
    }

    /**
     * @brief To String
     * 
     * Oluşmuş olan durumu bir metine çevirerek
     * çalışma zamanında döndürecek ve bellek alanı
     * kaplamadan bunu yapacak. Önceden hazırladığım
     * liste sayesinde ise, basit bir kontrol ile
     * herhangi bir şeye ihtiyaç duymadan metini döndürecek
     * 
     * @param Level Status
     * @return string_view
     */
    constexpr std::string_view to_string(
        Level lvl
    ) noexcept
    {
        const size_t arr_size = to_index(lvl);
        return arr_size < names.size() ? names[arr_size] : names[to_index(Level::Null)];
    }

    /**
     * @brief To String
     * 
     * Oluşmuş olan durumu bir metine çevirerek
     * çalışma zamanında döndürecek ve bellek alanı
     * kaplamadan bunu yapacak. Önceden hazırladığım
     * liste sayesinde ise, basit bir kontrol ile
     * herhangi bir şeye ihtiyaç duymadan metini döndürecek
     * 
     * @param size_t Index
     * @return string_view
     */
    constexpr std::string_view to_string(
        size_t index
    ) noexcept
    {
        return index < status_names.size() ? status_names[index] : status_names[to_index(Level::Null)];
    }
}
// Abdulkadir U. - 12/01/2026
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
#include <string>
#include <array>
#include <atomic>

// Namespace:
namespace dev::level
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

    inline constexpr size_t size_levels = static_cast<size_t>(Level::Null) + 1;

    // Array:
    const std::array<std::string, size_levels>
    levels = { "Fail", "Pass", "Warn", "Info", "Test", "Null" };

    // Array:
    const std::array<std::string, size_levels>
    names = { "Error", "Success", "Warning", "Information", "Debug", "Null" };

    // Array:
    std::array<std::atomic<size_t>, size_levels>
    tests = { 0, 0, 0, 0, 0, 0 };

    // Function Define:
    inline constexpr size_t get_valid_index(size_t) noexcept;
    inline constexpr size_t to_index(Level) noexcept;

    inline const std::string& to_string(Level) noexcept;
    inline const std::string& to_string(size_t) noexcept;

    /**
     * @brief Get Valid Index
     * 
     * Verilen index numarasının geçerli bir durum
     * seviyesine ait olup olmadığını test eder ve
     * geçerli ise direk, değilse de geçerli bir index
     * numarası döndürür
     * 
     * @param size_t Index
     * @return size_t
     */
    inline constexpr size_t get_valid_index(
        size_t index
    ) noexcept
    {
        return index < size_levels ? index : size_levels - 1;
    }

    /**
     * @brief To Index
     * 
     * Durum seviyesini index numarasına çevirir
     * 
     * @param Level Status
     * @return size_t
     */
    inline constexpr size_t to_index(
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
     * @return const string&
     */
    inline const std::string& to_string(
        Level lvl
    ) noexcept
    {
        const size_t arr_size = to_index(lvl);
        return arr_size < levels.size() ? levels[arr_size] : levels[to_index(Level::Null)];
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
     * @return const string&
     */
    inline const std::string& to_string(
        size_t index
    ) noexcept
    {
        return index < names.size() ? names[index] : names[to_index(Level::Null)];
    }
}
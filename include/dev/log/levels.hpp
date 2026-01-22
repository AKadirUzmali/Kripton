// Abdulkadir U. - 12/01/2026
// Abdulkadir U. - 21/01/2026
#pragma once

/**
 * Levels (Seviyeler)
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

// Include
#include <cstdint>
#include <string>
#include <array>
#include <atomic>

// Namespace
namespace dev::level
{
    // Enum
    enum class level_t: std::uint8_t
    {
        Err = 0,
        Succ,
        Warn,
        Info,
        Debug,
        Null
    };

    // Array
    static constexpr const char* const ss_levels[] =
    { "Fail", "Pass", "Warn", "Info", "Test", "Null" };

    static constexpr const char* const ss_names[] =
    { "Error", "Success", "Warning", "Information", "Debug", "Null" };

    static constexpr std::size_t ss_size_levels = sizeof(ss_levels) / sizeof(ss_levels[0]);
    static constexpr std::size_t ss_size_names = sizeof(ss_names) / sizeof(ss_names[0]);
    static constexpr std::size_t ss_size_tests = ss_size_levels;

    // Array:
    static std::atomic<std::size_t> ss_tests[ss_size_tests] =
    { 0, 0, 0, 0, 0, 0 };

    // Function Define:
    inline constexpr size_t get_valid_index(const size_t) noexcept;
    inline constexpr size_t to_index(const level_t) noexcept;

    inline constexpr const char* to_string(const level_t) noexcept;
    inline constexpr const char* to_string(const size_t) noexcept;

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
        const size_t ar_index
    ) noexcept
    {
        return ar_index < ss_size_levels ? ar_index : ss_size_levels - 1;
    }

    /**
     * @brief To Index
     * 
     * Durum seviyesini index numarasına çevirir
     * 
     * @param level_t Status
     * @return size_t
     */
    inline constexpr size_t to_index(
        level_t ar_lvl
    ) noexcept
    {
        return static_cast<size_t>(ar_lvl);
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
     * @param level_t Status
     * @return const char*
     */
    inline constexpr const char* to_string(level_t ar_lvl) noexcept
    {
        return ss_levels[get_valid_index(to_index(ar_lvl))];
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
     * @return const char*
     */
    inline constexpr const char* to_string(
        const size_t index
    ) noexcept
    {
        return ss_names[get_valid_index(index)];
    }
}
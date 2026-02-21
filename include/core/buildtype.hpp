// Abdulkadir U. - 23/01/2026
#pragma once

/**
 * Build Type (Derleme Türü)
 * 
 * Program da birçok derleme türü bulunmakta
 * ve bunlardan hangisinin yapıldığını ayırt
 * etmek için eklediğimiz bir yapıdır.
 * Şuanlık için ön görülen derleme türleri şunlardır:
 * 
 * 1) Dev (Geliştirme)
 * 2) Debug (Hata Testi)
 * 3) Alpha (Ön Test)
 * 4) Beta (Son Test)
 * 5) Release (Son Kullanıcı)
 * 6) None (Boş ya da Yok)
 */

// Include:
#include <cstdint>
#include <atomic>

// Namespace
namespace core::buildtype
{
    // Enum
    enum class build_t : std::uint8_t
    {
        Dev = 0,
        Debug,
        Alpha,
        Beta,
        Release,
        None
    };

    // Function Define
    [[maybe_unused]] [[nodiscard]] inline constexpr std::uint8_t get_valid_index(const std::uint8_t ar_index) noexcept;
    [[maybe_unused]] [[nodiscard]] inline constexpr std::uint8_t to_index(const build_t ar_type) noexcept;

    [[maybe_unused]] [[nodiscard]] inline constexpr const char* to_string(const build_t ar_type) noexcept;
    [[maybe_unused]] [[nodiscard]] inline constexpr const char* to_string(const std::uint8_t ar_index) noexcept;

    // Struct
    struct Build
    {
        private:
            static constexpr const char* const s_names[] =
            { "Dev", "Debug", "Alpha", "Beta", "Release", "None" };

            static constexpr std::uint8_t s_size_names = sizeof(s_names) / sizeof(s_names[0]);

        private:
            build_t m_buildtype;

        private:
            [[maybe_unused]] [[nodiscard]] static inline constexpr std::uint8_t to_index(const build_t ar_buildtype) noexcept;
            [[maybe_unused]] [[nodiscard]] static inline constexpr std::uint8_t get_valid_index(const std::uint8_t ar_idx) noexcept;

        public:
            constexpr explicit Build(const build_t ar_buildtype) noexcept;

            constexpr const char* c_str() const noexcept;
    };

    /**
     * @brief Build
     * 
     * Verilmiş olan tür argümanına göre türü ayarlanır
     * ve kontrolden geçirilmiş tür numarası ile uygun olan
     * tür isimi verilir
     * 
     * @note Thread açısından güvensiz fakat amaç 1 defa yap olduğu için bunu sorun etmiyoruz
     * 
     * @param build_t Build Type
     */
    [[maybe_unused]] [[nodiscard]]
    constexpr Build::Build(
        const build_t ar_buildtype
    ) noexcept
    : m_buildtype(ar_buildtype)
    {}

    /**
     * @brief To Index
     * 
     * Verilen derleme türünü alıp index numarasının türüne
     * çevirir ve bu sayede geçerli veya geçersiz bir index
     * numarası elde etmiş oluruz ama aynı türde bir değer olur
     * 
     * @note Verdiği değer derleme türü değerlerinden büyük saçma bir değer olabilir
     * @note Bu yüksek sayı durumunu engellemek için ise get_valid_index fonksiyonu kullanılır
     * 
     * @param build_t Type
     * @return uint8_t
     */
    [[maybe_unused]] [[nodiscard]]
    constexpr std::uint8_t Build::to_index(
        const build_t ar_buildtype
    ) noexcept
    {
        return static_cast<std::uint8_t>(ar_buildtype);
    }

    /**
     * @brief Get Valid Index
     * 
     * Verilen index numarasının geçerli olup olmadığını
     * kontrol ederek geçerli bir index numarası verir
     * 
     * @param uint8_t Index
     * @return uint8_t
     */
    constexpr std::uint8_t Build::get_valid_index(
        const std::uint8_t ar_idx
    ) noexcept {
        return (ar_idx < Build::s_size_names) ? ar_idx : (Build::s_size_names - 1);
    }

    /**
     * @brief C Str
     * 
     * Verilen derleme türünün temsil ettiği metini döndürerek
     * insan tarafından da okunabilir kılar
     * 
     * @return const char*
     */
    constexpr const char* Build::c_str() const noexcept
    {
        return Build::s_names[Build::get_valid_index(Build::to_index(this->m_buildtype))];
    }

    // Define
    #ifndef __BUILD_TYPE__
        #define __BUILD_TYPE__ build_t::None
    #endif
}
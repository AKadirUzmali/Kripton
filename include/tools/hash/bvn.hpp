// Abdulkadir U. - 21/01/2026
#pragma once

/**
 * Basic Version Number (BVN)
 * 
 * Basit bir şekilde sürüm numarasına göre (major, minor, same, patch)
 * kontrol sağlanması için gerekli şifrelemeyi oluşturacak yapıdır 
 * ve bu sayede sürüm kontrolü işlemi sağlanmış olur.
 * Bu kontrol ile bazı yapıların sürümlerinin aynı olması
 * gerektiği durumlar (socket) rahatlıkla sürüm numarası
 * ile karşılaştırılıp işleme devam edilip edilmeyeceği belirlenir.
 * Bu sayede sürüm farklılıklarından oluşabilecek uyumsuzlukları
 * da engellemiş olur ama istendiği durumda aynı sürüm durumu
 * geçersiz bırakılmış ise, kaç sürüm farkı olursa olsun
 * uyumlu şekilde çalıştığı garanti edilmiş olur ve sürüm
 * farklılığına rağmen sistem çalışmaya devam etmiş olur fakat,
 * olası bir sorun oluştuğunda bu anlaşılmayabilir ve programın
 * ani bir şekilde sonlanmasına ya da bellek açıklarına sebep
 * olabilir!
 */

// Include
#include <core/version.hpp>

// Namespace
namespace tools::hash::bvn
{
    // Using Namespace
    using namespace core::version;

    // Struct
    struct Bvn
    {
        private:
            version_t m_version;

        public:
            constexpr explicit Bvn(
                const std::uint16_t ar_major,
                const std::uint16_t ar_minor,
                const std::uint32_t ar_patch,
                const bool ar_compatible
            ) noexcept
            : m_version(ar_major, ar_minor, ar_patch, ar_compatible)
            {}

            [[maybe_unused]] [[nodiscard]]
            constexpr bool operator==(const Bvn& other) const noexcept
            { return m_version == other.m_version; }

            [[maybe_unused]] [[nodiscard]]
            constexpr bool operator!=(const Bvn& other) const noexcept
            { return m_version != other.m_version; }
    };
}
// Abdulkadir U. - 21/01/2026
#pragma once

/**
 * Status (Durum)
 * 
 * Programda birden çok yapı çalışmakta ve bunlar işlem yaparken
 * belirli bir cevap döndürmekte. Döndürülen cevabı yorumlayabilmek
 * için oluşturulan durum sınıfı yapısı bu ihtiyacı gidermektedir.
 */

// Include
#include <cstdint>
#include <type_traits>

// Namespace
namespace core::status
{
    // Enum
    enum class status_t : std::uint8_t
    {
        none = 0,
        err,
        ok,
        warn,
        info
    };

    // Enum
    enum class domain_t : std::uint8_t
    {
        none = 0,
        socket,
        server,
        client,
        policy
    };

    /**
     * @brief To Underlying
     * 
     * Enum veriyi otomatik olarak tamsayı
     * türüne dönüştürmeyi sağlar
     * 
     * @param E Enum
     * @return auto
     */
    template<typename E>
    constexpr auto to_underlying(
        E ar_e
    ) noexcept
    {
        return static_cast<std::underlying_type_t<E>>(ar_e);
    }

    // Struct
    struct Status
    {
        private:
            status_t m_status;
            domain_t m_domain;
            uint16_t m_code;

        public:
            constexpr Status(
                const status_t ar_status,
                const domain_t ar_domain,
                const uint16_t ar_code
            ) noexcept;

            constexpr inline status_t get_status() const noexcept { return m_status; }
            constexpr inline domain_t get_domain() const noexcept { return m_domain; }
            constexpr inline uint16_t get_code() const noexcept { return m_code; }

            constexpr bool is_none()   const noexcept { return this->m_status == status_t::none; }
            constexpr bool is_err()    const noexcept { return this->m_status == status_t::err; }
            constexpr bool is_ok()     const noexcept { return this->m_status == status_t::ok; }
            constexpr bool is_warn()   const noexcept { return this->m_status == status_t::warn; }
            constexpr bool is_info()   const noexcept { return this->m_status == status_t::info; }

            static constexpr Status ok(domain_t ar_domain = domain_t::none, uint16_t ar_code = 0) { return Status{ status_t::ok, ar_domain, ar_code }; }
            static constexpr Status err(domain_t ar_domain = domain_t::none, uint16_t ar_code = 0) { return Status{ status_t::err, ar_domain, ar_code }; }
            static constexpr Status warn(domain_t ar_domain = domain_t::none, uint16_t ar_code = 0) { return Status{ status_t::warn, ar_domain, ar_code }; }
            static constexpr Status info(domain_t ar_domain = domain_t::none, uint16_t ar_code = 0) { return Status{ status_t::info, ar_domain, ar_code }; }

            constexpr bool operator==(const Status& ar_other) noexcept;
            constexpr bool operator!=(const Status& ar_other) noexcept;

            constexpr Status operator()(const status_t ar_status, const domain_t ar_domain, const uint16_t ar_code) noexcept;
    };

    constexpr Status::Status(
        const status_t ar_status,
        const domain_t ar_domain,
        const uint16_t ar_code
    ) noexcept
    : m_status(ar_status), m_domain(ar_domain), m_code(ar_code)
    {}

    constexpr bool Status::operator==(
        const Status& ar_other
    ) noexcept
    {
        return this->m_status == ar_other.m_status &&
            this->m_domain == ar_other.m_domain &&
            this->m_code == ar_other.m_code;
    }

    constexpr bool Status::operator!=(
        const Status& ar_other
    ) noexcept
    {
        return !(*this == ar_other);
    }

    constexpr Status Status::operator()(
        const status_t ar_status,
        const domain_t ar_domain,
        const uint16_t ar_code
    ) noexcept
    {
        return Status{ ar_status, ar_domain, ar_code };
    }
}
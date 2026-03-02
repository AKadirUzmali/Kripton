// Abdulkadir U. - 2026/01/28
#pragma once

/**
 * Access Policy (Erişimi Politikası)
 */

// Include
#include <cstdint>
#include <cstring>
#include <string>
#include <atomic>
#include <mutex>
#include <unordered_set>

#include <core/status.hpp>

// Namespace
namespace netsocket::policy
{
    // Using
    using max_conn_t = std::uint16_t;
    using name_len_t = std::uint8_t;

    // Using Namespace
    using namespace core::status;

    // Limit
    static constexpr max_conn_t _MIN_CONNECTION = 1;
    static constexpr max_conn_t _DEF_CONNECTION = 1024;
    static constexpr max_conn_t _MAX_CONNECTION = 8192;

    static constexpr max_conn_t _MIN_SAME_IP_COUNT = 1;
    static constexpr max_conn_t _DEF_SAME_IP_COUNT = _MIN_SAME_IP_COUNT;
    static constexpr max_conn_t _MAX_SAME_IP_COUNT = 128;

    static constexpr name_len_t _MIN_LEN_PASSWORD = 4;
    static constexpr name_len_t _MAX_LEN_PASSWORD = 128;

    static constexpr name_len_t _MIN_LEN_USERNAME = 2;
    static constexpr name_len_t _MAX_LEN_USERNAME = 32;

    // Enum
    enum class policy_e : uint16_t
    {
        err = 0,
        not_valid_username,
        not_valid_password,
        fail_set_username,
        fail_set_password,
        fail_set_require_password,
        value_under_min,
        value_over_max,
        fail_set_max_connection,
        fail_set_max_same_ip,
        ipaddr_is_empty,
        ipaddr_already_unbanned,
        ipaddr_already_banned,
        fail_set_unban_ipaddr,
        fail_set_ban_ipaddr,
        not_allow_ipaddr_already_banned,
        not_allow_ipaddr_not_in_list,
        cannot_auth_password_not_matched,
        set_not_allow_ipaddr,

        succ = 1000,
        set_username,
        set_password,
        set_require_password,
        set_max_connection,
        set_max_same_ip,
        set_unban_ipaddr,
        set_ban_ipaddr,
        can_allow_ipaddr,
        can_auth_no_password_require,
        can_auth_with_password,
        set_allow_ipaddr,

        warn = 2000,
        same_value,
        cannot_auth_with_old_password,
        ipaddr_already_allowed,

        info = 3000
    };

    // Function Define
    [[maybe_unused]] [[nodiscard]] static constexpr bool is_valid_username(const char* ar_username) noexcept;
    [[maybe_unused]] [[nodiscard]] static constexpr bool is_valid_password(const char* ar_password) noexcept;

    // Class
    class AccessPolicy final
    {
        private:
            std::atomic<bool> m_require_password { false };
            std::string m_password {};
            std::string m_old_password {};
            std::string m_username {};

            std::atomic<max_conn_t> m_max_connection { _DEF_CONNECTION };
            std::atomic<max_conn_t> m_max_same_ip { _DEF_SAME_IP_COUNT };

            std::unordered_set<std::string> m_banned_ip_list;
            std::unordered_set<std::string> m_allowed_ip_list;

            mutable std::mutex m_mtx;
        
        public:
            explicit AccessPolicy(
                const std::string& ar_username = "",
                const std::string& ar_password = ""
            );

            inline bool is_connection_banned(const std::string& ar_ipaddr) noexcept;
            inline bool is_connection_allowed(const std::string& ar_ipaddr) noexcept;
            inline bool is_require_password() const noexcept;
            
            inline const std::string& get_username() const noexcept;
            inline const std::string& get_password() const noexcept;
            inline const std::string& get_old_password() const noexcept;

            inline max_conn_t get_max_connection() const noexcept;
            inline max_conn_t get_max_same_ip() const noexcept;

            Status set_username(const std::string& ar_username) noexcept;
            Status set_password(const std::string& ar_password) noexcept;
            Status set_require_password(const bool ar_require = true) noexcept;
            Status set_max_connection(const max_conn_t ar_max_conn = _DEF_CONNECTION) noexcept;
            Status set_max_same_ip(const max_conn_t ar_max_same_ip = _DEF_SAME_IP_COUNT) noexcept;
            Status set_ban(const std::string& ar_ipaddr, const bool ar_ban = true) noexcept;
            Status set_allow(const std::string& ar_ipaddr) noexcept;

            Status can_allow(const std::string& ar_ipaddr) noexcept;
            Status can_auth(const std::string& ar_ipaddr, const std::string& ar_pwd) noexcept;

    };

    /**
     * @brief AccessPolicy
     * 
     * Kullanıcı adı ve şifre alıp bunları
     * ayarlayarak sınıfı başlatmış olur
     * 
     * @param string& Username
     * @param string& Password
     */
    AccessPolicy::AccessPolicy(
        const std::string& ar_username,
        const std::string& ar_password
    )
    {
        this->set_username(ar_username);
        this->set_password(ar_password);
    }

    /**
     * @brief Is Connection Banned
     * 
     * Verilen ip adresinin engellenmiş olup olmadığını
     * engellenmiş ip adresi listesinden kontrol ederek
     * karar verecek
     * 
     * @return bool
     */
    bool AccessPolicy::is_connection_banned(const std::string& ar_ipaddr) noexcept
    {
        std::scoped_lock tm_lock(this->m_mtx);
        return this->m_banned_ip_list.find(ar_ipaddr) != this->m_banned_ip_list.end();
    }

    /**
     * @brief Is Connection Allowed
     * 
     * Verilen ip adresinin erişime sahip olup olmadığını
     * erişim ip adresi listesinden kontrol ederek
     * karar verecek
     * 
     * @return bool
     */
    bool AccessPolicy::is_connection_allowed(const std::string& ar_ipaddr) noexcept
    {
        std::scoped_lock tm_lock(this->m_mtx);
        if( this->m_allowed_ip_list.empty() )
            return true;

        return this->m_allowed_ip_list.find(ar_ipaddr) != this->m_allowed_ip_list.end();
    }

    /**
     * @brief Is Require Password
     * 
     * Şifre istenmesinin gerekli olup olmadığını döndürecek
     * 
     * @return bool
     */
    bool AccessPolicy::is_require_password() const noexcept
    {
        return this->m_require_password.load();
    }

    /**
     * @brief Get Username
     * 
     * Kullanıcı adını döndürür
     * 
     * @return string&
     */
    const std::string& AccessPolicy::get_username() const noexcept
    {
        return this->m_username;
    }

    /**
     * @brief Get Password
     * 
     * Şuanki şifreyi döndürür
     * 
     * @return string&
     */
    const std::string& AccessPolicy::get_password() const noexcept
    {
        return this->m_password;
    }

    /**
     * @brief Get Old Password
     * 
     * Eski şifreyi döndürür
     * 
     * @return string&
     */
    const std::string& AccessPolicy::get_old_password() const noexcept
    {
        return this->m_old_password;
    }

    /**
     * @brief Get Max Connection
     * 
     * Bağlantı yapılabilecek toplam miktar
     * 
     * @return max_conn_t
     */
    max_conn_t AccessPolicy::get_max_connection() const noexcept
    {
        return this->m_max_connection.load(std::memory_order_relaxed);
    }

    /**
     * @brief Get Max Same Ip
     * 
     * Aynı ip adresinden yapılabilecek en fazla bağlantı miktarı
     * 
     * @return max_conn_t
     */
    max_conn_t AccessPolicy::get_max_same_ip() const noexcept
    {
        return this->m_max_same_ip.load(std::memory_order_relaxed);
    }

    /**
     * @brief Set Username
     * 
     * Kullanıcı adı belirlemeye yarar.
     * Geçerlilik kontrolü sonrası yeni kullanıcı
     * adını ayarlar
     * 
     * @param string& Username
     * @return Status
     */
    Status AccessPolicy::set_username(
        const std::string& ar_username
    ) noexcept
    {
        if( !is_valid_username(ar_username.c_str()) )
            return Status::err(domain_t::policy, to_underlying(policy_e::not_valid_username));

        std::scoped_lock tm_lock(this->m_mtx);
        this->m_username = ar_username;

        return this->m_username == ar_username ?
            Status::ok(domain_t::policy, to_underlying(policy_e::set_username)) :
            Status::err(domain_t::policy, to_underlying(policy_e::fail_set_username));
    }

    /**
     * @brief Set Password
     * 
     * Yeni bir şifre ayarlanmasını sağlar ve
     * verilen şifrenin geçerliliğini kontrol ettikten
     * sonra şifreyi geçici olarak tutar. Yeni şifreyi
     * ayarlar ve tutulan şifreyi eski şifre olarak ayarlar
     * 
     * @param string& Password
     * @return Status
     */
    Status AccessPolicy::set_password(const std::string& ar_password) noexcept
    {
        if( !is_valid_password(ar_password.c_str()) )
            return Status::err(domain_t::policy, to_underlying(policy_e::not_valid_password));

        std::scoped_lock tm_lock(this->m_mtx);

        std::string tm_new_old_password = this->m_password;
        this->m_password = ar_password;

        if( this->m_password != ar_password )
            return Status::err(domain_t::policy, to_underlying(policy_e::fail_set_password));

        this->m_old_password = tm_new_old_password;
        return Status::ok(domain_t::policy, to_underlying(policy_e::set_password));
    }

    /**
     * @brief Set Require Password
     * 
     * Şifre gerekliliğinin ayarlanmasına ya da
     * devre dışı bırakılmasına karar verecek.
     * Aynı değer verilmesi durumunda direk başarı dönecek
     * 
     * @param bool Require
     * @return Status
     */
    Status AccessPolicy::set_require_password(const bool ar_require) noexcept
    {
        if( this->m_require_password.load() == ar_require)
            return Status::warn(domain_t::policy, to_underlying(policy_e::same_value));

        this->m_require_password.store(ar_require);
        return this->m_require_password.load() == ar_require ?
            Status::ok(domain_t::policy, to_underlying(policy_e::set_require_password)) :
            Status::err(domain_t::policy, to_underlying(policy_e::fail_set_require_password));
    }

    /**
     * @brief Set Max Connection
     * 
     * Toplam yapılabilecek bağlantı miktarını ayarlayacak.
     * Belirli sınır kontrolleri ile aşımı önleyecek
     * 
     * @param max_conn_t Max Connection
     * @return Status
     */
    Status AccessPolicy::set_max_connection(const max_conn_t ar_max_conn) noexcept
    {
        if( ar_max_conn < _MIN_CONNECTION ) return Status::err(domain_t::policy, to_underlying(policy_e::value_under_min));
        else if( ar_max_conn > _MAX_CONNECTION ) return Status::err(domain_t::policy, to_underlying(policy_e::value_over_max));
        else if( this->m_max_connection.load(std::memory_order_relaxed) == ar_max_conn ) return Status::warn(domain_t::policy, to_underlying(policy_e::same_value));

        this->m_max_connection.store(ar_max_conn);
        return this->m_max_connection.load(std::memory_order_relaxed) == ar_max_conn ?
            Status::ok(domain_t::policy, to_underlying(policy_e::set_max_connection)) :
            Status::err(domain_t::policy, to_underlying(policy_e::fail_set_max_connection));
    }

    /**
     * @brief Set Max Same IP
     * 
     * Toplam olarak aynı ip adresinden yapılabilecek bağlantı
     * miktarını ayarlayacak. Sınır aşımı kontrolü dahilinde olacak
     * 
     * @param max_conn_t Max Same IP
     * @return Status
     */
    Status AccessPolicy::set_max_same_ip(const max_conn_t ar_max_same_ip) noexcept
    {
        if( ar_max_same_ip < _MIN_SAME_IP_COUNT ) return Status::err(domain_t::policy, to_underlying(policy_e::value_under_min));
        else if( ar_max_same_ip > _MAX_SAME_IP_COUNT ) return Status::err(domain_t::policy, to_underlying(policy_e::value_over_max));
        else if( this->m_max_same_ip.load(std::memory_order_relaxed) == ar_max_same_ip ) return Status::warn(domain_t::policy, to_underlying(policy_e::same_value));

        this->m_max_same_ip.store(ar_max_same_ip);
        return this->m_max_same_ip.load(std::memory_order_relaxed) == ar_max_same_ip ?
            Status::ok(domain_t::policy, to_underlying(policy_e::set_max_same_ip)) :
            Status::err(domain_t::policy, to_underlying(policy_e::fail_set_max_same_ip));
    }

    /**
     * @brief Set Ban
     * 
     * Belirtilen ip adresinin yasaklanmasına ya da yasağın
     * kaldırılmasına karar verecektir
     * 
     * @param string& IP
     * @param bool Ban
     * 
     * @return Status
     */
    Status AccessPolicy::set_ban(
        const std::string& ar_ipaddr,
        const bool ar_ban
    ) noexcept
    {
        if( ar_ipaddr.empty() )
            return Status::err(domain_t::policy, to_underlying(policy_e::ipaddr_is_empty));

        std::scoped_lock tm_lock(this->m_mtx);

        // unban
        if( !ar_ban ) {
                if( this->m_banned_ip_list.find(ar_ipaddr) == this->m_banned_ip_list.end() )
                    return Status::warn(domain_t::policy, to_underlying(policy_e::ipaddr_already_unbanned));

                this->m_banned_ip_list.erase(ar_ipaddr);

            return this->m_banned_ip_list.find(ar_ipaddr) == this->m_banned_ip_list.end() ?
                Status::ok(domain_t::policy, to_underlying(policy_e::set_unban_ipaddr)) :
                Status::err(domain_t::policy, to_underlying(policy_e::fail_set_unban_ipaddr));
        }

        // ban
        if( this->m_banned_ip_list.find(ar_ipaddr) != this->m_banned_ip_list.end() )
            return Status::warn(domain_t::policy, to_underlying(policy_e::ipaddr_already_banned));

        this->m_banned_ip_list.emplace(ar_ipaddr);

        return this->m_banned_ip_list.find(ar_ipaddr) != this->m_banned_ip_list.end() ?
            Status::ok(domain_t::policy, to_underlying(policy_e::set_ban_ipaddr)) :
            Status::err(domain_t::policy, to_underlying(policy_e::fail_set_ban_ipaddr));
    }

    /**
     * @brief Set Allow
     * 
     * Belirtilen ip adresinin erişimesi için izin verilip verilmemesine
     * karar verecektir
     * 
     * @param string& IP
     * @return Status
     */
    Status AccessPolicy::set_allow(const std::string& ar_ipaddr) noexcept
    {
        if( ar_ipaddr.empty() )
            return Status::err(domain_t::policy, to_underlying(policy_e::ipaddr_is_empty));

        std::scoped_lock tm_lock(this->m_mtx);

        if( this->m_allowed_ip_list.find(ar_ipaddr) != this->m_allowed_ip_list.end() )
            return Status::warn(domain_t::policy, to_underlying(policy_e::ipaddr_already_allowed));
        
        this->m_allowed_ip_list.emplace(ar_ipaddr);
        return this->m_allowed_ip_list.find(ar_ipaddr) != this->m_allowed_ip_list.end() ?
            Status::ok(domain_t::policy, to_underlying(policy_e::set_allow_ipaddr)) :
            Status::err(domain_t::policy, to_underlying(policy_e::set_not_allow_ipaddr));
    }

    /**
     * @brief Can Allow
     * 
     * Belirtilen ip adresine erişim izninin verilip verilmediğini kontrol
     * edecek. Eğer yasaklı (banlı) listede yoksa izin verilme listesini
     * kontrol edecek ve eğer orada varsa, izin verilmiş demektir
     * 
     * @param string& IP
     * @return Status
     */
    Status AccessPolicy::can_allow(const std::string& ar_ipaddr) noexcept
    {
        if( ar_ipaddr.empty() )
            return Status::err(domain_t::policy, to_underlying(policy_e::ipaddr_is_empty));

        std::scoped_lock tm_lock(this->m_mtx);

        if( this->m_banned_ip_list.find(ar_ipaddr) != this->m_banned_ip_list.end() )
            return Status::err(domain_t::policy, to_underlying(policy_e::not_allow_ipaddr_already_banned));
        else if( this->m_allowed_ip_list.find(ar_ipaddr) == this->m_allowed_ip_list.end() && this->m_allowed_ip_list.size() > 0 )
            return Status::err(domain_t::policy, to_underlying(policy_e::not_allow_ipaddr_not_in_list));

        return Status::ok(domain_t::policy, to_underlying(policy_e::can_allow_ipaddr));
    }

    /**
     * @brief Can Auth
     * 
     * Belirtilen ip adresi izin verilmişse eğer, şifre kontrolüne tabi tutulur.
     * Şifre istenmiyorsa sorun değil ama aksi halde şifre doğruluğu yapar.
     * Her ikisini de başarıyla geçerse, giriş yapabilir demektir
     * 
     * @param string& IP
     * @return Status
     */
    Status AccessPolicy::can_auth(
        const std::string& ar_ipaddr,
        const std::string& ar_pwd
    ) noexcept
    {
        Status tm_status = this->can_allow(ar_ipaddr);
        if( !tm_status.is_ok() )
            return tm_status;

        if( !this->m_require_password.load() )
            return Status::ok(domain_t::policy, to_underlying(policy_e::can_auth_no_password_require));

        std::scoped_lock tm_lock(this->m_mtx);

        if( ar_pwd == this->m_password )
            return Status::ok(domain_t::policy, to_underlying(policy_e::can_auth_with_password));
        else if( ar_pwd == this->m_old_password )
            return Status::warn(domain_t::policy, to_underlying(policy_e::cannot_auth_with_old_password));

        return Status::err(domain_t::policy, to_underlying(policy_e::cannot_auth_password_not_matched));
    }

    /**
     * @brief Is Valid Username
     * 
     * Verilen isim değerinin geçerli olup olmadığını kontrol eder
     * 
     * @param char* Username
     * @return bool
     */
    [[maybe_unused]] [[nodiscard]]
    constexpr bool is_valid_username(
        const char* ar_username
    ) noexcept
    {
        if( !ar_username )
            return false;
        
        const name_len_t tm_len = static_cast<name_len_t>(std::strlen(ar_username));
        return tm_len >= _MIN_LEN_USERNAME && tm_len <= _MAX_LEN_USERNAME;
    }

    /**
     * @brief Is Valid Password
     * 
     * Verilen metinin geçerli bir şifre olup olmadığını kontrol
     * eder ve bunu derleme zamanında da yapabilir
     * 
     * @param char* Password
     * @return bool
     */
    [[maybe_unused]] [[nodiscard]]
    constexpr bool is_valid_password(
        const char* ar_password
    ) noexcept
    {
        if( !ar_password )
            return false;
        
        const name_len_t tm_len = static_cast<name_len_t>(std::strlen(ar_password));
        return tm_len >= _MIN_LEN_PASSWORD && tm_len <= _MAX_LEN_PASSWORD;
    }
}
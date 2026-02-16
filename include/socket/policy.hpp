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

// Namespace
namespace netsocket::policy
{
    // Using:
    using max_conn_t = std::uint16_t;
    using name_len_t = std::uint8_t;

    // Limit
    static constexpr max_conn_t _MIN_CONNECTION = 1;
    static constexpr max_conn_t _DEF_CONNECTION = 1024;
    static constexpr max_conn_t _MAX_CONNECTION = 8192;

    static constexpr max_conn_t _MIN_SAME_IP_COUNT = 1;
    static constexpr max_conn_t _DEF_SAME_IP_COUNT = _MIN_SAME_IP_COUNT;
    static constexpr max_conn_t _MAX_SAME_IP_COUNT = 2;

    static constexpr name_len_t _MIN_LEN_PASSWORD = 4;
    static constexpr name_len_t _MAX_LEN_PASSWORD = 128;

    static constexpr name_len_t _MIN_LEN_USERNAME = 2;
    static constexpr name_len_t _MAX_LEN_USERNAME = 32;

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

            mutable std::mutex m_mtx;
        
        public:
            explicit AccessPolicy(
                const std::string& ar_username = "",
                const std::string& ar_password = ""
            );

            inline const std::string& get_username() const noexcept;
            bool set_username(const std::string& ar_username) noexcept;

            inline const std::string& get_password() const noexcept;
            inline const std::string& get_old_password() const noexcept;
            bool set_password(const std::string& ar_password) noexcept;

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
     * @brief Set Username
     * 
     * Kullanıcı adı belirlemeye yarar.
     * Geçerlilik kontrolü sonrası yeni kullanıcı
     * adını ayarlar
     * 
     * @param string& Username
     * @return bool
     */
    bool AccessPolicy::set_username(
        const std::string& ar_username
    ) noexcept
    {
        if( !is_valid_username(ar_username.c_str()) )
            return false;

        std::scoped_lock tm_lock(this->m_mtx);
        this->m_username = ar_username;

        return this->m_username == ar_username;
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
     * @brief Set Password
     * 
     * Yeni bir şifre ayarlanmasını sağlar ve
     * verilen şifrenin geçerliliğini kontrol ettikten
     * sonra şifreyi geçici olarak tutar. Yeni şifreyi
     * ayarlar ve tutulan şifreyi eski şifre olarak ayarlar
     * 
     * @param string& Password
     * @return bool
     */
    bool AccessPolicy::set_password(
        const std::string& ar_password
    ) noexcept
    {
        if( !is_valid_password(ar_password.c_str()) )
            return false;

        std::scoped_lock tm_lock(this->m_mtx);

        std::string tm_new_old_password = this->m_password;
        this->m_password = ar_password;

        if( this->m_password != ar_password )
            return false;

        this->m_old_password = tm_new_old_password;
        return true;
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
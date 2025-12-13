// Abdulkadir U. - 24/11/2025
#pragma once

/**
 * Access Policy (Erişim Politikası)
 * 
 * Sunucu ve İstemci tarafında belirli
 * erişim politikaları olmalı ve bu bağlantılar
 * bu politikalara uyarak davranmalı. Şifre,
 * yasaklı kullanıcı, doğrulama vs. gibi güvenlik
 * işlemlerini sunucu ya da istemcilere bırakmak yerine
 * ayrı bir şekilde tasarlayıp onlarında kullanmasını
 * sağlamak daha iyi olacaktır.
 */

// Include:
#include <Tool/Utf/Utf.h>

#include <unordered_set>
#include <mutex>
#include <atomic>
#include <string>

// Using Namespace:
using namespace tool;

// Namespace: Core::Socket
namespace core::socket
{
    // Namespace: Access Policy
    namespace accesspolicy
    {
        // Using:
        using max_conn_t = uint16_t;

        // Enum Class: Access Policy Code
        enum class e_accesspolicy
        {
            err = 1000,
            err_enable_password,
            err_max_conn_under_limit,
            err_max_conn_over_limit,
            err_set_max_conn,
            err_password_len_under_limit,
            err_password_len_over_limit,
            err_set_password,
            err_not_allow_ip,
            err_auth_fail_with_passwd,
            err_ip_addr_ban_not_removed,
            err_ip_addr_not_banned,

            succ = 2000,
            succ_enable_password,
            succ_set_max_conn,
            succ_set_password,
            succ_allow_ip,
            succ_auto_auth,
            succ_auth_with_passwd,
            succ_ip_addr_banned,
            succ_ip_addr_ban_removed,

            warn = 3000,
            warn_enable_password_same_value,
            warn_ip_already_banned,
            warn_ip_not_banned,
        };

        // Limit
        inline constexpr max_conn_t MIN_CONNECTION = 1;
        inline constexpr max_conn_t DEF_CONNECTION = 1024;
        inline constexpr max_conn_t MAX_CONNECTION = 8192;

        inline constexpr size_t MIN_LEN_PASSWORD = 4;
        inline constexpr size_t MAX_LEN_PASSWORD = 128;
    }

    // Using Namespace:
    using namespace accesspolicy;

    // Class: Access Policy
    class AccessPolicy final
    {
        private:
            std::atomic<bool> require_password { false };
            std::u32string password;

            std::atomic<max_conn_t> max_connection { DEF_CONNECTION };
            std::unordered_set<std::string> banned_ip_list;

            mutable std::mutex mtx;

        public:
            AccessPolicy() = default;
            ~AccessPolicy() = default;

            bool isBanned(std::string&) noexcept;

            e_accesspolicy enablePassword(const bool = true) noexcept;
            inline const std::u32string& getPassword() const noexcept;
            e_accesspolicy setPassword(const std::u32string&) noexcept;

            inline max_conn_t getMaxConnection() const noexcept;
            e_accesspolicy setMaxConnection(const max_conn_t = DEF_CONNECTION) noexcept;

            e_accesspolicy canAllow(const std::string&) const noexcept;
            e_accesspolicy canAuth(const std::u32string&) const noexcept;

            e_accesspolicy ban(const std::string) noexcept;
            e_accesspolicy unban(const std::string&) noexcept;
    };
}

// Using Namespace:
using namespace core::socket;
using namespace accesspolicy;

/**
 * @brief [Public] Is Banned
 * 
 * Verilen ip adresinin yasaklanıp yasaklanmadığını
 * listede arama yaparak kontrol ediyoruz. Eğer listenin
 * sonuna gelmiş ise, ip adresi yasaklanmamıştır bu yüzden
 * bulunamamıştır ama aksi halde yasaklıdır.
 * 
 * @param string& Ip Address
 * @return bool
 */
bool AccessPolicy::isBanned(std::string& _ipaddr) noexcept
{
    std::scoped_lock<std::mutex> lock(this->mtx);
    return this->banned_ip_list.find(std::string(_ipaddr)) != this->banned_ip_list.end();
}

/**
 * @brief [Public] Enable Password
 * 
 * Şifre durumunun aktif ya da deaktif
 * edilmesini sağlayacak fonksiyon yapısıdır
 * 
 * @param bool Status
 * @return e_accesspolicy
 */
e_accesspolicy AccessPolicy::enablePassword(const bool _status) noexcept
{
    if( this->require_password.load() == _status )
        return e_accesspolicy::warn_enable_password_same_value;

    this->require_password.store(_status);

    return this->require_password.load() == _status ?
        e_accesspolicy::succ_enable_password :
        e_accesspolicy::err_enable_password;
}

/**
 * @brief [Public] Get Password
 * 
 * Belirlenen şifre bilgisini döndürecek
 * 
 * @return u32string&
 */
const std::u32string& AccessPolicy::getPassword() const noexcept
{
    return this->password;
}

/**
 * @brief [Public] Set Password
 * 
 * Erişim için bir şifre gerekebilir ve bu şifreyi
 * belirli bir standarta göre ayarlayacak
 * 
 * @param u32string& Password
 * @return e_accesspolicy
 */
e_accesspolicy AccessPolicy::setPassword(const std::u32string& _password) noexcept
{
    if( _password.empty() || _password.length() < MIN_LEN_PASSWORD ) return e_accesspolicy::err_password_len_under_limit;
    else if( _password.length() > MAX_LEN_PASSWORD ) return e_accesspolicy::err_password_len_over_limit;

    std::scoped_lock<std::mutex> lock(this->mtx);
    this->password = _password;

    return this->password == _password ?
        e_accesspolicy::succ_set_password :
        e_accesspolicy::err_set_password;
}

/**
 * @brief [Public] Get Max Connection
 * 
 * En fazla bağlantı yapabilme miktarını döndürecek
 * 
 * @return max_conn_t
 */
max_conn_t AccessPolicy::getMaxConnection() const noexcept
{
    return this->max_connection;
}

/**
 * @brief [Public] Set Max Connection
 * 
 * En fazla bağlantı yapabilme sınırını belirleyecek
 * ve en düşük ile en yüksek arasında olmasını sağlayacak
 * 
 * @param max_conn_t Max Connection
 * @return e_accesspolicy
 */
e_accesspolicy AccessPolicy::setMaxConnection(const max_conn_t _max_connection) noexcept
{
    if( _max_connection < MIN_CONNECTION )
        return e_accesspolicy::err_max_conn_under_limit;
    else if( _max_connection > MAX_CONNECTION )
        return e_accesspolicy::err_max_conn_over_limit;

    this->max_connection = _max_connection;

    return this->max_connection == _max_connection ?
        e_accesspolicy::succ_set_max_conn :
        e_accesspolicy::err_set_max_conn;
}

/**
 * @brief [Public] Can Allow
 * 
 * Verilen ip adresini yasak listesinde arama yaparak
 * kontrol eder ve arama listesinde bulunamamış ise
 * ip adresi izin verilebilir demektir.
 * 
 * @param string& Ip Address
 * @return e_accesspolicy
 */
e_accesspolicy AccessPolicy::canAllow(const std::string& _ipaddr) const noexcept
{
    std::scoped_lock<std::mutex> lock(this->mtx);
    return this->banned_ip_list.find(std::string(_ipaddr)) == this->banned_ip_list.end() ?
        e_accesspolicy::succ_allow_ip :
        e_accesspolicy::err_not_allow_ip;
}

/**
 * @brief [Public] Can Auth
 * 
 * Verilen şifre bilgisi ile kayıtlı şifre karşılaştırılır
 * ve eşleşme olduğu durumda başarı onayı verilir aksi halde
 * hatalıdır.
 * 
 * @param u32string& Password
 * @return e_accesspolicy
 */
e_accesspolicy AccessPolicy::canAuth(const std::u32string& _passwd) const noexcept
{
    if( !this->require_password.load() )
        return e_accesspolicy::succ_auto_auth;

    std::scoped_lock<std::mutex> lock(this->mtx);
    return this->password == _passwd ?
        e_accesspolicy::succ_auth_with_passwd :
        e_accesspolicy::err_auth_fail_with_passwd;
}

/**
 * @brief [Public] Ban
 * 
 * Verilen ip adresi yasak listesinde bulunuyor ise
 * tekrar yasaklamaya çalışmasın ve listeyi gereksiz yere
 * doldurmasın diye uyarı mesajı döndürür ama aksi halde
 * verilen ip adresini yasaklı listeye ekler.
 * 
 * @param string Ip Address
 * @return e_accesspolicy
 */
e_accesspolicy AccessPolicy::ban(const std::string _ipaddr) noexcept
{
    std::scoped_lock<std::mutex> lock(this->mtx);

    if( this->banned_ip_list.find(_ipaddr) != this->banned_ip_list.end() )
        return e_accesspolicy::warn_ip_already_banned;

    this->banned_ip_list.emplace(std::move(_ipaddr));

    return this->banned_ip_list.find(_ipaddr) != this->banned_ip_list.end() ?
        e_accesspolicy::succ_ip_addr_banned :
        e_accesspolicy::err_ip_addr_not_banned;
}

/**
 * @brief [Public] Unban
 * 
 * Verilen ip adresini yasak listesinde arama
 * yaparak bulmaya çalışır ve eğer bulamazsa zaten
 * yasaklı değildir uyarısı döndürür ama aksi halde
 * yasaklı olan ip adresinin yasaklı durumunu kaldırır.
 * 
 * @param string& Ip Address
 * @return e_accesspolicy
 */
e_accesspolicy AccessPolicy::unban(const std::string& _ipaddr) noexcept
{
    std::scoped_lock<std::mutex> lock(this->mtx);
        
    if( this->banned_ip_list.find(_ipaddr) == this->banned_ip_list.end() )
        return e_accesspolicy::warn_ip_not_banned;

    this->banned_ip_list.erase(_ipaddr);

    return this->banned_ip_list.find(_ipaddr) == this->banned_ip_list.end() ?
        e_accesspolicy::succ_ip_addr_ban_removed :
        e_accesspolicy::err_ip_addr_ban_not_removed;
}
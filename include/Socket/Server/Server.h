// Abdulkadir U. - 24/11/2025
#pragma once

/**
 * Server (Sunucu)
 * 
 * Soket işlemleri 2 şeye dayanır:
 * 1) Sunucu
 * 2) İstemci
 * 
 * İstemci ya da istemcilerin bağlanabilmesini
 * sağlayacak ana bir sunucuya ihtiyaç vardır.
 * Bu sunucu yapısını istendiği durumda sadece
 * belirli ip adreslerinden gelecek istekle ya da
 * buna ek olarak şifre ile giriş yapabilme
 * sınırlaması da getirebilecek şekilde ayarlanacak.
 * En fazla kullanıcı sınırı da ayarlanabilmesinin yanında
 * belirlenen ip adresinin engellenmesi gibi durumlarda
 * olabilecek. Yapılan işlemler (sunucu ayarı değiştirilmesi,
 * yeni kullanıcı bağlanması, kullanıcı yasaklanması vs.)
 * gibi işlemler kayıt altına alınacaktır.
 */

// Include:
#include <Tool/Utf/Utf.h>
#include <Socket/Socket.h>
#include <ThreadPool/ThreadPool.h>
#include <Socket/AccessPolicy.h>

#include <atomic>
#include <mutex>
#include <unordered_map>
#include <string>
#include <string_view>
#include <iostream>

// Namespace: Core::Socket
namespace core::socket
{
    // Namespace: Server
    namespace server
    {
        // Limit:
        static constexpr size_t MIN_CONN = 1;
        static constexpr size_t MAX_CONN = (uint16_t)~0;

        // Enum Class: Server Code
        enum class e_server
        {
            err_ip_not_allowed_for_connection = 1000,
            err_connection_is_full,
            err_auth_fail,

            succ_connection = 2000,
            succ_auth
        };
    }

    // Using Namespace:
    using namespace core::virbase;
    using namespace server;

    // Class: Server
    template<class Algo, typename SockType>
    class Server final : public Socket<Algo, SockType>
    {
        private:
            ThreadPool& tpool;
            AccessPolicy policy;

            std::atomic<size_t> clients;
            std::unordered_map<std::string, bool> client_list;

            mutable std::mutex mtx;

        public:
            explicit Server(
                Algo&&,
                ThreadPool&,
                const socket_port_t = invalid_port,
                const bool = false
            );
            
            ~Server() = default;

            inline AccessPolicy& getPolicy() noexcept;

            e_server connection(const std::string&) noexcept;
            e_server authenticate(const std::string&, const std::u32string&);

            inline size_t getClientCount() const noexcept;
    };
}

// Using Namespace:
using namespace core::socket;
using namespace server;

/**
 * @brief [Public] Constructor
 * 
 * Algoritma yapısı ve işlem havuzunu alaraktan sunucu
 * oluşturmayı yapacak ve istendiği durumda sunucu kullanıcı
 * limiti ile de belirlenen kullanıcı sayısını aşmayacak şekilde
 * olacak ve parola koruması da ayarlanabilir şekilde ama başlangıçta kapalı.
 * 
 * @tparam Algo&& Algorithm
 * @param ThreadPool& Thread Pool
 * @param size_t Max Connection
 * @param bool Secure Password
 */
template<class Algo, typename SockType>
Server<Algo, SockType>::Server(
    Algo&& _algo,
    ThreadPool& _tpool,
    const socket_port_t _port,
    const bool _password_require
)
: Socket<Algo, SockType>(std::forward<Algo>(_algo), _port),
  tpool(_tpool),
  clients(0)
{
    this->policy.enablePassword(_password_require);
}

/**
 * @brief [Public] Get Policy
 * 
 * Erişim kontrolü durumlarına erişebilmeyi
 * sağlamak için nesneyi referans olarak döndürür
 * 
 * @return AccessPolicy&
 */
template<class Algo, typename SockType>
AccessPolicy& Server<Algo, SockType>::getPolicy() noexcept
{
    return this->policy;
}

/**
 * @brief [Public] Connection
 * 
 * Sunucuya bağlanmayı sağlayan fonksiyondur.
 * İp adresinin yasaklı olup olmaması gibi etkenleri de
 * kontrol ederek istemcinin sunucuya bağlantı sağlaması gibi
 * işlemleri yapar.
 * 
 * @param string& Ip Address
 * @return e_server
 */
template<class Algo, typename SockType>
e_server Server<Algo, SockType>::connection(const std::string& _ipaddr) noexcept
{
    if( this->policy.canAllow(_ipaddr) != e_accesspolicy::succ_allow_ip )
        return e_server::err_ip_not_allowed_for_connection;

    if( this->clients.load() >= this->policy.getMaxConnection() )
        return e_server::err_connection_is_full;

    {
        std::scoped_lock<std::mutex> lock(this->mtx);
        this->client_list[_ipaddr] = true;
    }

    this->clients++;

    this->tpool.enqueue([this, _ipaddr]()
    {
        
    });

    return e_server::succ_connection;
}

/**
 * @brief [Public] Authenticate
 * 
 * Sunucuya giriş de şifre kontrolü yapar ve
 * eğer şifre doğrulanmış ise doğrulama başarılı döner
 * ve kullanıcının erişim izni olur
 * 
 * @param string& Ip Address
 * @param u32string& Password
 * @return e_server
 */
template<class Algo, typename SockType>
e_server Server<Algo, SockType>::authenticate(const std::string& _ipaddr, const std::u32string& _passwd)
{
    auto auth = this->policy.canAuth(_passwd);

    if( auth == e_accesspolicy::succ_auto_auth ||
        auth == e_accesspolicy::succ_auth_with_passwd )
        return e_server::succ_auth;

    return e_server::err_auth_fail;
}

/**
 * @brief [Public] Get Client Count
 * 
 * Sunucu da aktif olarak bulunan istemci miktarını
 * saklı tutan bir değişken var. Bu değişkenin değerini
 * döndürerek sunucu daki istemci sayısını öğrenmiş oluyoruz.
 * 
 * @return size_t
 */
template<class Algo, typename SockType>
size_t Server<Algo, SockType>::getClientCount() const noexcept
{
    return this->clients;
}
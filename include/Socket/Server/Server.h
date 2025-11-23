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

#include <atomic>
#include <mutex>

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
            err_max_conn_under_limit = 1000,
            err_max_conn_over_limit,
            err_set_max_conn,
            err_set_secure_passwd,

            succ_set_max_conn = 2000,
            succ_set_secure_passwd
        };
    }

    // Using Namespace:
    using namespace core::virbase;
    using namespace server;

    // Class: Server
    template<class Algo, typename SockType>
    class Server final : public virtual Socket<Algo, SockType>
    {
        private:
            std::atomic<uint16_t> max_connection;

            std::atomic<bool> secure_password;
            std::vector<int> banned_list;

            ThreadPool& tpool;

            mutable std::mutex srv_mutex;

        public:
            explicit Server(
                Algo&&,
                ThreadPool&,
                const size_t = server::MAX_CONN,
                const bool = false,
                const socket_port_t = invalid_port
            ) noexcept;
            
            ~Server() noexcept;

            e_server setConnectionLimit(const size_t = MAX_CONN) noexcept;
            e_server setSecurePassword(const bool = false) noexcept;
    };

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
        const size_t _max_conn,
        const bool _passwd,
        const socket_port_t _port
    ) noexcept
    : Socket<Algo, SockType>(_algo, _port),
      tpool(_tpool)
    {
        this->setConnectionLimit(_max_conn);
        this->setSecurePassword(_passwd);
    }

    /**
     * @brief [Public] Destructor
     * 
     * Sunucu sınıfı ile işlem bittiğinde
     * yapılması gereken sonlandırma işlemlerinin
     * yapılacağı sınıfın yıkıcı yapısı
     */
    template<class Algo, typename SockType>
    Server<Algo, SockType>::~Server() noexcept
    {
        this->setConnectionLimit(MIN_CONN - 1);
        this->setSecurePassword(false);

        ~Socket();
    }

    /**
     * @brief [Public] Set Connection Limit
     * 
     * Sunucuya bağlanan istemci sayısının belirli bir
     * miktarı olmak zorunda, bunu sınır aşımı olmaması adına
     * ve fazla fazla imkan sunmak adına unsigned short
     * sayı miktarı sınırı ile 1 arasında sınırladık.
     * Kontrollü bir şekilde kullanıcı limitini belirleyecek fonksiyon.
     * 
     * @param size_t Max Connection
     * @return e_server
     */
    template<class Algo, typename SockType>
    e_server Server<Algo, SockType>::setConnectionLimit(const size_t _max_conn) noexcept
    {
        if( _max_conn < MIN_CONN ) return e_server::err_max_conn_under_limit;
        else if( _max_conn > MAX_CONN ) return e_server::err_max_conn_over_limit;

        {
            std::unique_lock<std::mutex> lock(this->srv_mutex);
            this->max_connection = _max_conn;
        }
        
        return this->max_connection == _max_conn ?
            e_server::succ_set_max_conn :
            e_server::err_set_max_conn;
    }

    /**
     * @brief [Public] Set Secure Password
     * 
     * Sunucu isterse eğer şifreli girişleri kabul etme
     * ile sınırlandırabilir. Bu durumu ayarlamamızı
     * sağlayacak olan fonksiyon.
     * 
     * @param bool Secure Passwd
     * @return e_server
     */
    template<class Algo, typename SockType>
    e_server Server<Algo, SockType>::setSecurePassword(const bool _secure_passwd) noexcept
    {
        {
            std::unique_lock<std::mutex> lock(this->srv_mutex);
            this->secure_password = _secure_passwd;
        }

        return this->secure_password == _secure_passwd ?
            e_server::succ_set_secure_passwd :
            e_server::err_set_secure_passwd
    }
}
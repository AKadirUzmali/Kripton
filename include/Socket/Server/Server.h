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
#include <chrono>
#include <thread>
#include <vector>
#include <future>
#include <functional>

// Namespace: Core::Socket
namespace core::socket
{
    // Namespace: Server
    namespace server
    {        
        // Using:
        using wait_time_t = uint16_t;

        // Define:
        static constexpr size_t size_c32 = sizeof(char32_t);

        static constexpr socket_t invalid_bind = 0;
        static constexpr socket_t invalid_recive = 0;
        static constexpr socket_t invalid_accept = 0;
        static constexpr socket_t invalid_listen = 0;
        
        // Limit:
        static constexpr wait_time_t MIN_WAIT_TIME = 0; // ms
        static constexpr wait_time_t DEF_WAIT_TIME = 100; // ms
        static constexpr wait_time_t MAX_WAIT_TIME = 1000; // ms

        // String
        static const std::u32string DEF_NICKNAME = U"Server";

        // Enum Class: Server Code
        enum class e_server : size_t
        {
            err_server_not_created,
            err_server_bind_fail,
            err_server_listen_fail,
            err_server_accept_fail,
            err_auth_fail,
            err_wait_time_under_limit,
            err_wait_time_over_limit,
            err_set_wait_time_fail,
            err_set_running_failed,
            err_server_already_running,
            err_server_close_fail,

            succ_server_run,
            succ_set_wait_time,
            succ_set_running,
            succ_server_close,

            warn_no_status,
            warn_server_run_status_same,
            warn_server_already_stop
        };
    }

    // Using Namespace:
    using namespace core::virbase;
    using namespace server;

    // Class: Server
    template<class Algo>
    class Server final : public Socket<Algo>
    {
        public:
            using work_handler = std::function<e_server(const std::u32string&)>;

        private:
            ThreadPool& tpool;
            AccessPolicy policy;

            std::future<e_server> frun {};
            std::atomic<e_server> status { e_server::warn_no_status };

            std::atomic<bool> running { false };
            std::atomic<wait_time_t> wait_us { DEF_WAIT_TIME };

            std::unordered_map<std::string, bool> clients;

            std::mutex mtx;
            work_handler handler {};

        private:
            e_server loop() noexcept;
            void accept_loop() noexcept;
            void client_worker(socket_t _soc,const std::string& _ip) noexcept;

        public:
            explicit Server(
                Algo&& _algorithm,
                ThreadPool& _tpool,
                work_handler _handler,
                const socket_port_t _port = invalid_port,
                const bool _passwd_require = false,
                const size_t _buffsize = DEF_SIZE_BUFFER,
                const max_conn_t _max_conn = DEF_CONNECTION,
                const wait_time_t _wait_time = DEF_WAIT_TIME
            );
            
            ~Server();

            inline AccessPolicy& getPolicy() noexcept;

            inline bool isRunning() const noexcept;
            inline e_server getStatus() const noexcept;

            inline wait_time_t getWaitTime() const noexcept;
            e_server setWaitTime(const wait_time_t _wait = DEF_WAIT_TIME, const bool _secure = true) noexcept;

            inline void setHandler(work_handler _handler) noexcept;

            std::future<e_server>& run() noexcept;
            std::future<e_server>& stop() noexcept;
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
template<class Algo>
Server<Algo>::Server(
    Algo&& _algorithm,
    ThreadPool& _tpool,
    work_handler _handler,
    const socket_port_t _port,
    const bool _passwd_require,
    const size_t _buffsize,
    const max_conn_t _max_conn,
    const wait_time_t _wait_time
)
: Socket<Algo>(std::forward<Algo>(_algorithm), DEF_NICKNAME, _port, _buffsize),
  tpool(_tpool),
  handler(std::move(_handler))
{
    this->policy.enablePassword(_passwd_require);
    this->policy.setMaxConnection(_max_conn);

    if( this->setWaitTime(_wait_time) != e_server::succ_set_wait_time )
        this->setWaitTime(DEF_WAIT_TIME);
}

/**
 * @brief [Public] Destructor
 * 
 * Sınıf yıkıcısıdır. İşlem tamamı
 * ile bittiği zaman gerekli yok etme
 * işlemlerini yapar. Eğer hala çalışıyorsa
 * bitmesini bekler
 */
template<class Algo>
Server<Algo>::~Server()
{
    this->stop();

    if( this->frun.valid() )
        this->frun.wait();
}

/**
 * @brief [Public] Get Policy
 * 
 * Erişim kontrolü durumlarına erişebilmeyi
 * sağlamak için nesneyi referans olarak döndürür
 * 
 * @return AccessPolicy&
 */
template<class Algo>
AccessPolicy& Server<Algo>::getPolicy() noexcept
{
    return this->policy;
}

/**
 * @brief [Public] Is Running
 * 
 * Sunucunun çalışıp çalımadığının bilgisini döndürür
 * 
 * @return bool
 */
template<class Algo>
bool Server<Algo>::isRunning() const noexcept
{
    return this->running.load();
}

/**
 * @brief [Public] Get Status
 * 
 * Sunucuya ait durum kodunu döndürür
 * 
 * @return e_server
 */
template<class Algo>
e_server Server<Algo>::getStatus() const noexcept
{
    return this->status.load();
}

/**
 * @brief [Public] Get Wait Time
 * 
 * Her veri alımı arası bir gecikme süresi bulunur.
 * Bu süreyi değer olarak döndürüyoruz
 * 
 * @return wait_time_t
 */
template<class Algo>
wait_time_t Server<Algo>::getWaitTime() const noexcept
{
    return this->wait_us.load();
}

/**
 * @brief [Public] Set Wait Time
 * 
 * Veri alımları arasında oluşabilecek süre gecikmesini
 * belirleyecek ama kontrol altında yapacak bunu ve
 * eğer kontrol devre dışı bırakılmış ise, sınırsız süre
 * belirlenebilir fakat bu pek tavsiye edilmez
 * 
 * @param wait_time_t Wait Time
 * @param bool Secure
 * @return e_server
 */
template<class Algo>
e_server Server<Algo>::setWaitTime(const wait_time_t _wait, const bool _secure) noexcept
{
    if( _wait < MIN_WAIT_TIME ) return e_server::err_wait_time_under_limit;
    else if( _secure && _wait > MAX_WAIT_TIME ) return e_server::err_wait_time_over_limit;

    this->wait_us.store(_wait);
    return this->wait_us.load() == _wait ?
        e_server::succ_set_wait_time :
        e_server::err_set_wait_time_fail;
}

template<class Algo>
void Server<Algo>::setHandler(work_handler _handler) noexcept
{
    this->handler = std::move(_handler);
}

/**
 * @brief [Public] Run
 * 
 * Sunucuyu çalıştıracak olan fonksiyondur
 * Gerekli kontrolü yaptıktan sonra sunucu
 * döngü halinde çalışır
 * 
 * @return future<e_server>&
*/
template<class Algo>
std::future<e_server>& Server<Algo>::run() noexcept
{
    if( this->running.exchange(true) )
    {
        this->status.store(e_server::err_server_already_running);
        return this->frun;
    }

    if( this->create() != e_socket::succ_socket_create )
    {
        this->status.store(e_server::err_server_not_created);
        return this->frun;
    }

    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(this->getPort());
    addr.sin_addr.s_addr = INADDR_ANY;

    if( ::bind(this->getSocket(), (sockaddr*)&addr, sizeof(addr)) < invalid_bind )
    {
        this->status.store(e_server::err_server_bind_fail);
        return this->frun;
    }

    if( ::listen(this->getSocket(), (int)this->policy.getMaxConnection()) < invalid_listen )
    {
        this->status.store(e_server::err_server_listen_fail);
        return this->frun;
    }

    this->frun = std::async(std::launch::async, [this]() {
        e_server result = this->loop();
        this->status.store(result);
        return result;
    });

    return this->frun;
}

/**
 * @brief [Public] Stop
 * 
 * Sunucu çalışıyorsa eğer durduracaktır ve
 * gerekli kontrolleri yaparak bunu güvenli şekilde
 * yapar
 * 
 * @return future<e_server>&
 */
template<class Algo>
std::future<e_server>& Server<Algo>::stop() noexcept
{
    if( !this->running.exchange(false) )
    {
        this->status.store(e_server::warn_server_already_stop);
        return this->frun;
    }

    this->close();
    this->status.store(this->isClose() ?
        e_server::succ_server_close :
        e_server::err_server_close_fail
    );

    return this->frun;
}

/**
 * @brief [Private] Loop
 * 
 * Döngü halinde çalışacak ve durdurma yapılana dek
 * geçici bekletme ile çalışacak. İşlem bittiğinde
 * başarı kodunu döndürecek
 * 
 * @return e_server
 */
template<class Algo>
e_server Server<Algo>::loop() noexcept
{
    this->tpool.enqueue([this]{ this->accept_loop(); });

    while( this->running.load() )
        std::this_thread::sleep_for(std::chrono::microseconds(this->wait_us.load()));
    return e_server::succ_server_run;
}

/**
 * @brief [Private] Accept Loop
 * 
 * Döngü ile işlem sonlanması durumu ayarlanana
 * dek çalışacak ve istemciyi dinleyecek.
 * İstemci yasaklı ise pas geçecek ama eğer
 * yasaklı değil ise en fazla bağlantı limitini
 * değerlendirecek. Limit de sorun yoksa eğer
 * işlem sırasına ekleyerek istemci veri
 * işlemlerine başlayacak
 */
template<class Algo>
void Server<Algo>::accept_loop() noexcept
{
    while( this->running.load() )
    {
        sockaddr_in cli {};
        socklen_t len = sizeof(cli);
        socket_t fd = ::accept(this->getSocket(), (sockaddr*)&cli, &len);
        
        if( fd <= invalid_socket )
            continue;

        std::string ip = inet_ntoa(cli.sin_addr);

        if( this->policy.isBanned(ip) )
        {
            ::close(fd);
            continue;
        }

        {
            std::scoped_lock<std::mutex> lock(this->mtx);

            if( this->clients.size() >= this->policy.getMaxConnection() )
            {
                ::close(fd);
                continue;
            }

            this->clients[ip] = true;
        }

        this->tpool.enqueue([this, fd, ip] {
            this->client_worker(fd, ip);
        });
    }
}

/**
 * @brief [Private] Client Worker
 * 
 * İstemci soketinden gelen veriyi istemcinin
 * ip adresini kullanarak alacak. Veri gelmemesi
 * durumunda dinlemeyi sonlandıracak ama aksi halde
 * veri geldiğinde veriyi işleyecek ve bu sayede
 * iletişim sağlanmış olacak. Şifreli iletişim olması
 * adına gelen şifreli veriyi çözdükten sonra
 * isterse direk ya da isterse eklemeli şekilde veriyi
 * şifreleyip iletecek. Sonlandıktan sonra ise
 * soketi kapatıp ip adresini listeden silecek
 * 
 * @param socket_t Socket
 * @param std::string& Ip Address 
 */
template<class Algo>
void Server<Algo>::client_worker
(
    socket_t _soc,
    const std::string& _ip
) noexcept
{
    std::vector<char32_t> buffer(this->getBufferSize());

    while( this->running.load() )
    {
        std::this_thread::sleep_for(std::chrono::microseconds(this->wait_us.load()));

        int recive = ::recv(_soc, buffer.data(), buffer.size() * size_c32, 0);
        if( recive <= invalid_recive )
            break;

        buffer.resize(recive / size_c32);

        std::u32string data(buffer.begin(), buffer.end());
        this->getAlgorithm().decrypt(data);

        if( this->handler )
            this->handler(data);

        this->getAlgorithm().encrypt(data);

        ::send(_soc, data.data(), data.size() * size_c32, 0);

        utf::u32vector_clear(buffer);
    }

    ::close(_soc);
    std::scoped_lock lock(this->mtx);
    this->clients.erase(_ip);
}
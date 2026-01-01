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
#include <Socket/AccessPolicy.h>
#include <ThreadPool/ThreadPool.h>
#include <Handler/Crash/CrashBase.h>

#include <atomic>
#include <mutex>
#include <unordered_map>
#include <string>
#include <string_view>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <array>
#include <future>
#include <functional>

// Namespace: Core::Virbase::Socket
namespace core::virbase::socket
{
    // Namespace: Server
    namespace server
    {        
        // Windows / DOS (Winsock)
        #if defined __PLATFORM_DOS__
            static inline constexpr int inv_accept = SOCKET_ERROR;
            static inline constexpr int inv_bind = -1;
        #elif defined __PLATFORM_POSIX__
            static inline constexpr int inv_accept = -1;
            static inline constexpr int inv_bind = -1;
        #endif
        
        // Limit:
        static constexpr wait_time_t MIN_WAIT_TIME = 0; // ms
        static constexpr wait_time_t DEF_WAIT_TIME = 100; // ms
        static constexpr wait_time_t MAX_WAIT_TIME = 1000; // ms

        // String
        static const std::u32string DEF_NICKNAME = U"Server";

        // Enum Class: Server Code
        enum class e_server : glo::status_t
        {
            unknwn = static_cast<glo::status_t>(glo::e_status_t::server),

            err = unknwn + static_cast<glo::status_t>(glo::e_status::err),
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
            err_socket_not_found_in_list,
            err_socket_banned,
            err_server_send,
            err_server_recv,
            err_send_pack_over_size,
            err_recv_socket_closed,
            err_recv_server,
            err_recv_over_limit_and_banned,
            err_socket_potential_attacker,
            err_client_socket_invalid,
            err_client_send_no_data,
            err_socket_password_changed,
            err_auth_no_data,

            succ = unknwn + static_cast<glo::status_t>(glo::e_status::succ),
            succ_server_bind,
            succ_server_listen,
            succ_server_accept,
            succ_server_run,
            succ_set_wait_time,
            succ_set_running,
            succ_server_close,
            succ_server_send,
            succ_server_recv,

            warn = unknwn + static_cast<glo::status_t>(glo::e_status::warn),
            warn_no_status,
            warn_server_run_status_same,
            warn_server_already_stop,
            warn_socket_can_be_close,
            warn_server_send_not_all_data,
            warn_recv_wrong_password
        };

        constexpr bool operator==(glo::status_t _first, e_server _second) noexcept { return _first == static_cast<glo::status_t>(_second); }
        constexpr bool operator!=(glo::status_t _first, e_server _second) noexcept { return _first != static_cast<glo::status_t>(_second); }
    }

    // Using Namespace:
    using namespace core::handler;
    using namespace server;

    // Class: Server
    template<class Algo = Algorithm>
    class Server final : public Socket<Algo>
    {
        public:
            using work_handler = std::function<e_server(const datapacket_t&)>;

        private:
            ThreadPool& tpool;

            mutable std::shared_future<e_server> frun {};

            std::atomic<e_server> status { e_server::warn_no_status };

            std::atomic<bool> running { false };
            std::atomic<wait_time_t> wait_us { DEF_WAIT_TIME };

            std::unordered_map<socket_t, SocketCtx_t> clients;

            std::mutex mtx;
            work_handler handler {};

            std::shared_ptr<std::promise<e_server>> internal_promise;

        private:
            void setPromise(e_server) noexcept;

            void loop() noexcept;
            void client_worker(socket_t, const std::string&) noexcept;

            e_server bind() noexcept;
            e_server listen() noexcept;

        public:
            explicit Server(
                Algo& _algorithm,
                ThreadPool& _tpool,
                work_handler _handler,
                const socket_port_t _port,
                const bool _passwd_require,
                const bool _log,
                const std::u32string& _logheader,
                const std::u32string& _logfilename,
                const size_t _buffsize = DEF_SIZE_BUFFER,
                const max_conn_t _max_conn = DEF_CONNECTION,
                const wait_time_t _wait_time = DEF_WAIT_TIME
            );
            
            ~Server();

            inline bool isRunning() const noexcept;
            inline e_server getStatus() const noexcept;

            inline wait_time_t getWaitTime() const noexcept;
            e_server setWaitTime(const wait_time_t _wait = DEF_WAIT_TIME, const bool _secure = true) noexcept;

            inline void setHandler(work_handler _handler) noexcept;

            virtual glo::status_t send(const socket_t, datapacket_t&) noexcept override;
            virtual glo::status_t receive(const socket_t, datapacket_t&) noexcept override;

            std::shared_future<e_server> run() noexcept;
            e_server stop() noexcept;

            virtual void onCrash() noexcept override;
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
    template<class Algo>
    Server<Algo>::Server(
        Algo& _algorithm,
        ThreadPool& _tpool,
        work_handler _handler,
        const socket_port_t _port,
        const bool _passwd_require,
        const bool _log,
        const std::u32string& _logheader,
        const std::u32string& _logfilename,
        const size_t _buffsize,
        const max_conn_t _max_conn,
        const wait_time_t _wait_time
    )
    : Socket<Algo>(
        _algorithm,
        DEF_NICKNAME,
        _port,
        _log,
        _logheader,
        _logfilename,
        _buffsize),
      tpool(_tpool)
    {
        this->getPolicy().enablePassword(_passwd_require);
        this->getPolicy().setMaxConnection(_max_conn);
        this->setHandler(_handler);

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
    }

    /**
     * @brief [Private] Set Promise
     * 
     * Yanıt döndürmeyi söz vermeye ayarlıyoruz
     * 
     * @param e_server Value
     */
    template<class Algo>
    void Server<Algo>::setPromise(e_server _value) noexcept
    {
        auto prom = this->internal_promise;
        if( !prom )
            return;

        try {
            prom->set_value(_value);
        } catch( ... ) {}
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
     * 
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

    /**
     * @brief [Public] Set Handler
     * 
     * Çalışma işlevini gerçekleştirecek yapıyı
     * atayacak.
     * 
     * @param work_handler Handler
     */
    template<class Algo>
    void Server<Algo>::setHandler(work_handler _handler) noexcept
    {
        this->handler = std::move(_handler);
    }

    /**
     * @brief [Public] Send
     * 
     * Socketler arası iletişimde veri gönderimini sağlayacak
     * olan send fonksiyonu gönderilmek istenen metini alır
     * istenen nesneye çevirip gönderir
     * 
     * @param socket_t Socket
     * @param datapacket_t Data Packet
     * 
     * @return e_socket
     */
    template<class Algo>
    glo::status_t Server<Algo>::send(
        const socket_t _socket,
        datapacket_t& _rawpacket
    ) noexcept
    {
        if( _socket == inv_socket )
            return glo::to_status<e_server>(e_server::err_client_socket_invalid);

        const std::string ipaddr = get_ip(_socket);
        const auto findip = this->clients.find(_socket);

        if( findip == this->clients.end() )
            return glo::to_status<e_server>(e_server::err_socket_not_found_in_list);
        else if( this->getPolicy().isBanned(ipaddr) )
            return glo::to_status<e_server>(e_server::err_socket_banned);

        e_socket send_status = glo::to_status<e_socket>(Socket<Algo>::send(_socket, _rawpacket));
        return send_status == e_socket::succ_socket_send ?
            glo::to_status<e_server>(e_server::succ_server_send) :
            glo::to_status<e_socket>(send_status);
    }

    /**
     * @brief [Public] Receive
     * 
     * Socketler arası iletişimde gönderilen veriyi almak
     * için kullanılan recv fonksiyonundan faydalanarak
     * gönderilen veriyi alıp işleyip belirtilen
     * çıktı değişkenine aktarır
     * 
     * @param socket_t Socket
     * @param datapacket_t Data Packet
     * 
     * @return glo::status_t
     */
    template<class Algo>
    glo::status_t Server<Algo>::receive(
        const socket_t _socket,
        datapacket_t& _netpack
    ) noexcept
    {
        if( _socket == inv_socket )
            return glo::to_status<e_server>(e_server::err_client_socket_invalid);

        const std::string ipaddr = get_ip(_socket);

        {
            std::scoped_lock<std::mutex> lock(this->mtx);

            auto client_it = this->clients.find(_socket);
            if( client_it == this->clients.end() )
                return glo::to_status<e_server>(e_server::err_socket_not_found_in_list);

            if( this->getPolicy().isBanned(ipaddr) )
                return glo::to_status<e_server>(e_server::err_socket_banned);
        }

        e_socket recv_status = glo::to_status<e_socket>(Socket<Algo>::receive(_socket, _netpack));
        if( recv_status != e_socket::succ_socket_recv )
            return glo::to_status<e_socket>(recv_status);

        {
            std::scoped_lock<std::mutex> lock(this->mtx);

            if( this->clients[_socket].user.username.empty() || this->clients[_socket].user.username != _netpack.name ) {
                this->clients[_socket].user.username = _netpack.name;

                for( auto& client_it : this->clients ) {
                    if( client_it.second.ip == ipaddr )
                        continue;

                    if( client_it.second.user.username == this->clients[_socket].user.username )
                        ++client_it.second.user.same_user_count;
                }
            }
        }

        if( !this->getPolicy().isEnablePassword() )
            return glo::to_status<e_server>(e_server::succ_server_recv);

        switch( this->getPolicy().canAuth(_netpack.pwd) )
        {
            case e_accesspolicy::succ_auth_with_passwd:
                return glo::to_status<e_server>(e_server::succ_server_recv);
            default:
                if( _netpack.pwd.empty() && _netpack.name.empty() && _netpack.msg.empty() )
                    return glo::to_status<e_server>(e_server::err_auth_no_data);

                {
                    std::scoped_lock<std::mutex> lock(this->mtx);

                    ++this->clients[_socket].user.try_passwd;

                    if( this->isLog() ) LOG_MSG(this->getLogger(),
                        U"Client (" + utf::to_utf32(ipaddr) + U") sent wrong password attempt #" +
                        utf::to_utf32(std::to_string(this->clients[_socket].user.try_passwd)),
                        test::e_status::warning,
                        true
                    );

                    if( this->clients[_socket].user.try_passwd == static_cast<uint16_t>(MAX_SOCKET_RETRY_PASSWD) )
                        return glo::to_status<e_server>(e_server::err_socket_banned);
                    else if( this->clients[_socket].user.try_passwd > static_cast<uint16_t>(MAX_SOCKET_RETRY_PASSWD) )
                        return glo::to_status<e_server>(e_server::err_socket_potential_attacker);
                    else if( _netpack.pwd == this->getPolicy().getOldPassword() )
                        return glo::to_status<e_server>(e_server::err_socket_password_changed);
                }
            
                return glo::to_status<e_server>(e_server::warn_recv_wrong_password);
        }

        return glo::to_status<e_server>(e_server::err_recv_server);
    }

    /**
     * @brief [Private] Bind
     * 
     * Sunucunun her adresten bağlantıyı kabul etmeyi, port adresini
     * ayarlamayı ve bağlantı ipv4 türünü ayarlamayı sağlamak
     * 
     * @return e_server
     */
    template<class Algo>
    e_server Server<Algo>::bind() noexcept
    {
        sockaddr_in addr {};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(this->getPort());

        int result = ::bind(this->getSocket(), (sockaddr*)&addr, sizeof(addr));
        return result != inv_bind ?
            e_server::succ_server_bind :
            e_server::err_server_bind_fail;
    }

    /**
     * @brief [Private] Listen
     * 
     * Sunucunun istemciyi dinlemesini sağlamak
     * 
     * @return e_server
     */
    template<class Algo>
    e_server Server<Algo>::listen() noexcept
    {
        int backlog = this->getPolicy().getMaxConnection();

        if( this->getPolicy().getMaxConnection() < MIN_CONNECTION ) backlog = MIN_CONNECTION;
        else if( this->getPolicy().getMaxConnection() > MAX_CONNECTION ) backlog = MAX_CONNECTION;

        return ::listen(this->getSocket(), backlog) == 0 ?
            e_server::succ_server_listen :
            e_server::err_server_listen_fail;
    }

    /**
     * @brief [Public] Run
     * 
     * Sunucuyu çalıştıracak olan fonksiyondur
     * Gerekli kontrolü yaptıktan sonra sunucu
     * döngü halinde çalışır
     * 
     * @return shared_future<e_server>&
    */
    template<class Algo>
    std::shared_future<e_server> Server<Algo>::run() noexcept
    {
        if( this->isRunning() )
        {
            this->status.store(e_server::err_server_already_running);
            return this->frun;
        }

        this->running.store(true);
        this->internal_promise = std::make_shared<std::promise<e_server>>();

        try {
            this->frun = this->internal_promise->get_future().share();
        } catch( ... ) {
            this->status.store(e_server::err_set_running_failed);

            std::promise<e_server> tmprom;
            tmprom.set_value(this->status.load());
            return tmprom.get_future().share();
        }

        if( this->create() != e_socket::succ_socket_create )
        {
            this->status.store(e_server::err_server_not_created);
            this->setPromise(this->status.load());
            return this->frun;
        }

        if( this->bind() != e_server::succ_server_bind )
        {
            this->status.store(e_server::err_server_bind_fail);
            this->setPromise(this->status.load());
            return this->frun;
        }

        if( this->listen() != e_server::succ_server_listen )
        {
            this->status.store(e_server::err_server_listen_fail);
            this->setPromise(this->status.load());
            return this->frun;
        }

        auto fut = std::async(std::launch::async, [this] {
            std::thread([this]{ this->loop(); }).detach();

            this->status.store(e_server::succ_server_run);
            this->setPromise(this->status.load());

            return this->frun;
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
     * @return e_server
     */
    template<class Algo>
    e_server Server<Algo>::stop() noexcept
    {
        this->running.store(false);
        
        socket_t socketdata = this->getSocket();

        if( socketdata != inv_socket )
        {
            #if defined __PLATFORM_DOS__
                ::shutdown(socketdata, SD_BOTH);
                ::closesocket(socketdata);
            #else
                ::shutdown(socketdata, SHUT_RDWR);
                ::close(socketdata);
            #endif
        }

        this->status.store(this->isClose() ?
            e_server::succ_server_close :
            e_server::err_server_close_fail);

        this->setPromise(this->status.load());

        return this->status.load();
    }

    /**
     * @brief [Private] Loop
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
    void Server<Algo>::loop() noexcept
    {
        const wait_time_t client_timeout = static_cast<wait_time_t>(std::max<size_t>(MIN_WAIT_TIME, this->wait_us.load()));
        
        while( this->isRunning() )
        {
            std::this_thread::sleep_for(std::chrono::microseconds(client_timeout));

            if( this->getSocket() == inv_socket )
                break;

            sockaddr_in servaddr {};
            socklen_t servlen = sizeof(servaddr);
            socket_t servaccpt = ::accept(this->getSocket(), reinterpret_cast<sockaddr*>(&servaddr), &servlen);

            if( servaccpt == static_cast<decltype(servaccpt)>(inv_accept) )
                continue;

            std::string ipaddr = get_ip(servaccpt);
            if( this->getPolicy().isBanned(ipaddr) ) {
                close_socket(servaccpt);

                if( this->isLog() ) LOG_MSG(
                    this->getLogger(),
                    utf::to_utf32("Ip Address (" + ipaddr + ") Is Already Banned"),
                    test::e_status::error,
                    false
                );
                continue;
            }

            {
                std::scoped_lock<std::mutex> lock(this->mtx);

                size_t count_same_ip = 0;
                size_t count_accept = this->clients.count(servaccpt);

                for( const auto& [sock, ctx] : this->clients ) {
                    if( ctx.ip == ipaddr )
                        ++count_same_ip;
                }

                if( count_same_ip > this->getPolicy().getMaxSameIp() || count_accept > this->getPolicy().getMaxSameIp() )
                {
                    close_socket(servaccpt);
                    continue;
                }

                if( this->clients.size() >= this->getPolicy().getMaxConnection() ) {
                    close_socket(servaccpt);
                    continue;
                }

                this->clients.emplace(servaccpt, SocketCtx_t{ .sock = servaccpt, .ip = ipaddr, {} });

                if( this->isLog() ) LOG_MSG(this->getLogger(),
                    U"New Client Connected: " + utf::to_utf32(ipaddr) + U" | Current Connections: " +
                    utf::to_utf32(std::to_string(this->clients.size())),
                    test::e_status::information,
                    false
                );
            }

            this->tpool.enqueue([this, servaccpt, ipaddr] {
                this->client_worker(servaccpt, ipaddr);
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
        if( !set_socket_timeout(_soc, this->getTimeout()) )
            return;

        auto last_activity = std::chrono::steady_clock::now();

        while( this->isRunning() )
        {
            if( this->getPolicy().isBanned(_ip) )
            {
                if( this->isLog() ) LOG_MSG(this->getLogger(), 
                    utf::to_utf32(_ip + " ip address banned"),
                    test::e_status::warning,
                    false
                );
                break;
            }

            if( std::chrono::steady_clock::now() - last_activity > std::chrono::seconds(this->getTimeout()) )
            {
                if( this->isLog() ) LOG_MSG(this->getLogger(), 
                    utf::to_utf32(_ip + " idle timeout over limit (" + std::to_string(this->getTimeout()) + ")"),
                    test::e_status::warning,
                    false
                );
                break;
            }

            datapacket_t net_datapacket;
            e_server status_client = glo::to_status<e_server>(this->receive(_soc, net_datapacket));

            switch( status_client )
            {
                case e_server::succ_server_recv:
                    last_activity = std::chrono::steady_clock::now();
                    break;
                case e_server::warn_recv_wrong_password:
                    if( this->isLog() ) LOG_MSG(
                        this->getLogger(),
                        utf::to_utf32("Data Couldn't Receive From Ip Address (" + _ip + ") Status Code: " + std::to_string(static_cast<size_t>(status_client))),
                        test::e_status::error,
                        false
                    );
                    continue;

                case e_server::err_socket_banned:
                    this->getPolicy().ban(_ip);

                    if( this->isLog() ) LOG_MSG(
                            this->getLogger(),
                            utf::to_utf32("Ip Address (" + _ip + ") Banned | Status Code: " + std::to_string(static_cast<size_t>(status_client))),
                            test::e_status::error,
                            false
                        );
                    goto end_of_socket;

                case e_server::err_socket_potential_attacker:
                    this->getPolicy().ban(_ip);

                    if( this->isLog() ) LOG_MSG(
                        this->getLogger(),
                        utf::to_utf32("Ip Address (" + _ip + ") Is Potential Attacker | Status Code: " + std::to_string(static_cast<size_t>(status_client))),
                        test::e_status::error,
                        false
                    );
                    goto end_of_socket;
                
                default:
                    if( this->isLog() ) LOG_MSG(
                        this->getLogger(),
                        utf::to_utf32("Ip Address (" + _ip + ") | Status Code: " + std::to_string(static_cast<size_t>(status_client))),
                        test::e_status::error,
                        true
                    );
                    goto end_of_socket;
            }

            if( this->handler )
                this->handler(net_datapacket);

            if( this->isLog() ) LOG_MSG(this->getLogger(),
                U"[DATA] Password: " + net_datapacket.pwd + U" | Username: " + net_datapacket.name + U" | Message: " + net_datapacket.msg +
                U" | From Ip: " + utf::to_utf32(_ip) + U" | Saved Username: " + this->clients[_soc].user.username +
                U" | Same User Count: " + utf::to_utf32(std::to_string(this->clients[_soc].user.same_user_count)) +
                U" | Try Passwd Count: " + utf::to_utf32(std::to_string(this->clients[_soc].user.try_passwd)),
                test::e_status::information,
                false
            );

            status_client = glo::to_status<e_server>(this->send(_soc, net_datapacket));
            switch( status_client )
            {
                case e_server::succ_server_send:
                    last_activity = std::chrono::steady_clock::now();
                    break;
                default:
                    if( this->isLog() ) LOG_MSG(
                        this->getLogger(),
                        utf::to_utf32("Data Couldn't Send To Ip Address (" + _ip + ") Status Code: " + std::to_string(static_cast<size_t>(status_client))),
                        test::e_status::error,
                        false
                    );

                    goto end_of_socket;
            }
        }

        end_of_socket:
            close_socket(_soc);

        {
            std::scoped_lock lock(this->mtx);
            this->clients.erase(_soc);
        }
    }

    /**
     * @brief [Public] On Crash
     * 
     * Çökme durumunda sunucuyu
     * güvenli bir şekilde kapatmayı sağlar
     */
    template<class Algo>
    void Server<Algo>::onCrash() noexcept
    {
        this->stop();
        this->close();
    }
}
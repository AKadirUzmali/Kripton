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
#include <Global.h>
#include <Socket/Socket.h>
#include <Socket/AccessPolicy.h>
#include <ThreadPool/ThreadPool.h>
#include <Handler/Crash/CrashBase.h>

#include <atomic>
#include <mutex>
#include <unordered_map>
#include <string>
#include <iostream>
#include <chrono>
#include <thread>
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
            err_server_stop_fail,
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
            err_socket_invalid_for_delete,
            err_client_delete,
            err_thread_create,
            err_server_running_status,
            err_server_wakeup_pipe,

            succ = unknwn + static_cast<glo::status_t>(glo::e_status::succ),
            succ_server_bind,
            succ_server_listen,
            succ_server_accept,
            succ_server_run,
            succ_set_wait_time,
            succ_set_running,
            succ_server_stop,
            succ_server_send,
            succ_server_recv,
            succ_client_delete,

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
            using work_handler = std::function<void(
                Server<Algo>&,
                const datapacket_t,
                const socket_t,
                const SocketCtx_t
            )>;

        private:
            ThreadPool& tpool;

            std::thread worker;
            std::atomic<bool> running { ATOMIC_VAR_INIT(false) };

            std::unordered_map<socket_t, SocketCtx_t> clients;

            std::mutex mtx;
            work_handler handler;

            int wakeup_pipe[2] { -1, -1 };

        private:
            void loop() noexcept;
            void client_worker(socket_t, const std::string&) noexcept;

            e_server bind() noexcept;
            e_server listen() noexcept;

        public:
            explicit Server
            (
                Algo& _algorithm,
                ThreadPool& _tpool,
                work_handler _handler,
                const socket_port_t _port,
                const std::u32string& _username = DEF_NICKNAME,
                const ipv_t _tcp_ip_type = ipv_t::dual,
                const bool _passwd_require = true,
                const bool _log = false,
                const std::u32string& _logheader = U"Server Log",
                const std::u32string& _logfilename = U"server_log",
                const size_t _buffsize = DEF_SIZE_BUFFER,
                const max_conn_t _max_conn = DEF_CONNECTION,
                const wait_time_t _wait_time = DEF_WAIT_TIME,
                const wait_time_t _timeout = DEF_SOCKET_TIMEOUT
            );
            
            ~Server();

            inline bool isRunning() const noexcept;

            inline std::unordered_map<socket_t, SocketCtx_t>& getClients() noexcept;
            inline const std::unordered_map<socket_t, SocketCtx_t>& getClients() const noexcept;

            void setHandler(work_handler& _handler) noexcept;

            e_server deleteClient(const socket_t) noexcept;

            virtual glo::status_t send(const socket_t, datapacket_t&) noexcept override;
            virtual glo::status_t receive(const socket_t, datapacket_t&) noexcept override;

            e_server run() noexcept;
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
     * @param Algo& Algorithm
     * @param ThreadPool& Thread Pool
     * @param work_handler Work Handler
     * @param socket_port_t Socket Port
     * @param u32string& Username
     * @param ipv_t Socket Tcp Ip Type (v4, v6)
     * @param bool Is Password Require
     * @param bool Is Logging Active
     * @param u32string& Log File Header
     * @param u32string& Log File Name
     * @param buffer_size_t Buffer Size
     * @param wait_time_t Wait Time For Loop
     * @param wait_time_t Socket Timeout
     */
    template<class Algo>
    Server<Algo>::Server(
        Algo& _algorithm,
        ThreadPool& _tpool,
        work_handler _handler,
        const socket_port_t _port,
        const std::u32string& _username,
        const ipv_t _tcp_ip_type,
        const bool _passwd_require,
        const bool _log,
        const std::u32string& _logheader,
        const std::u32string& _logfilename,
        const size_t _buffsize,
        const max_conn_t _max_conn,
        const wait_time_t _wait_time,
        const wait_time_t _timeout
    )
    : Socket<Algo>(
        _algorithm,
        _username,
        _port,
        _tcp_ip_type,
        _log,
        _logheader,
        _logfilename,
        _buffsize,
        _wait_time,
        _timeout),
      tpool(_tpool)
    {
        this->getPolicy().enablePassword(_passwd_require);
        this->getPolicy().setMaxConnection(_max_conn);
        this->setHandler(_handler);
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
     * @brief [Public] Get Clients
     * 
     * Tüm istemcilerin tutulduğu listeyi
     * referans olarak döndürmeyi sağlar. İstendiği
     * durumda istenilen istemci ile iletişim kurabilmeyi
     * sağlamak için yapıldı
     * 
     * @return unordered_map<socket_t, SocketCtx_t>&
     */
    template<class Algo>
    std::unordered_map<socket_t, SocketCtx_t>& Server<Algo>::getClients() noexcept
    {
        return this->clients;
    }

    /**
     * @brief [Public] Get Clients
     * 
     * Tüm istemcilerin tutulduğu listeyi
     * referans olarak döndürmeyi sağlar. İstendiği
     * durumda istenilen istemcinin okunabilmesi ama
     * değerin değiştirilememesi için yapıldı
     * 
     * @return const unordered_map<socket_t, SocketCtx_t>&
     */
    template<class Algo>
    const std::unordered_map<socket_t, SocketCtx_t>& Server<Algo>::getClients() const noexcept
    {
        return this->clients;
    }

    /**
     * @brief [Public] Delete Client
     * 
     * İstemciyi belirtilmiş sokete göre listede
     * arayacak ve bulunması durumunda silecek
     * ve toplam soket sayısında eksilme yapacak
     * 
     * @param socket_t Socket
     * @return e_server
     */
    template<class Algo>
    e_server Server<Algo>::deleteClient
    (
        const socket_t _soc
    ) noexcept
    {
        if( !is_socket_valid(_soc) )
            return e_server::err_socket_invalid_for_delete;

        std::scoped_lock<std::mutex> lock(this->mtx);
        return this->clients.erase(_soc) ?
            e_server::succ_client_delete :
            e_server::err_client_delete;
    }

    /**
     * @brief [Public] Set Handler
     * 
     * Çalışma işlevini gerçekleştirecek yapıyı
     * atayacak.
     * 
     * @param work_handler& Handler
     */
    template<class Algo>
    void Server<Algo>::setHandler(work_handler& _handler) noexcept
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
        if( !is_socket_valid(_socket) )
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
        if( !is_socket_valid(_socket) )
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
        sockaddr* addr = nullptr;
        socklen_t len_addr = 0;

        if( this->getIpv() == ipv_t::ipv4 )
        {
            sockaddr_in addrv4 {};
            addrv4.sin_family = AF_INET;
            addrv4.sin_addr.s_addr = INADDR_ANY;
            addrv4.sin_port = htons(this->getPort());

            addr = reinterpret_cast<sockaddr*>(&addrv4);
            len_addr = sizeof(addrv4);
        }
        else
        {
            sockaddr_in6 addrv6 {};
            addrv6.sin6_family = AF_INET6;
            addrv6.sin6_port = htons(this->getPort());
            addrv6.sin6_addr = in6addr_any;

            addr = reinterpret_cast<sockaddr*>(&addrv6);
            len_addr = sizeof(addrv6);
        }

        int result = ::bind(this->getSocket(), addr, len_addr);
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
     * @return e_server
    */
    template<class Algo>
    e_server Server<Algo>::run() noexcept
    {
        if( this->isRunning() )
            return e_server::err_server_already_running;

        if( this->create() != e_socket::succ_socket_create )
            return e_server::err_server_not_created;

        if( this->bind() != e_server::succ_server_bind )
            return e_server::err_server_bind_fail;

        if( this->listen() != e_server::succ_server_listen )
            return e_server::err_server_listen_fail;

        if( ::pipe(this->wakeup_pipe) != 0 )
            return e_server::err_server_wakeup_pipe;

        this->running.store(true);

        try
        {
            this->worker = std::thread([this]{
                this->loop();
            });
        }
        catch (...)
        {
            this->running.store(false);
            return e_server::err_thread_create;
        }

        return e_server::succ_server_run;
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
        if( !this->running.exchange(false) )
            return e_server::err_server_running_status;

        uint8_t sig = 1;
        ::write(this->wakeup_pipe[1], &sig, sizeof(sig));

        if( this->worker.joinable() )
            this->worker.join();

        close_socket(this->wakeup_pipe[0]);
        close_socket(this->wakeup_pipe[1]);

        this->wakeup_pipe[0] = -1;
        this->wakeup_pipe[1] = -1;

        const socket_t srv_socket = this->getSocket();
        #if defined __PLATFORM_DOS__
            ::shutdown(srv_socket, SD_BOTH);
            ::closesocket(srv_socket);
        #else
            ::shutdown(srv_socket, SHUT_RDWR);
            ::close(srv_socket);
        #endif

        this->setSocket(inv_socket);
        return e_server::succ_server_stop;
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
        const socket_t listen_fd = this->getSocket();
        if( !is_socket_valid(listen_fd) )
            return;

        pollfd pfds[2];

        pfds[0].fd = listen_fd;
        pfds[0].events = POLL_IN;

        pfds[1].fd = this->wakeup_pipe[0];
        pfds[1].events = POLL_IN;

        const int wait_timeout = static_cast<int>(this->getWaitTime());

        while( this->running.load() )
        {
            int pret = ::poll(pfds, 2, wait_timeout);
            if( pret <= 0 )
                continue;

            if( !(pfds[1].revents & POLL_IN) )
                continue;

            if( !(pfds[0].revents & POLL_IN) )
                continue;

            sockaddr_storage socstrage {};
            socklen_t slen = sizeof(socstrage);
            socklen_t cli_soc = ::accept(listen_fd, reinterpret_cast<sockaddr*>(&socstrage), &slen);

            if( !is_socket_valid(cli_soc) )
                continue;

            const std::string ipaddr = get_ip(cli_soc);

            {
                std::scoped_lock lock(this->mtx);

                size_t count_same_ip = 0;
                for( const auto& [sock, ctx] : this->clients ) {
                    if( ctx.ip == ipaddr )
                        ++count_same_ip;
                }

                if( count_same_ip > this->getPolicy().getMaxSameIp() ||
                    this->clients.size() >= this->getPolicy().getMaxConnection() )
                {
                    close_socket(cli_soc);
                    continue;
                }

                this->clients.emplace(
                    cli_soc,
                    SocketCtx_t{ .ip = ipaddr, UserPacket_t{} }
                );
            }

            this->tpool.enqueue([this, cli_soc, ipaddr]{
                this->client_worker(cli_soc, ipaddr);
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
                    break;

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

            if( status_client != e_server::succ_server_recv )
                continue;

            SocketCtx_t curr_client;
            {
                std::scoped_lock lock(this->mtx);

                auto client_it = this->clients.find(_soc);
                if( client_it == this->clients.end() )
                    goto end_of_socket;

                curr_client = client_it->second;
            }

            if( this->handler )
                this->handler(*this, net_datapacket, _soc, curr_client);

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
            this->running.store(false);
            close_socket(_soc);

        this->deleteClient(_soc);
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
        Socket<Algo>::onCrash();
        this->stop();
    }
}
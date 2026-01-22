// Abdulkadir U. - 16/12/2025
#pragma once

/**
 * Client (İstemci)
 * 
 * Soket işlemlerinin 2. kısmına geldik,
 * yani client (istemci) kısmı. Bu kısımda
 * birden çok sunucuya bağlantı sağlayabilen
 * bir istemci yapısı olacak çünkü bir istemci
 * istediği sunucuya bağlantıya aynı anda
 * bağlantı sağlama hakkına sahip.
 * 
 * Sunucu bağlantısı sağlayan, veri göndermeyi sağlayan
 * bir yapı oluşturacağız, bu sayede soket
 * işlemleri tamamlanmış olacak.
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
    // Namespace: Client
    namespace client
    {
        // Windows / DOS (Winsock)
        #if defined __PLATFORM_DOS__
            static inline constexpr int inv_connect = SOCKET_ERROR;
        #elif defined __PLATFORM_POSIX__
            static inline constexpr int inv_connect = -1;
        #endif

        // Enum Class: Client Code
        enum class e_client : size_t
        {
            unknwn = static_cast<glo::status_t>(glo::e_status_t::client),

            err = unknwn + static_cast<glo::status_t>(glo::e_status::err),
            err_client_close_fail,
            err_client_already_running,
            err_set_running_failed,
            err_client_not_created,
            err_client_not_connected,
            err_connect_addrinfo_fail,
            err_client_connection_worker,
            err_ip_for_server_is_empty,
            err_set_server_ip,
            err_connect_invalid_ipv4,
            err_connect_invalid_ipv6,

            succ = unknwn + static_cast<glo::status_t>(glo::e_status::succ),
            succ_client_stop,
            succ_client_connect,
            succ_client_run,
            succ_set_server_ip,

            warn = unknwn + static_cast<glo::status_t>(glo::e_status::warn),
            warn_no_client,
        };

        constexpr bool operator==(glo::status_t _first, e_client _second) noexcept { return _first == static_cast<glo::status_t>(_second); }
        constexpr bool operator!=(glo::status_t _first, e_client _second) noexcept { return _first != static_cast<glo::status_t>(_second); }
    }

    // Using Namespace:
    using namespace core::handler;
    using namespace client;

    // Class: Client
    template<class Algo = Algorithm>
    class Client final : public Socket<Algo>, public virtual CrashBase
    {
        public:
            using work_handler = std::function<void(Client&, const datapacket_t)>;
            using work_sender = std::function<void(Client&, const datapacket_t, const bool)>;

        private:
            ThreadPool& tpool;

            std::atomic<e_client> status { ATOMIC_VAR_INIT(e_client::warn_no_client) };

            std::atomic<bool> running { ATOMIC_VAR_INIT(false) };

            std::string servipaddr;

            std::mutex climtx;
            work_handler handler {};
            work_sender sender {};

            std::u32string msg;

        private:
            e_client connect() noexcept;
            void connection_worker() noexcept;

        public:
            explicit Client
            (
                Algo& _algorithm,
                ThreadPool& _tpool,
                work_handler _handler,
                work_sender _sender,
                const std::string& _servip,
                const socket_port_t _port,
                const std::u32string& _username = U"Client",
                const ipv_t _tcp_ip_type = ipv_t::dual,
                const bool _log = false,
                const std::u32string& _logheader = U"Client Log",
                const std::u32string& _logfilename = U"client_log",
                const size_t _buffsize = DEF_SIZE_BUFFER,
                const wait_time_t _wait_time = DEF_WAIT_TIME,
                const wait_time_t _timeout = DEF_SOCKET_TIMEOUT
            );

            ~Client();

            inline bool isRunning() const noexcept;

            inline e_client getStatus() const noexcept;

            inline const std::string& getServerIp() const noexcept;
            e_client setServerIp(const std::string&) noexcept;

            void setHandler(work_handler& _handler) noexcept;
            void setSender(work_sender& _sender) noexcept;

            bool setMsgForSend(const std::u32string&) noexcept;

            e_client run() noexcept;
            e_client stop() noexcept;

            virtual void onCrash() noexcept override;
    };

    /**
     * @brief [Public] Constructor
     */
    template<class Algo>
    Client<Algo>::Client
    (
        Algo& _algorithm,
        ThreadPool& _tpool,
        work_handler _handler,
        work_sender _sender,
        const std::string& _servip,
        const socket_port_t _port,
        const std::u32string& _username,
        const ipv_t _tcp_ip_type,
        const bool _log,
        const std::u32string& _logheader,
        const std::u32string& _logfilename,
        const size_t _buffsize,
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
        this->setServerIp(_servip);
        this->getPolicy().setMaxConnection(MIN_CONNECTION);
        this->setHandler(_handler);
        this->setSender(_sender);
    }

    /**
     * @brief [Public] Destrcutor
     */
    template<class Algo>
    Client<Algo>::~Client()
    {
        this->stop();
    }

    /**
     * @brief [Public] Set Handler
     * 
     * Çalışma işlevini gerçekleştirecek yapıyı atayacak
     * 
     * @param work_handler& Handler
     */
    template<class Algo>
    void Client<Algo>::setHandler(work_handler& _handler) noexcept
    {
        this->handler = std::move(_handler);
    }

    /**
     * @brief [Public] Set Sender
     * 
     * Mesaj gönderimi yapıldıktan sonra
     * işlevini gerçekleştirecek yapıyı atayacak
     * 
     * @param work_sender& Sender
     */
    template<class Algo>
    void Client<Algo>::setSender(work_sender& _sender) noexcept
    {
        this->sender = std::move(_sender);
    }

    /**
     * @brief [Public] Set Message For Send
     * 
     * Soket de göndereceğimiz mesaj verisini ayarlayan
     * basit bir fonksiyon. Bu fonksiyon sayesinde
     * client run fonksiyonu veri gönderebilecek
     * 
     * @param u32string& Data
     * @return bool
     */
    template<class Algo>
    bool Client<Algo>::setMsgForSend(const std::u32string& _data) noexcept
    {
        if( _data.empty() )
            return false;

        this->msg = _data;
        return this->msg == _data;
    }

    /**
     * @brief [Public] Is Running
     * 
     * Sunucunun çalışıp çalımadığının bilgisini döndürür
     * 
     * @return bool
     */
    template<class Algo>
    bool Client<Algo>::isRunning() const noexcept
    {
        return this->running.load();
    }

    /**
     * @brief [Public] Get Status
     * 
     * Sunucuya ait durum kodunu döndürür
     * 
     * @return e_client
     */
    template<class Algo>
    e_client Client<Algo>::getStatus() const noexcept
    {
        return this->status.load();
    }

    /**
     * @brief [Public] Get Server Ip Address
     * 
     * Bağlanmak istenilen sunucuya ait ip adresini
     * döndürerek bilgiye erişmeyi sağlar
     * 
     * @return const string&
     */
    template<class Algo>
    const std::string& Client<Algo>::getServerIp() const noexcept
    {
        return this->servipaddr;
    }

    /**
     * @brief [Public] Set Server Ip Address
     * 
     * Bağlanmak istenilen sunucuya ait ip adresini
     * alarak bağlanmak istenen sunucu için ip adresini
     * sınıf içinde ayarlar
     * 
     * @param string& Server Ip Address
     * @return e_client
     */
    template<class Algo>
    e_client Client<Algo>::setServerIp(const std::string& _servip) noexcept
    {
        if( _servip.empty() )
            return e_client::err_ip_for_server_is_empty;

        this->servipaddr = _servip;
        return this->servipaddr == _servip ?
            e_client::succ_set_server_ip :
            e_client::err_set_server_ip;
    }

    /**
     * @brief [Public] Connect
     * 
     * Sunucuya bağlanmamızı sağlayacak ve
     * ipv4 ile ipv6 için uyumlu hale getirildi
     * 
     * @return e_client
     */
    template<class Algo>
    e_client Client<Algo>::connect() noexcept
    {
        if( !is_socket_valid(this->getSocket()) )
            return e_client::err_client_not_created;

        const socket_t cli_soc = this->getSocket();
        const std::string ipaddr = this->getServerIp();
        const socket_port_t cli_port = this->getPort();
        const std::string port_str = std::to_string(cli_port);

        sockaddr* addr = nullptr;
        socklen_t len_addr = 0;

        if( this->getIpv() == ipv_t::ipv4 )
        {
            sockaddr_in addrv4 {};
            addrv4.sin_family = AF_INET;
            addrv4.sin_port = htons(this->getPort());

            if( ::inet_pton(addrv4.sin_family, ipaddr.c_str(), &addrv4.sin_addr) != 1 )
                return e_client::err_connect_invalid_ipv4;

            addr = reinterpret_cast<sockaddr*>(&addrv4);
            len_addr = sizeof(addrv4);
        }
        else
        {
            sockaddr_in6 addrv6 {};
            addrv6.sin6_family = AF_INET6;
            addrv6.sin6_port = htons(this->getPort());

            if( ::inet_pton(addrv6.sin6_family, ipaddr.c_str(), &addrv6.sin6_addr) != 1 )
                return e_client::err_connect_invalid_ipv6;

            addr = reinterpret_cast<sockaddr*>(&addrv6);
            len_addr = sizeof(addrv6);
        }

        if( ::connect(cli_soc, addr, len_addr) == inv_connect )
        {
            #ifdef __PLATFORM_DOS__
                int err = WSAGetLastError();
                std::cerr << "connect error: " << err << std::endl;
            #endif

            LOG_MSG(this->getLogger(), U"Failed To Connect Server | Code: ", test::e_status::error, true);
            return e_client::err_client_not_connected;
        }

        return e_client::succ_client_connect;

        /*
        #ifdef __PLATFORM_POSIX__
            int flags = ::fcntl(cli_soc, F_GETFL, 0);
            if( flags < 0 )
                return e_client::err_client_not_created;

            ::fcntl(cli_soc, F_SETFL, flags | O_NONBLOCK);
        #elif defined(__PLATFORM_DOS__)
            unsigned long mode = 1;
            ::ioctlsocket(cli_soc, FIONBIO, &mode);
        #endif

        addrinfo addr_info {};
        switch( this->getIpv() )
        {
            case ipv_t::ipv4:
                addr_info.ai_family = tcp::ipv4::domain;
                break;
            case ipv_t::ipv6:
                addr_info.ai_family = tcp::ipv6::domain;
                break;
            case ipv_t::dual:
            default:
                addr_info.ai_family = AF_UNSPEC;
        }

        // addr_info.ai_socktype = tcp::ipv6::type;

        addr_info.ai_socktype = SOCK_STREAM;
        addr_info.ai_protocol = IPPROTO_TCP;
        addr_info.ai_flags = AI_NUMERICSERV | AI_NUMERICHOST;

        addrinfo* res = nullptr;
        int getai = ::getaddrinfo(ipaddr.c_str(), port_str.c_str(), &addr_info, &res);
        if( getai != 0 )
            return e_client::err_connect_addrinfo_fail;

        for( addrinfo* ptr = res; ptr; ptr = ptr->ai_next )
        {
            int conn = ::connect(cli_soc, ptr->ai_addr, ptr->ai_addrlen);
            if( conn == 0 )
            {
                ::freeaddrinfo(res);
                return e_client::succ_client_connect;
            }

            #ifdef __PLATFORM_DOS__
                const int err = ::WSAGetLastError();
                if( err != WSAEWOULDBLOCK && err != WSAEINPROGRESS )
                    continue;
            #elif defined(__PLATFORM_POSIX__)
                if( errno != EINPROGRESS )
                    continue;
            #endif

            pollfd pfd {};
            pfd.fd = cli_soc;
            pfd.events = POLL_OUT;

            const int pres = ::poll(&pfd, 1, wait_timeout);
            if( pres <= 0 )
                continue;

            int so_error = 0;
            socklen_t slen = sizeof(so_error);
            ::getsockopt(cli_soc, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&so_error), &slen);

            if( so_error == 0 )
            {
                ::freeaddrinfo(res);
                return e_client::succ_client_connect;
            }
        }

        ::freeaddrinfo(res);
        return e_client::err_client_not_connected;
        */
    }

    /**
     * @brief [Public] Run
     * 
     * İstemciyi çalıştıracak olan fonksiyondur
     * Gerekli kontrolü yaptıktan sonra istemci
     * döngü halinde çalışır
     * 
     * @return e_client
    */
    template<class Algo>
    e_client Client<Algo>::run() noexcept
    {
        if( this->isRunning() )
            return e_client::err_client_already_running;

        if( this->create() != e_socket::succ_socket_create )
            return e_client::err_client_not_created;

        if( this->connect() != e_client::succ_client_connect )
            return e_client::err_client_not_connected;

        this->running.store(true);

        try
        {
            this->tpool.enqueue([this]{
                this->connection_worker();
            });
        }
        catch (...)
        {
            this->running.store(false);
            return e_client::err_client_connection_worker;
        }

        return e_client::succ_client_run;
    }

    /**
     * @brief [Public] Stop
     * 
     * İstemci çalışıyorsa eğer durduracaktır ve
     * gerekli kontrolleri yaparak bunu güvenli şekilde
     * yapar
     * 
     * @return e_server
     */
    template<class Algo>
    e_client Client<Algo>::stop() noexcept
    {
        this->running.store(false);
        
        const socket_t cli_socket = this->getSocket();
        if( is_socket_valid(cli_socket) )
        {
            #if defined __PLATFORM_DOS__
                ::shutdown(cli_socket, SD_BOTH);
                ::closesocket(cli_socket);
            #else
                ::shutdown(cli_socket, SHUT_RDWR);
                ::close(cli_socket);
            #endif
        }

        this->setSocket(inv_socket);
        return e_client::succ_client_stop;
    }

    /**
     * @brief [Public] Connection Worker
     * 
     * İstemci sunucudan belirli bir gecikme ile
     * sürekli veri almayı dener
     */
    template<class Algo>
    void Client<Algo>::connection_worker() noexcept
    {
        const socket_t cli_soc = this->getSocket();
        if( !is_socket_valid(cli_soc) )
            return;

        LOG_MSG(this->getLogger(), U"[connection_worker] test 1", test::e_status::information, true);

        const wait_time_t loop_timeout = this->getWaitTime();
        const std::string ipaddr = get_ip(cli_soc);
    
        LOG_MSG(this->getLogger(), U"[connection_worker] test 2", test::e_status::information, true);

        auto last_activity = std::chrono::steady_clock::now();

        datapacket_t net_datapacket;
        e_socket status_server;

        LOG_MSG(this->getLogger(), U"[connection_worker] test 3", test::e_status::information, true);

        net_datapacket.name = U"Client_Test_Name";
        net_datapacket.pwd = U"Password@123!-_üçşğ_test";
        net_datapacket.msg = U"Hello Server 😁, It's Client! 😌";

        while( this->running.load() )
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(loop_timeout));

            if( this->msg.empty() )
                goto client_job_receive;

            LOG_MSG(this->getLogger(), U"[connection_worker] test 9", test::e_status::information, true);

            status_server = glo::to_status<e_socket>(this->send(cli_soc, net_datapacket));

            LOG_MSG(this->getLogger(), U"[connection_worker] test 10", test::e_status::information, true);

            switch( status_server )
            {
                case e_socket::succ_socket_send:
                    break;

                case e_socket::err_packet_no_data:
                    break;
                
                default:
                    LOG_MSG(this->getLogger(), U"[client_worker] send error: " + utf::to_utf32(std::to_string(static_cast<size_t>(status_server))), test::e_status::error, true);
                    goto end_of_socket;
            }

            LOG_MSG(this->getLogger(), U"[connection_worker] test 11", test::e_status::information, true);

            if( status_server != e_socket::succ_socket_send )
                continue;

            LOG_MSG(this->getLogger(), U"[connection_worker] test 12", test::e_status::information, true);

            last_activity = std::chrono::steady_clock::now();

            if( this->isLog() ) LOG_MSG(this->getLogger(),
                U"Client data sented to (" + utf::to_utf32(this->getServerIp()) + U") server",
                test::e_status::information,
                true
            );

            LOG_MSG(this->getLogger(), U"[connection_worker] test 13", test::e_status::information, true);

            if( this->sender )
                this->sender(*this, net_datapacket, status_server == e_socket::succ_socket_send);

            LOG_MSG(this->getLogger(), U"[connection_worker] test 14", test::e_status::information, true);

            client_job_receive:
                status_server = glo::to_status<e_socket>(this->receive(cli_soc, net_datapacket));

                LOG_MSG(this->getLogger(), U"[connection_worker] test 4", test::e_status::information, true);

                switch( status_server )
                {
                    case e_socket::succ_socket_recv:
                        break;

                    case e_socket::err_recv_message_empty:
                    case e_socket::err_recv_username_empty:
                        break;

                    default:
                        LOG_MSG(this->getLogger(), U"[client_worker] recv error: " + utf::to_utf32(std::to_string(static_cast<size_t>(status_server))), test::e_status::error, true);
                        goto end_of_socket;
                }

                LOG_MSG(this->getLogger(), U"[connection_worker] test 5", test::e_status::information, true);

                if( status_server != e_socket::succ_socket_recv )
                    continue;

                LOG_MSG(this->getLogger(), U"[connection_worker] test 6", test::e_status::information, true);

                if( this->isLog() ) LOG_MSG(this->getLogger(),
                    U"Client data received from (" + utf::to_utf32(this->getServerIp()) + U") server",
                    test::e_status::information,
                    true
                );

                LOG_MSG(this->getLogger(), U"[connection_worker] test 7", test::e_status::information, true);

                last_activity = std::chrono::steady_clock::now();

                if( this->handler )
                    this->handler(*this, net_datapacket);

                LOG_MSG(this->getLogger(), U"[connection_worker] test 8", test::e_status::information, true);
        }

        end_of_socket:
            this->running.store(false);
            close_socket(cli_soc);

        LOG_MSG(this->getLogger(), U"[connection_worker] test 15", test::e_status::information, true);
    }

    /**
     * @brief [Public] On Crash
     * 
     * Çökme durumunda sunucuyu
     * güvenli bir şekilde kapatmayı sağlar
     */
    template<class Algo>
    void Client<Algo>::onCrash() noexcept
    {
        this->stop();
        this->close();
    }
}
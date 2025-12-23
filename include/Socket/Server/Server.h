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
        // Using:
        using wait_time_t = uint16_t;

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
        enum class e_server : size_t
        {
            err = 1000,
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

            succ = 2000,
            succ_server_bind,
            succ_server_listen,
            succ_server_accept,
            succ_server_run,
            succ_set_wait_time,
            succ_set_running,
            succ_server_close,
            succ_server_send,
            succ_server_recv,

            warn = 3000,
            warn_no_status,
            warn_server_run_status_same,
            warn_server_already_stop,
            warn_socket_can_be_close,
            warn_server_send_not_all_data,
            warn_recv_wrong_password
        };
    }

    // Using Namespace:
    using namespace core::handler;
    using namespace server;

    // Class: Server
    template<class Algo = Algorithm>
    class Server final : public Socket<Algo, e_server>
    {
        public:
            using work_handler = std::function<e_server(NetPacket&)>;

        private:
            ThreadPool& tpool;

            mutable std::shared_future<e_server> frun {};

            std::atomic<e_server> status { e_server::warn_no_status };

            std::atomic<bool> running { false };
            std::atomic<wait_time_t> wait_us { DEF_WAIT_TIME };

            std::unordered_map<std::string, UserPacket_t> clients;

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
                Algo&& _algorithm,
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

            virtual auto send(const socket_t, NetPacket&) noexcept -> e_server override;
            virtual auto receive(const socket_t, NetPacket&) noexcept -> e_server override;

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
        Algo&& _algorithm,
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
    : Socket<Algo, e_server>(
        std::forward<Algo>(_algorithm),
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
     * @param NetPacket& NetPack
     * 
     * @return e_socket
     */
    template<class Algo>
    auto Server<Algo>::send(const socket_t _socket, NetPacket& _netpack) noexcept -> e_server
    {
        if( _socket == inv_socket )
            return e_server::err_client_socket_invalid;

        const std::string ipaddr = getIP(_socket);
        const auto findip = this->clients.find(ipaddr);

        if( findip == this->clients.end() )
            return e_server::err_socket_not_found_in_list;
        // else if( !this->getPolicy().isBanned(ipaddr) )
        //     return e_server::err_socket_banned;

        if( _netpack.get().empty() )
            return e_server::err_client_send_no_data;

        const std::u32string tmp_message = utf::to_utf32(_netpack.getMessage());
        NetPacket tmp_netpack(this->getPolicy().getPassword(), this->getPolicy().getUsername(), tmp_message);

        const size_t pack_size = tmp_netpack.get().size();
        if( pack_size > SIZE_OVER_SOCKET )
            return e_server::err_send_pack_over_size;

        int send_bytes = ::send(_socket, tmp_netpack.get().data(), static_cast<int>(pack_size), 0);

        if( send_bytes < 0 )
            return e_server::err_server_send;
        else if( send_bytes == 0 )
            return e_server::warn_socket_can_be_close;
        else if( send_bytes < static_cast<int>(pack_size) )
            return e_server::warn_server_send_not_all_data;

        if( this->isLog() )
            LOG_MSG(this->getLogger(), utf::to_utf32(_netpack.getMessage()), test::e_status::information, false);

        return send_bytes >= static_cast<int>(pack_size) ?
            e_server::succ_server_send :
            e_server::err_server_send;
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
     * @param NetPacket& NetPack
     * 
     * @return e_server
     */
    template<class Algo>
    auto Server<Algo>::receive(const socket_t _socket, NetPacket& _netpack) noexcept -> e_server
    {
        if( _socket == inv_socket )
            return e_server::err_client_socket_invalid;

        const std::string ipaddr = getIP(_socket);
        const auto findip = this->clients.find(ipaddr);

        if( findip == this->clients.end() )
            return e_server::err_socket_not_found_in_list;
        // else if( !this->getPolicy().isBanned(ipaddr) )
        //     return e_server::err_socket_banned;

        if( _netpack.get().empty() )
            return e_server::err_client_send_no_data;

        int recv_bytes = ::recv(_socket, _netpack.get().data(), static_cast<int>(_netpack.get().size()), 0);

        if( recv_bytes == 0 )
            return e_server::err_recv_socket_closed;
        else if( recv_bytes < 0 )
            return e_server::err_recv_server;
        else if( recv_bytes > static_cast<int>(SIZE_OVER_SOCKET) ) {
            this->getPolicy().ban(ipaddr);
            return e_server::err_recv_over_limit_and_banned;
        }

        std::u32string u32passwd = utf::to_utf32(_netpack.getPassword());
        this->getAlgorithm().decrypt(u32passwd);

        if( u32passwd != this->getPolicy().getPassword() )
        {
            this->clients[ipaddr].try_passwd++;

            if(this->clients[ipaddr].try_passwd == static_cast<uint16_t>(MAX_SOCKET_RETRY_PASSWD) )
                return e_server::err_socket_banned;
            else if( this->clients[ipaddr].try_passwd > static_cast<uint16_t>(MAX_SOCKET_RETRY_PASSWD) )
                return e_server::err_socket_potential_attacker;

            return e_server::warn_recv_wrong_password;
        }
        else if( u32passwd == this->getPolicy().getOldPassword() )
        {
            this->clients[ipaddr].try_passwd++;
            return e_server::err_socket_password_changed;
        }

        std::u32string u32username = utf::to_utf32(std::to_string(this->clients[ipaddr].same_user_count)) + this->clients[ipaddr].username;
        _netpack.pack(U" ", u32username, utf::to_utf32(_netpack.getMessage()));

        return e_server::succ_server_recv;
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
        int backlog = static_cast<int>(this->getPolicy().getMaxConnection());
        if( backlog < static_cast<int>(MIN_CONNECTION) )
            backlog = static_cast<int>(DEF_CONNECTION);

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
            this->tpool.enqueue([this]{ this->loop(); });

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
        const auto timeout = static_cast<long>(std::max<size_t>(MIN_WAIT_TIME, this->wait_us.load()));
        
        while( this->isRunning() )
        {
            std::this_thread::sleep_for(std::chrono::microseconds(timeout));

            if( this->getSocket() == inv_socket )
                break;

            sockaddr_in servaddr {};
            socklen_t servlen = sizeof(servaddr);
            socket_t servaccpt = ::accept(this->getSocket(), reinterpret_cast<sockaddr*>(&servaddr), &servlen);

            if( servaccpt == inv_accept )
                continue;

            std::string ipaddr = inet_ntoa(servaddr.sin_addr);
            if( this->getPolicy().isBanned(ipaddr) ) {
                close_socket(servaccpt);
                continue;
            }

            {
                std::scoped_lock<std::mutex> lock(this->mtx);

                if( this->clients.size() >= this->getPolicy().getMaxConnection() ) {
                    close_socket(servaccpt);
                    continue;
                }

                this->clients[ipaddr].try_passwd = 0;
                this->clients[ipaddr].username = this->getPolicy().getUsername();
                this->clients[ipaddr].same_user_count = 0;

                for( auto& [ip, datapack] : this->clients ) {
                    if( ipaddr == ip )
                        continue;

                    if( this->clients[ipaddr].username == datapack.username )
                        this->clients[ipaddr].same_user_count++;
                }
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
        int opt = 1;
        ::setsockopt(_soc, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));

        const size_t timeout = static_cast<size_t>(std::max<size_t>(MIN_WAIT_TIME, this->wait_us.load()));

        NetPacket netpack;
        e_server status_client;

        while( !this->getPolicy().isBanned(_ip) && this->isRunning() )
        {
            std::this_thread::sleep_for(std::chrono::microseconds(timeout));

            status_client = this->receive(_soc, netpack);
            
            if( status_client != e_server::succ_server_recv && !this->isLog() )
                break;

            switch( status_client )
            {
                case e_server::succ_server_recv:
                    continue;
                case e_server::err_client_socket_invalid:
                    LOG_MSG(this->getLogger(), utf::to_utf32(_ip + " ip address socket not valid"), test::e_status::error, false);
                    break;
                case e_server::err_socket_not_found_in_list:
                    LOG_MSG(this->getLogger(), utf::to_utf32(_ip + " ip address not found in client list"), test::e_status::error, false);
                    break;
                case e_server::err_socket_banned:
                    LOG_MSG(this->getLogger(), utf::to_utf32(_ip + " ip address banned"), test::e_status::error, false);
                    break;
                case e_server::err_client_send_no_data:
                    LOG_MSG(this->getLogger(), utf::to_utf32(_ip + " ip address send no data"), test::e_status::error, false);
                    break;
                case e_server::err_recv_server:
                    LOG_MSG(this->getLogger(), utf::to_utf32(_ip + " server recv error"), test::e_status::error, false);
                    break;
                case e_server::err_recv_over_limit_and_banned:
                    LOG_MSG(this->getLogger(), utf::to_utf32(_ip + " ip address sent data over limit and banned"), test::e_status::error, false);
                    break;
                case e_server::err_socket_potential_attacker:
                    LOG_MSG(this->getLogger(), utf::to_utf32(_ip + " ip address is potential attacker"), test::e_status::error, false);
                    break;
                case e_server::warn_recv_wrong_password:
                    LOG_MSG(this->getLogger(), utf::to_utf32(_ip + " ip address sent wrong password"), test::e_status::error, false);
                    break;
                default:
                    LOG_MSG(this->getLogger(), U"Unknown problem occured", test::e_status::unknown, false);
                    break;
            }

            if( this->handler )
                this->handler(netpack);

            std::u32string u32pack;
            netpack.copy(u32pack);

            this->getAlgorithm().encrypt(u32pack);
            netpack.pack(u32pack);

            status_client = this->send(_soc, netpack);

            if( status_client != e_server::succ_server_send && !this->isLog() )
                break;

            switch( status_client )
            {
                case e_server::succ_server_send:
                    continue;
                case e_server::err_client_socket_invalid:
                    LOG_MSG(this->getLogger(), utf::to_utf32(_ip + " ip address socket not valid"), test::e_status::error, false);
                    break;
                case e_server::err_socket_not_found_in_list:
                    LOG_MSG(this->getLogger(), utf::to_utf32(_ip + " ip address not found in client list"), test::e_status::error, false);
                    break;
                case e_server::err_socket_banned:
                    LOG_MSG(this->getLogger(), utf::to_utf32(_ip + " ip address banned"), test::e_status::error, false);
                    break;
                case e_server::err_client_send_no_data:
                    LOG_MSG(this->getLogger(), utf::to_utf32(_ip + " ip address send no data"), test::e_status::error, false);
                    break;
                case e_server::err_send_pack_over_size:
                    LOG_MSG(this->getLogger(), utf::to_utf32(_ip + " ip address sent packet over size limit"), test::e_status::error, false);
                    break;
                case e_server::warn_socket_can_be_close:
                    LOG_MSG(this->getLogger(), utf::to_utf32(_ip + " ip address socket can be close"), test::e_status::warning, false);
                    break;
                case e_server::warn_server_send_not_all_data:
                    LOG_MSG(this->getLogger(), utf::to_utf32(_ip + " Server couldn't send all data"), test::e_status::warning, false);
                    break;
                case e_server::err_server_send:
                    LOG_MSG(this->getLogger(), utf::to_utf32(_ip + " Server data send process failed"), test::e_status::error, false);
                    break;
                default:
                    LOG_MSG(this->getLogger(), U"Unknown problem occured", test::e_status::unknown, false);
                    break;
            }

            if( status_client != e_server::succ_server_send )
                break;
        }

        close_socket(_soc);

        {
            std::scoped_lock<std::mutex> lock(this->mtx);
            this->clients.erase(_ip);
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
// Abdulkadir U. - 2026/02/27
#pragma once

/**
 * Server (Sunucu)
 * 
 * Soket işlemlerinde iki adet işlem vardır:
 * 
 * 1) Sunucu (Server)
 * 2) İstemci (Client)
 * 
 * Bizim sunucumuz bir şifreli veri iletim sunucusu.
 * Aldığı ve ilettiği verilerin hepsi şifrelidir.
 * Verilen şifreleme türü ile veri alırken ve gönderirken
 * gerekli şifreleme ve şifre çözme işlemlerini yapar.
 * 
 * Sunucu sadece tek bir istemci değil, birden çok istemciyi
 * işleme sokabilecek şekilde tasarlandı. İlk önce istemci
 * bağlantısı sağlanır ve bağlantı kabulü yapılır.
 * Bağlantı kabulü sonrası engellenmiş listede olup olmadığı
 * ve izin verilmiş listede olup olmadığı da kontrol edilir.
 * Engelli listede değilse ve izin verilmiş ise işleme devam
 * eder, aksi halde istemci ile bağlantıyı kapatır ve bildiri
 * göndermez. Bunun amacı ise, istemcinin izin verilmemiş ya da
 * engelli olduğunu anlamaması için çünkü aksi durumda
 * istemcinin farklı güvenlik açığı yöntemleri denemesine neden olabilir.
 * Herhangi bir sorun olmaması durumunda ise bu sefer
 * el sıkışması (handshake) kontrolü ile sunucu ve istemci
 * sürüm karşılaştırılması yapar. Uyuşum sağlanamadığında bunu bildiren
 * bir mesaj iletir istemciye. Uyumlu olması durumunda ise işlem
 * devam eder. Bu sefer de istemciden sunucu şifre bilgisi ister fakat
 * şöyle bir problem var. Eğer şifre doğru ama anahtar doğru değilse,
 * yine hata döndürecektir ama istemci bunu anlamayacaktır. Bunun asıl nedeni
 * ise, herhangi bir saldırı durumunda istemci tarafının brute-force (kaba kuvvet)
 * ile anahtar bilgisini bulamamasını sağlamak ve akabinde (devamında)
 * şifre bilgisi testlerine geçememesini sağlamak çünkü bu durumda
 * istemci anahtar bilgisinin ya da şifresinin hangisinin yanlış olmadığını
 * bilmemektedir. Çok fazla deneme yapmasını engellemek için ise
 * belirlenen miktarda deneme hakkına sahip olacaktır ama gerçekte sınırsız
 * olmasa da, sınırsızmış gibi hissettirmesi için çok yüksek miktarda
 * deneme imkanını sunucu isterse sunabilir. Örnek olarak, deneme hakkını
 * 3 değilde 1.234.567 yapabilir.
 * 
 * Sunucu tüm bağlantı kontrolleri sonrası istemci ile mesajlaşmaya başlar.
 * İstemci ile isterse belirli bir süre sonra mesaj gelmemesi durumunda
 * bağlantı sonlandırma yapabilir ya da bu özelliği kapatarak istemcinin
 * kendisini kapatana kadar ya da engellenene kadar sunucu da kalması
 * sağlanabilir.
 * 
 * 
 * Güncelleme 2026/03/21
 * 
 * Bir sunucu aynı port üzerinden iki tane sunucu açamaz. Bunun gibi hatayı
 * önlemek için program sunucu port değerini değiştirdikçe bunu bir listeye
 * ekler ver hali hazırda olup olmadığını kontrol eder. Sürekli kontrol
 * yapmaması için ise bayrak değeri ile bunu daha rahat kontrol edebiliriz.
 * Bunun dışında, run fonksiyonu dönüş değeri "void" yerine "Status" olarak
 * ayarlandı. Bu sayede sunucuyu işleten fonksiyon çalıştığında başarı
 * sonucu alabileceğiz ama aksi halde hata sonucu alarak hatamızı da bulabileceğiz.
 * Hata sonuçlarını direk ana sürümde çıktı vermek yerine, sadece hata ayıklama
 * sırasında çıktı verebilmek için de DEBUG_ONLY ön işlemcisini ekledik.
 * Bu sayede ise ana sürümde anlaşılamayan mesajlar görmek ve performans düşüşü
 * yaşamak yerine sorunları bu şekilde çözmüş olacağız.
 */

// Include
#include <functional>
#include <unordered_map>
#include <unordered_set>

#include <kits/corekit.hpp>
#include <kits/hashkit.hpp>

#include <dev/developer.hpp>

#include <pool/cipherpool.hpp>
#include <pool/threadpool.hpp>

#include <socket/socket.hpp>
#include <socket/policy.hpp>
#include <socket/netpacket.hpp>

// Namespace
namespace netsocket::server
{
    // Using Namespace
    using namespace core::platform;
    using namespace core::buildtype;
    using namespace core::version;

    using namespace dev::output::file;
    using namespace dev::output::console;
    using namespace dev::log;
    using namespace dev::trace;

    using namespace tools::charset;
    using namespace tools::hash::vch;
    using namespace tools::time;

    using namespace pool::cipherpool;
    using namespace pool::threadpool;

    // Invalid/Valid
    #if __OS_WINDOWS__
        static constexpr int ss_inv_accept = SOCKET_ERROR;
        static constexpr int ss_inv_bind = -1;
        static constexpr int ss_vld_listen = 0;
    #elif __OS_POSIX__
        static constexpr int ss_inv_accept = -1;
        static constexpr int ss_inv_bind = -1;
        static constexpr int ss_vld_listen = 0;
    #else
        #error "[ERROR] Unsupported Operating System!"
    #endif

    // Enum
    enum class server_code_t : uint16_t
    {
        err = 0,
        has_error,
        critical_error,
        block_error,
        server_not_bind,
        server_not_listen,
        server_not_accept,
        server_not_init,
        accept_socket_not_valid,
        server_has_to_be_init,
        client_could_not_removed_from_list,
        socket_not_found_in_client_list,
        client_not_allowed,
        client_banned_from_server,
        fail_sent_to_client,
        fail_recv_from_client,
        server_banned_client,
        server_banned_potential_attacker,
        client_sent_wrong_password,
        server_recv_err,
        server_could_not_stop,
        server_cannot_run_until_err_fixed,
        server_could_not_init_for_run,
        server_run_thread_fail,
        server_already_init,
        server_not_waited,
        server_port_already_in_use,

        succ = 1000,
        server_bind,
        server_listen,
        server_accept,
        server_init,
        accept_client,
        client_removed_from_list,
        sent_to_client,
        recv_from_client,
        server_recv_without_pwd_require,
        client_recv_and_entered_pwd,
        client_recv_with_auth,
        client_cannot_auth_no_data,
        server_stopped,
        server_started_to_run,
        handler_set,
        server_thread_waited,
        server_port_changed,

        warn = 2000,
        already_bind,
        already_listen,
        already_init,
        client_entered_old_password,

        info = 3000
    };

    // Limit
    static constexpr uint16_t _MAX_RETRY_PWD = 3;

    // Class
    class Server final : public Socket
    {
        public:
            using data_handler = std::function<void(
                Server&,
                const socket_t&,
                const DataPacket&,
                const SocketCtx&
            )>;

        private:
            // Flag
            static constexpr flag::flag_t _FLAG_SERVER_ERR = { 1 << 30 };
            static constexpr flag::flag_t _FLAG_DATA_SET_WARN = { 1 << 29 };
            static constexpr flag::flag_t _FLAG_RUNNING = { 1 << 28 };
            static constexpr flag::flag_t _FLAG_ALREADY_BIND = { 1 << 27 };
            static constexpr flag::flag_t _FLAG_ALREADY_LISTEN = { 1 << 26 };
            static constexpr flag::flag_t _FLAG_ALREADY_INIT = { 1 << 25 };
            static constexpr flag::flag_t _FLAG_ERR_BIND = { 1 << 24 };
            static constexpr flag::flag_t _FLAG_ERR_LISTEN = { 1 << 23 };
            static constexpr flag::flag_t _FLAG_ERR_INIT = { 1 << 22 };
            static constexpr flag::flag_t _FLAG_PORT_ALREADY_IN_USE = { 1 << 21 };

            // Port List
            static inline std::unordered_set<socket_port_t> s_used_ports;

        private:
            ThreadPool m_tpool;

            std::thread m_worker;
            std::atomic<bool> m_running { false };

            flag::Flag m_status { _FLAG_SOCKET_NULL };
            std::unordered_map<socket_t, SocketCtx> m_clients;

            data_handler m_handler;

            mutable std::mutex m_mtx;

        private:
            Status do_bind() noexcept;
            Status do_listen() noexcept;
            Status do_accept(socket_t& ar_cli_sock) noexcept;

            void server_runner() noexcept;

            void client_worker(
                socket_t ar_cli_sock,
                const std::string ar_ipaddr,
                SocketCtx& ar_client
            ) noexcept;

        public:
            explicit Server(
                algorithm::Algorithm& ar_cipher,
                const std::string& ar_filepath,
                const std::string& ar_name,
                const std::string& ar_password,
                const bool ar_password_require,
                const socket_port_t ar_port,
                const ipv_t ar_ipv,
                const policy::max_conn_t ar_max_client = policy::_DEF_CONNECTION,
                const policy::max_conn_t ar_max_same_ip = policy::_DEF_SAME_IP_COUNT,
                const flag::flag_t ar_flag = _FLAG_SOCKET_NULL
            );

            ~Server();

            inline bool is_running() const noexcept;
            inline bool is_full() const noexcept;
            inline bool is_empty() const noexcept;

            inline bool has_error() const noexcept override;

            inline const std::unordered_map<socket_t, SocketCtx>& get_clients() const noexcept;
            inline std::size_t get_client_count() const noexcept;

            Status delete_client(const socket_t ar_sock) noexcept;

            Status set_port(const socket_port_t ar_port) noexcept override;
            Status set_handler(data_handler& ar_handler) noexcept;

            Status send(const socket_t ar_sock, DataPacket& ar_datapack) noexcept override;
            Status recv(const socket_t ar_sock, DataPacket& ar_datapack) noexcept override;

            Status run() noexcept override;
            Status stop() noexcept override;

            void print() noexcept override;

        protected:
            void crashed() noexcept override;
    };

    /**
     * @brief Server
     * 
     * Sunucu da aslında bir soket olduğu için
     * temel soket ihtiyaçlarını aldıktan sonra
     * ek olarak sunucu ihtiyaçlarını alıyoruz.
     * Şuan için toplam istemci sayısı ve aynı
     * ip adresine sahip toplam istemci sayısı
     * sunucu için yeterli. Herhangi bir şeyin
     * istendiği gibi olmaması durumunda
     * kullanabilmek için durum bayrağı da oluşturduk
     * bu sayede hata kontrolünü daha rahat yapabileceğiz
     * 
     * @param Algorithm& Cipher
     * @param string& Filepath
     * @param string& Name
     * @param string& Password
     * @param bool Password Reqiure
     * @param socket_port_t Port
     * @param ipv_t Ip Version
     * @param flag_t Flag
     * @param max_conn_t Max Client
     * @param max_conn_t Max Same Ip
     */
    Server::Server(
        algorithm::Algorithm& ar_cipher,
        const std::string& ar_filepath,
        const std::string& ar_name,
        const std::string& ar_password,
        const bool ar_password_require,
        const socket_port_t ar_port,
        const ipv_t ar_ipv,
        const policy::max_conn_t ar_max_client,
        const policy::max_conn_t ar_max_same_ip,
        const flag::flag_t ar_flag
    )
    : Socket(ar_cipher, ar_filepath, ar_name, ar_password, ar_port, ar_ipv, ar_flag)
    {
        Status tm_status;

        // PASSWORD REQUIRE
        tm_status = this->get_policy().set_require_password(ar_password_require);
        if( !tm_status.is_ok() )
            this->m_status.add(_FLAG_DATA_SET_WARN);

        // MAX CONNECTION
        tm_status = this->get_policy().set_max_connection(ar_max_client);
        if( !tm_status.is_ok() )
            this->m_status.add(_FLAG_DATA_SET_WARN);

        // MAX SAME IP
        tm_status = this->get_policy().set_max_same_ip(ar_max_same_ip);
        if( !tm_status.is_ok() )
            this->m_status.add(_FLAG_DATA_SET_WARN);

        // INITIALIZE
        if( this->m_status.has(_FLAG_ALREADY_INIT) )
            return;

        // CREATE
        tm_status = this->create();
        if( !tm_status.is_ok() ) {
            this->m_status.add(_FLAG_SERVER_ERR);
            return;
        }

        // BIND
        tm_status = this->do_bind();
        if( !tm_status.is_ok() ) {
            this->m_status.add(_FLAG_SERVER_ERR, _FLAG_ERR_BIND);
            return;
        }

        // LISTEN
        tm_status = this->do_listen();
        if( !tm_status.is_ok() ) {
            this->m_status.add(_FLAG_SERVER_ERR, _FLAG_ERR_LISTEN);
            return;
        }

        // TIMEOUT
        #if __OS_POSIX__
            timeval tm_val {};
            tm_val.tv_sec = static_cast<decltype(tm_val.tv_sec)>(this->get_timeout());
            tm_val.tv_usec = 0;

            setsockopt(this->get_socket(), SOL_SOCKET, SO_RCVTIMEO, &tm_val, sizeof(tm_val));
            setsockopt(this->get_socket(), SOL_SOCKET, SO_SNDTIMEO, &tm_val, sizeof(tm_val));
        #elif __OS_WINDOWS__
            DWORD tm_val = static_cast<DWORD>(this->get_timeout());

            setsockopt(this->get_socket(), SOL_SOCKET, SO_RCVTIMEO, (const char*)&tm_val, sizeof(tm_val));
            setsockopt(this->get_socket(), SOL_SOCKET, SO_SNDTIMEO, (const char*)&tm_val, sizeof(tm_val));
        #endif

        this->m_status.add(_FLAG_ALREADY_INIT);
        this->m_status.unset(_FLAG_SERVER_ERR, _FLAG_ERR_BIND, _FLAG_ERR_LISTEN, _FLAG_ERR_INIT);
    }

    /**
     * @brief ~Server
     * 
     * Sınıf sonlandırılmadan önce yapılması
     * gereken son temizlik işlemlerini yapar
     */
    Server::~Server()
    {
        this->stop();
    }

    /**
     * @brief Do Bind
     * 
     * Sunucu da Ipv4/v6 bağlantısını yaparak
     * istemcilerin bağlanabilmesi için ön hazırlık yapar.
     * Zaten yapılmışsa ya da hata varsa bunu bildirir
     * 
     * @return Status
     */
    Status Server::do_bind() noexcept
    {
        // ERROR
        if( this->has_error() )
            return Status::err(domain_t::server, status::to_underlying(server_code_t::has_error));
        else if( this->m_status.has(_FLAG_ALREADY_BIND) )
            return Status::warn(domain_t::server, status::to_underlying(server_code_t::already_bind));

        // VARIABLE
        sockaddr* tm_addr = nullptr;
        socklen_t tm_len_addr = 0;

        // IPV4/V6
        switch( this->get_ipv() )
        {
            // IPV6
            case ipv_t::ipv6:
            {
                sockaddr_in6 tm_addrv6 {};
                tm_addrv6.sin6_family = tcp::ipv6::domain;
                tm_addrv6.sin6_addr = in6addr_any;
                tm_addrv6.sin6_port = htons(this->get_port());

                tm_addr = reinterpret_cast<sockaddr*>(&tm_addrv6);
                tm_len_addr = sizeof(tm_addrv6);

                this->get_flag().add(_FLAG_SOCKET_IPV6);
            }
            break;

            // IPV4
            case ipv_t::ipv4:
            default:
            {
                sockaddr_in tm_addrv4 {};
                tm_addrv4.sin_family = tcp::ipv4::domain;
                tm_addrv4.sin_addr.s_addr = INADDR_ANY;
                tm_addrv4.sin_port = htons(this->get_port());

                tm_addr = reinterpret_cast<sockaddr*>(&tm_addrv4);
                tm_len_addr = sizeof(tm_addrv4);

                this->get_flag().unset(_FLAG_SOCKET_IPV6);
            }
            break;
        }

        // BIND
        int tm_res = ::bind(this->get_socket(), tm_addr, tm_len_addr);
        if( tm_res == ss_inv_bind )
        {
            this->m_status.unset(_FLAG_ALREADY_BIND);

            // BIND ERROR POSIX
            #if __OS_POSIX__
                if(errno == EADDRINUSE ) {
                    this->m_status.add(_FLAG_PORT_ALREADY_IN_USE);
                    return Status::err(domain_t::server, status::to_underlying(server_code_t::server_port_already_in_use));
                }
            // BIND ERROR WINDOWS
            #elif __OS_WINDOWS__
                if( WSAGetLastError() == WSAEADDRINUSE ) {
                    this->m_status.add(_FLAG_PORT_ALREADY_IN_USE);
                    return Status::err(domain_t::server, status::to_underlying(server_code_t::server_port_already_in_use));
                }
            #endif

            return Status::err(domain_t::server, status::to_underlying(server_code_t::server_not_bind));
        }

        this->m_status.add(_FLAG_ALREADY_BIND);
        this->m_status.unset(_FLAG_SERVER_ERR);

        return Status::ok(domain_t::server, status::to_underlying(server_code_t::server_bind));
    }

    /**
     * @brief Do Listen
     * 
     * Sunucu belirli bir miktar soket dinleyebilir.
     * Bu miktarı ayarlayarak gelecekte ne kadar
     * soket dinleyebileceğini belirtiyoruz.
     * Zaten yapılmışsa ya da hata varsa bunu bildirir
     * 
     * @return Status
     */
    Status Server::do_listen() noexcept
    {
        // ERROR
        if( this->has_error() )
            return Status::err(domain_t::server, status::to_underlying(server_code_t::has_error));
        else if( !this->m_status.has(_FLAG_ALREADY_BIND) )
            return Status::err(domain_t::server, status::to_underlying(server_code_t::server_not_bind));
        else if( this->m_status.has(_FLAG_ALREADY_LISTEN) )
            return Status::warn(domain_t::server, status::to_underlying(server_code_t::already_listen));

        // LISTEN
        int tm_backlog = static_cast<int>(this->get_policy().get_max_connection());
        int tm_res = ::listen(this->get_socket(), tm_backlog);
        if( tm_res != ss_vld_listen )
        {
            this->m_status.unset(_FLAG_ALREADY_LISTEN);
            return Status::err(domain_t::server, status::to_underlying(server_code_t::server_not_listen));
        }

        this->m_status.add(_FLAG_ALREADY_LISTEN);
        this->m_status.unset(_FLAG_SERVER_ERR);

        return Status::ok(domain_t::server, status::to_underlying(server_code_t::server_listen));
    }

    /**
     * @brief Do Accept
     * 
     * Sunucuya bir soket bağlanmaya çalıştıktan sonra
     * buna sunucunun izin verip vermeyeceğini sağlarız
     * Hata varsa bunu bildirir
     * 
     * @param socket_t& Client Socket
     * 
     * @return Status
     */
    Status Server::do_accept(socket_t& ar_cli_sock) noexcept
    {
        // ERROR
        if( this->has_error() )
            return Status::err(domain_t::server, status::to_underlying(server_code_t::has_error));
        else if( !Socket::is_valid_socket(this->get_socket()) )
            return Status::err(domain_t::server, status::to_underlying(socket_code_t::socket_not_valid));

        // VARIABLE
        sockaddr_storage tm_sockstore {};
        socklen_t tm_sock_len = sizeof(tm_sockstore);
        socket_t tm_server_sock = this->get_socket();

        // ACCEPT
        ar_cli_sock = ::accept(tm_server_sock, reinterpret_cast<sockaddr*>(&tm_sockstore), &tm_sock_len);
        if( !Socket::is_valid_socket(ar_cli_sock) )
            return Status::err(domain_t::server, status::to_underlying(server_code_t::accept_socket_not_valid));

        return Status::ok(domain_t::server, status::to_underlying(server_code_t::accept_client));
    }

    /**
     * @brief Server Runner
     * 
     * Sunucuyu döngü halinde sürekli çalışır şekilde
     * tutup işlemlerini yürütecek olan fonksiyondur.
     * İstemcilerin kabulünü vs. yaparak istemcileri
     * işleme alır
     */
    void Server::server_runner() noexcept
    {
        // IP
        std::string tm_ip;
        tm_ip.reserve(128);

        // RUN
        while( this->is_running() )
        {
            // CLIENT SOCKET
            socket_t tm_cli_accpt = ss_inv_socket;

            // ACCEPT
            if( !this->do_accept(tm_cli_accpt).is_ok() )
                continue;

            // CLIENT IP
            tm_ip.clear();
            tm_ip = Socket::get_ip(tm_cli_accpt);

            // SOCKET VERSION HASH VERIFY
            Status tm_handshake = Socket::handshake_recv_verify(tm_cli_accpt, false);
            if( !tm_handshake.is_ok() ) {
                DEBUG_ONLY(this->get_logger().write(level_t::Warn, tm_ip + " version hash didn't match | code: " + std::to_string(tm_handshake.get_code()), GET_SOURCE));

                tm_handshake = Socket::handshake_send_verify(tm_cli_accpt);
                DEBUG_ONLY(
                    if( !tm_handshake.is_ok() )
                        this->get_logger().write(level_t::Warn, tm_ip + " recv version hash sent for warning | code: " + std::to_string(tm_handshake.get_code()), GET_SOURCE)
                );

                continue;
            }

            // SOCKET VERSION HASH SEND VERIFY
            tm_handshake = Socket::handshake_send_verify(tm_cli_accpt);
            if( !tm_handshake.is_ok() ) {
                DEBUG_ONLY(this->get_logger().write(level_t::Warn, tm_ip + " version hash sent for warning | code: " + std::to_string(tm_handshake.get_code()), GET_SOURCE));
                continue;
            }

            DEBUG_ONLY(this->get_logger().write(level_t::Succ, tm_ip + " Handshake Verified", GET_SOURCE));

            // BANNED OR ALLOWED ?
            Status tm_allow = this->get_policy().can_allow(tm_ip);
            if( !tm_allow.is_ok() )
            {
                switch(tm_allow.get_code())
                {
                    case status::to_underlying(policy::policy_e::not_allow_ipaddr_already_banned):
                        DEBUG_ONLY(this->get_logger().write(level_t::Succ, tm_ip + " ip banned", GET_SOURCE));
                    break;

                    case status::to_underlying(policy::policy_e::not_allow_ipaddr_not_in_list):
                        DEBUG_ONLY(this->get_logger().write(level_t::Succ, tm_ip + " ip not in the allowed ips", GET_SOURCE));
                    break;
                }

                Socket::close_socket(tm_cli_accpt);
                continue;
            }

            // SAME IP
            size_t tm_count_same_ip = 0;
            for(const auto& [sock, ctx] : this->m_clients)
            {
                if(ctx.m_ip == tm_ip)
                    ++tm_count_same_ip;
            }

            if( this->m_clients.size() >= this->get_policy().get_max_connection() )
            {
                DEBUG_ONLY(this->get_logger().write(level_t::Warn, "Over total connection limit", GET_SOURCE));

                Socket::close_socket(tm_cli_accpt);
                continue;
            }
            else if( tm_count_same_ip >= this->get_policy().get_max_same_ip() )
            {
                DEBUG_ONLY(this->get_logger().write(level_t::Warn, "Over total same ip limit", GET_SOURCE));

                Socket::close_socket(tm_cli_accpt);
                continue;
            }

            this->m_clients.emplace(tm_cli_accpt, SocketCtx{ UserPacket{}, tm_ip });
            DEBUG_ONLY(this->get_logger().write(level_t::Info, tm_ip + " added client list", GET_SOURCE));

            this->m_tpool.enqueue([&, tm_cli_accpt, tm_ip]{
                auto tm_it = this->m_clients.find(tm_cli_accpt);
                if( tm_it == this->m_clients.end() )
                    return;

                SocketCtx& tm_client = tm_it->second;
                tm_client.m_user.m_same_user_count = tm_count_same_ip;

                this->client_worker(tm_cli_accpt, tm_ip, tm_client);
                Socket::close_socket(tm_cli_accpt);

                DEBUG_ONLY(this->get_logger().write(level_t::Warn, "Socket closed | " + tm_client.m_ip + '_' + std::to_string(tm_client.m_user.m_same_user_count), GET_SOURCE));

                this->m_clients.erase(tm_cli_accpt);
            });
        }
    }

    /**
     * @brief Client Worker
     * 
     * Sunucu da her bağlantı için bir işçi çalışacak.
     * Bu işçi, istemci ile iletişimi sağlayacak
     * 
     * @param socket_t Client Socket
     * @param string Client Ip Address
     * @param SocketCtx& Client Data
     */
    void Server::client_worker(
        socket_t ar_cli_sock,
        const std::string ar_ipaddr,
        SocketCtx& ar_client
    ) noexcept
    {
        (void)ar_client;

        // ERROR
        if( this->has_error() || !Socket::is_valid_socket(ar_cli_sock))
            return;

        // TIMEOUT
        const auto tm_timeout = std::chrono::seconds(this->get_timeout());

        // RUNNING
        while( this->is_running() )
        {
            // SET FD
            fd_set tm_readfs;
            FD_ZERO(&tm_readfs);
            FD_SET(ar_cli_sock, &tm_readfs);

            // SET TIMEOUT
            timeval tm_tv {};
            tm_tv.tv_sec = tm_timeout.count();
            tm_tv.tv_usec = 0;

            // SELECT
            int tm_ready = ::select(ar_cli_sock + 1, &tm_readfs, nullptr, nullptr, &tm_tv);
            if( tm_ready <= 0 )
                return;

            // BAN CHECK
            if( this->get_policy().is_connection_banned(ar_ipaddr) )
            {
                DEBUG_ONLY(this->get_logger().write(level_t::Warn, ar_ipaddr + " banned ip tried to connect"));
                break;
            }
            // ALLOW CHECK
            else if( !this->get_policy().is_connection_allowed(ar_ipaddr) )
            {
                DEBUG_ONLY(this->get_logger().write(level_t::Warn, ar_ipaddr + " not allowed"));
                break;
            }

            // DATA PACKET & RECV
            DataPacket tm_datapack;
            Status tm_status = this->recv(ar_cli_sock, tm_datapack);

            // RECV STATUS
            switch( tm_status.get_code() ) {
                case status::to_underlying(socket_code_t::socket_data_recv):       
                    DEBUG_ONLY(this->get_logger().write(level_t::Warn, "received from " + ar_ipaddr));
                break;

                case status::to_underlying(socket_code_t::packet_no_data):
                    DEBUG_ONLY(this->get_logger().write(level_t::Warn, "no data from " + ar_ipaddr));
                break;

                default:
                    DEBUG_ONLY(this->get_logger().write(level_t::Warn, "data couldn't receive from " + ar_ipaddr + " | code: " + std::to_string(tm_status.get_code())));
                return;
            }

            // LOG ALL
            DEBUG_ONLY(this->get_logger().write("Server|Recv", "Id: " + std::to_string(ar_cli_sock)));
            DEBUG_ONLY(this->get_logger().write("Server|Recv", "Password: " + tm_datapack.m_pwd));
            DEBUG_ONLY(this->get_logger().write("Server|Recv", "Username: " + tm_datapack.m_name));
            DEBUG_ONLY(this->get_logger().write("Server|Recv", "Message: " + tm_datapack.m_msg));

            // SEND
            tm_status = this->send(ar_cli_sock, tm_datapack);

            // SEND STATUS
            switch( tm_status.get_code() )
            {
                case status::to_underlying(socket_code_t::socket_data_sent):
                    DEBUG_ONLY(this->get_logger().write(level_t::Warn, "sent to " + ar_ipaddr, GET_SOURCE));
                break;

                default:
                    DEBUG_ONLY(this->get_logger().write(level_t::Warn, "data couldn't send to " + ar_ipaddr + " | code: " + std::to_string(tm_status.get_code())));
                return;
            }
        }
    }

    /**
     * @brief Is Running
     * 
     * Sunucunun hala çalışıp çalışmadığı bilgisini döndürür.
     * Sunucu da hata kontrolcüsü hata vermediği sürece ya da
     * çalışma durumu devam ettikçe çalışıyor demektir
     * 
     * @return bool
     */
    bool Server::is_running() const noexcept
    {
        return this->m_running.load(std::memory_order_acquire) && !CrashHandler::is_signal();
    }

    /**
     * @brief Is Full
     * 
     * Sunucunun tam anlamıyla dolup dolmadığını
     * çoklu işlemci kontrolü sağlayarak kontrol eder
     * 
     * @return bool
     */
    bool Server::is_full() const noexcept
    {
        std::scoped_lock tm_lock(this->m_mtx);
        return static_cast<std::size_t>(Socket::get_policy().get_max_connection()) <= this->m_clients.size();
    }

    /**
     * @brief Is Empty
     * 
     * Sunucuya bağlı herhangi bir istemcinin
     * olup olmadığını bildirir
     * 
     * @return bool
     */
    bool Server::is_empty() const noexcept
    {
        return this->m_clients.empty();
    }

    /**
     * @brief Has Error
     * 
     * Sunucunun çalışmasına engel olan ya da
     * olabilecek olası hataların varlığından
     * haberdar olabilmemizi sağlar
     * 
     * @return bool
     */
    bool Server::has_error() const noexcept
    {
        return Socket::has_error();
    }

    /**
     * @brief Get Clients
     * 
     * Sunucudaki bulunan istemcilerin listesini korumalı
     * referans olarak döndürür. Bu sayede verileri gözükse
     * bile erişim imkanı yoktur. Asıl amacı GUI içindir
     * 
     * @return const unordered_map<socket_t, SocketCtx>&
     */
    const std::unordered_map<socket_t, SocketCtx>& Server::get_clients() const noexcept
    {
        return this->m_clients;
    }

    /**
     * @brief Get Client Count
     * 
     * Sunucuda bulunan istemci sayısını döndürür.
     * İstemcilerin olduğu listenin boyutunu bilmek yeterli olur
     * 
     * @return size_t
     */
    std::size_t Server::get_client_count() const noexcept
    {
        std::scoped_lock tm_lock(this->m_mtx);
        return this->m_clients.size();
    }

    /**
     * @brief Delete Client
     * 
     * Verilen soket değerini kontrol ettikten sonra
     * istemci listesinde arama yaparak bulmaya çalışır
     * ve bulduğunda siler
     * 
     * @param socket_t Socket
     * @return Status
     */
    Status Server::delete_client(const socket_t ar_sock) noexcept
    {
        if( !Socket::is_valid_socket(ar_sock) )
            return Status::warn(domain_t::server, status::to_underlying(socket_code_t::socket_not_valid));

        std::scoped_lock tm_lock(this->m_mtx);
        return this->m_clients.erase(ar_sock) ?
            Status::ok(domain_t::server, status::to_underlying(server_code_t::client_removed_from_list)) :
            Status::err(domain_t::server, status::to_underlying(server_code_t::client_could_not_removed_from_list));
    }

    /**
     * @brief Set Port
     * 
     * Ana socket sınıfında bulunmasına rağmen sunucu sınıfında
     * yeniden tasarlanmıştır çünkü birden çok sunucu aynı port
     * adresini kullanmayı deneyebilir. Bu hatayı önlemek için ise
     * en başta port ayarlama fonksiyonu ile de ek kontrol sağlarız
     * 
     * @param socket_port_t Port
     * @return Status
     */
    Status Server::set_port(const socket_port_t ar_port) noexcept
    {
        // IS VALID PORT ?
        if( !is_valid_port(ar_port) )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::port_not_valid));

        // PORT ALREADY IN USE ?
        {
            std::scoped_lock tm_lock(this->m_mtx);
            
            if( s_used_ports.count(ar_port) )
            {
                this->m_status.add(_FLAG_PORT_ALREADY_IN_USE);
                return Status::err(domain_t::server, status::to_underlying(server_code_t::server_port_already_in_use));
            }
        }

        // SET THE PORT
        Status tm_status = Socket::set_port(ar_port);
        if( !tm_status.is_ok() )
            return tm_status;

        // ADD PORT TO LIST
        {
            std::scoped_lock tm_lock(this->m_mtx);
            s_used_ports.emplace(this->get_port());
            this->m_status.unset(_FLAG_PORT_ALREADY_IN_USE);
        }

        return Status::ok(domain_t::server, status::to_underlying(server_code_t::server_port_changed));
    }

    /**
     * @brief Set Handler
     * 
     * Sunucu istemciye veri gönderimi yaparken
     * bunların GUI da görünebilmesini sağlamak ya da
     * daha farklı işlemler yapabilmek için bu veriyi
     * işleyebilecek bir fonksiyon ayarlanmasını sağlar
     * 
     * @param data_handler& Handler
     * @return Status
     */
    Status Server::set_handler(data_handler& ar_handler) noexcept
    {
        this->m_handler = std::move(ar_handler);
        return Status::ok(domain_t::server, status::to_underlying(server_code_t::handler_set));
    }

    /**
     * @brief Send
     * 
     * Sunucu da yapısı gereği bir sokettir ama soketten
     * farklı olarak fazladan kontroller ve farklılıklar ile
     * veri gönderimi yapar. Gönderim için ana soket fonksiyonunu kullanır
     * 
     * @param socket_t Socket
     * @param DataPacket& Data Packet
     * 
     * @return Status
     */
    Status Server::send(
        const socket_t ar_sock,
        DataPacket& ar_datapack
    ) noexcept
    {
        // FIND CLIENT
        if( this->m_clients.find(ar_sock) == this->m_clients.end() )
            return Status::err(domain_t::server, status::to_underlying(server_code_t::socket_not_found_in_client_list));

        // GET IP
        const std::string tm_ip = Socket::get_ip(ar_sock);

        // IS ALLOWED AND NOT BANNED
        if( !this->get_policy().is_connection_allowed(tm_ip) )
            return Status::err(domain_t::server, status::to_underlying(server_code_t::client_not_allowed));
        else if( this->get_policy().is_connection_banned(tm_ip) )
            return Status::err(domain_t::server, status::to_underlying(server_code_t::client_banned_from_server));

        // SEND
        return Socket::send(ar_sock, ar_datapack);
    }

    /**
     * @brief Recv
     * 
     * Sunucu da yapısı gereği bir sokettir ama soketten
     * farklı olarak fazladan kontroller ve farklılıklar ile
     * veri alımı yapar. Alım için ana soket fonksiyonunu kullanır
     * 
     * @param socket_t Socket
     * @param DataPacket& Data Packet
     * 
     * @return Status
     */
    Status Server::recv(
        const socket_t ar_sock,
        DataPacket& ar_datapack
    ) noexcept
    {
        // FIND CLIENT
        auto tm_client_it = this->m_clients.find(ar_sock);
        if( tm_client_it == this->m_clients.end() )
            return Status::err(domain_t::server, status::to_underlying(server_code_t::socket_not_found_in_client_list));

        // GET IP
        const std::string tm_ip = Socket::get_ip(ar_sock);

        // IS ALLOWED AND NOT BANNED
        if( !this->get_policy().is_connection_allowed(tm_ip) )
            return Status::err(domain_t::server, status::to_underlying(server_code_t::client_not_allowed));
        else if( this->get_policy().is_connection_banned(tm_ip) )
            return Status::err(domain_t::server, status::to_underlying(server_code_t::client_banned_from_server));

        // RECEIVE
        if( !Socket::recv(ar_sock, ar_datapack).is_ok() )
            return Status::err(domain_t::server, status::to_underlying(server_code_t::fail_recv_from_client));

        // CAN AUTH ?
        Status tm_status = this->get_policy().can_auth(tm_ip, ar_datapack.m_pwd);
        if( tm_status.is_ok() )
            return Status::ok(domain_t::server, status::to_underlying(server_code_t::client_recv_with_auth));

        // WHY AUTH FAILED
        // IS DATA EMPTY ?
        if( ar_datapack.m_pwd.empty() && ar_datapack.m_msg.empty() && ar_datapack.m_name.empty() )
            return Status::err(domain_t::server, status::to_underlying(server_code_t::client_cannot_auth_no_data));

        // ENTERED WRONG PASSWORD
        ++(tm_client_it->second.m_user.m_try_passwd);

        // LOG THE WRONG PASSWORD ATTEMPT
        DEBUG_ONLY(this->get_logger().write(level_t::Warn, tm_ip + " sent wrong password attempt #" + std::to_string(tm_client_it->second.m_user.m_try_passwd)));

        // PASSWORD VARIABLE TYPE
        using pwd_t = decltype(tm_client_it->second.m_user.m_try_passwd);
            
        // EQUAL MAX RETRY PASSWORD
        if( tm_client_it->second.m_user.m_try_passwd == static_cast<pwd_t>(_MAX_RETRY_PWD) )
        {
            this->get_policy().set_ban(tm_ip, true);

            Socket::close_socket(ar_sock);
            Socket::shutdown_socket(ar_sock);

            this->delete_client(ar_sock);

            return Status::err(domain_t::server, status::to_underlying(server_code_t::server_banned_client));
        }
        // OVER MAX RETRY PASSWORD (POTENTIAL ATTACKER)
        else if( tm_client_it->second.m_user.m_try_passwd > static_cast<pwd_t>(_MAX_RETRY_PWD) )
        {
            this->get_policy().set_ban(tm_ip, true);

            Socket::close_socket(ar_sock);
            Socket::shutdown_socket(ar_sock);

            this->delete_client(ar_sock);

            return Status::err(domain_t::server, status::to_underlying(server_code_t::server_banned_potential_attacker));
        }

        // CLIENT ENTERING OLD PASSWORD
        if( tm_status.get_code() == status::to_underlying(policy::policy_e::cannot_auth_with_old_password))
            return Status::warn(domain_t::server, status::to_underlying(server_code_t::client_entered_old_password));
        // WRONG PASSWORD
        else if( tm_status.get_code() == status::to_underlying(policy::policy_e::cannot_auth_password_not_matched) )
            return Status::err(domain_t::server, status::to_underlying(server_code_t::client_sent_wrong_password));

        // RECEIVE ERROR
        return Status::err(domain_t::server, status::to_underlying(server_code_t::server_recv_err));
    }

    /**
     * @brief Run
     * 
     * Sunucuyu çalıştırmak için gereklidir. Sunucu
     * durdurulana dek çalışmaya devam eder ve istemciler ile
     * bağlantıyı sağlar. Hata oluşması ya da durdurma emri
     * sonrası çalışmayı sonlandırır
     * 
     * @return Status
     */
    Status Server::run() noexcept
    {
        // HAS ERROR
        if( this->m_status.has(_FLAG_SERVER_ERR) ) {
            this->get_logger().write(level_t::Err, "Server Cannot Run Until Error Fixed");
            return Status::err(domain_t::server, status::to_underlying(server_code_t::has_error));
        }

        // PORT ALREADY IN USE
        if( this->m_status.has(_FLAG_PORT_ALREADY_IN_USE) )
        {
            this->get_logger().write(level_t::Err, "Server Port (" + std::to_string(this->get_port()) + ") Already In Use");
            return Status::err(domain_t::server, status::to_underlying(server_code_t::server_port_already_in_use));
        }

        // SERVER SOCKET
        const socket_t tm_server = this->get_socket();

        // NOT VALID SOCKET
        if( !Socket::is_valid_socket(tm_server) ) { 
            this->get_logger().write(level_t::Err, "Server Socket Is Not Valid");
            return Status::err(domain_t::server, status::to_underlying(socket_code_t::socket_not_valid));
        }

        // SET RUNNING
        this->m_running.store(true, std::memory_order_relaxed);

        // SERVER RUNNER
        try {
            this->m_worker = std::thread([this]{
                this->server_runner();
            });
        } catch (...) {
            this->m_running.store(false, std::memory_order_relaxed);
            return Status::err(domain_t::server, status::to_underlying(server_code_t::server_run_thread_fail));
        }

        // RUNNED
        return Status::ok(domain_t::server, status::to_underlying(server_code_t::server_started_to_run));
    }

    /**
     * @brief Stop
     * 
     * Sunucuyu durdurmaya yarar. Eğer zaten durmuş ise
     * ya da durdurma başarılı ise bunları durum nesnesi
     * ile döndürerek bildirir
     * 
     * @return Status
     */
    Status Server::stop() noexcept
    {
        // TRY THE STOP
        if( !this->m_running.exchange(false) )
            return Status::err(domain_t::server, status::to_underlying(server_code_t::server_could_not_stop));

        // CLOSE SERVER SOCKET
        this->close();

        // WAIT FOR THREAD
        if( this->m_worker.joinable() )
            this->m_worker.join();

        // STOPPED
        return Status::ok(domain_t::server, status::to_underlying(server_code_t::server_stopped));
    }

    /**
     * @brief Print
     * 
     * Sunucu hakkında bilgileri çıktı verir.
     * Kaç adet istemci bağlı, en fazla sınırları nedir.
     * Port numarası, ip sürümü ve dahası hakkında bilgi verir
     */
    void Server::print() noexcept
    {
        this->get_logger().write("========== SERVER ==========");
        this->get_logger().write(std::string("Build: ").append(Build::c_str()));
        this->get_logger().write(std::string("Socket: ").append(this->is_running() ? "Running" : "Stopped"));
        this->get_logger().write(std::string("Port: ").append(std::to_string(this->get_port())));
        this->get_logger().write(std::string("Ip Version: ").append(this->get_ipv() == ipv_t::ipv6 ? "v6" : "v4"));
        this->get_logger().write(std::string("Total Client: ").append(std::to_string(this->get_client_count())));
        this->get_logger().write(std::string("Max Connection Limit: ").append(std::to_string(this->get_policy().get_max_connection())));
        this->get_logger().write(std::string("Max Same Ip Limit: ").append(std::to_string(this->get_policy().get_max_same_ip())));
        this->get_logger().write(std::string("Require Password: ").append(this->get_policy().is_require_password() ? "Yes" : "No"));
        this->get_logger().write("============================");
    }

    /**
     * @brief Crashed
     * 
     * Sunucunun bir anda çökmesi durumunda yapılacakları
     * işleme alır ve olası donanım israfını önlemeyi amaçlar
     */
    void Server::crashed() noexcept
    {
        this->stop();
        Socket::crashed();
    }
}
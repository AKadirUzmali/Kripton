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
 * 
 * Güncelleme: 2026/03/29
 * 
 * client_worker yerine sınıf için oluşturduğumuz m_handler fonksiyon
 * tutucu nesne kullanıldı. Bazı soket kapatmama mantık hataları ve
 * thread-safe olmayan statik nesneler için mutex eklenilip kullanıldı.
 * Hata giderme, güvenlik ve performans iyileştirmeleri yapıldı.
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
        err = 10000,
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
        server_already_running,
        server_init_create_err,
        server_init_bind_err,
        server_init_listen_err,
        this_port_binded_try_another,
        server_has_no_ip_type,
        server_not_create,
        server_reinit_err,

        succ = 11000,
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
        server_clear,

        warn = 12000,
        already_bind,
        already_listen,
        already_init,
        client_entered_old_password,

        info = 13000
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
                const SocketCtx&
            )>;

        private:
            // Flag
            static constexpr flag::flag_t _FLAG_ALREADY_CREATE = { 1 << 28 };
            static constexpr flag::flag_t _FLAG_ALREADY_BIND = { 1 << 27 };
            static constexpr flag::flag_t _FLAG_ALREADY_LISTEN = { 1 << 26 };
            static constexpr flag::flag_t _FLAG_ALREADY_INIT = { 1 << 25 };
            static constexpr flag::flag_t _FLAG_ERR_CREATE = { 1 << 24 };
            static constexpr flag::flag_t _FLAG_ERR_BIND = { 1 << 23 };
            static constexpr flag::flag_t _FLAG_ERR_LISTEN = { 1 << 22 };
            static constexpr flag::flag_t _FLAG_ERR_INIT = { 1 << 21 };
            static constexpr flag::flag_t _FLAG_ERR_HANDLER = { 1 << 20 };
            static constexpr flag::flag_t _FLAG_ERR_PORT_ALREADY_IN_USE = { 1 << 19 };

            // Port Mutex & List
            static inline std::mutex s_port_mtx;
            static inline std::uint16_t s_total_server;

        private:
            std::thread m_worker;
            ThreadPool m_tpool;

            std::atomic<bool> m_running { false };

            flag::Flag m_status { _FLAG_SOCKET_NULL };

            std::unordered_map<socket_t, SocketCtx> m_clients;
            std::unordered_map<std::string, size_t> m_ip_count;

            data_handler m_handler;

            mutable std::mutex m_mtx;

        private:
            Status do_bind() noexcept;
            Status do_listen() noexcept;
            Status do_accept(socket_t& ar_cli_sock) noexcept;

            void server_runner() noexcept;

        public:
            explicit Server(
                algorithm::Algorithm& ar_cipher,
                const std::string& ar_filepath,
                const std::string& ar_name,
                const std::string& ar_password,
                const bool ar_password_require,
                const socket_port_t ar_port,
                const ipv_t ar_ipv,
                data_handler ar_handler,
                const policy::max_conn_t ar_max_client = policy::_DEF_CONNECTION,
                const policy::max_conn_t ar_max_same_ip = policy::_DEF_SAME_IP_COUNT,
                const flag::flag_t ar_flag = _FLAG_SOCKET_NULL
            );

            ~Server();

            inline bool is_full() const noexcept;
            inline bool is_empty() const noexcept;

            inline bool has_error() const noexcept override;

            static inline size_t get_total_server() noexcept;

            inline const std::unordered_map<socket_t, SocketCtx>& get_clients() const noexcept;
            inline std::size_t get_client_count() const noexcept;
            inline const flag::Flag& get_status_flag() const noexcept;

            Status delete_client(const socket_t ar_sock) noexcept;

            Status set_port(const socket_port_t ar_port) noexcept override;
            Status set_handler(data_handler& ar_handler) noexcept;

            Status create() noexcept override;
            Status close() noexcept override;
            Status clear() noexcept override;

            Status send(const socket_t ar_sock, const DataPacket& ar_datapack) noexcept override;
            Status recv(const socket_t ar_sock, DataPacket& ar_datapack) noexcept override;

            Status run() noexcept override;
            Status stop() noexcept override;

            void print() noexcept override;

        protected:
            Status init() noexcept override;

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
     * @param data_handler Handler
     * @param max_conn_t Max Client
     * @param max_conn_t Max Same Ip
     * @param flag_t Flag
     */
    Server::Server(
        algorithm::Algorithm& ar_cipher,
        const std::string& ar_filepath,
        const std::string& ar_name,
        const std::string& ar_password,
        const bool ar_password_require,
        const socket_port_t ar_port,
        const ipv_t ar_ipv,
        data_handler ar_handler,
        const policy::max_conn_t ar_max_client,
        const policy::max_conn_t ar_max_same_ip,
        const flag::flag_t ar_flag
    )
    : Socket(ar_cipher, ar_filepath, ar_name, ar_password, ar_port, ar_ipv, ar_flag)
    {
        // INCREASE TOTAL SERVER
        {
            std::scoped_lock tm_lock(s_port_mtx);
            ++s_total_server;
        }

        // IS RUNNING
        if( this->is_running() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Server Constructor Running Already Error, Flag: " + std::to_string(this->m_status.get()), GET_SOURCE));

            // LOGGER
            if( this->get_flag().has(_FLAG_SOCKET_LOGGER) )
                this->get_logger().write(level_t::Err, this->get_policy().get_username(), "Server Cannot Create Error, Already Running");

            return;
        }

        // STATUS VARIABLE
        Status tm_status;

        // PASSWORD REQUIRE
        tm_status = this->get_policy().set_require_password(ar_password_require);
        if( !tm_status.is_ok() && !tm_status.is_warn() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Server Constructor Set Require Password Error, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
        
            // LOGGER
            if( this->get_flag().has(_FLAG_SOCKET_LOGGER) )
                this->get_logger().write(level_t::Err, this->get_policy().get_username(), "Password Require Error, Code: " + std::to_string(tm_status.get_code()));
        }

        // MAX CONNECTION
        tm_status = this->get_policy().set_max_connection(ar_max_client);
        if( !tm_status.is_ok() && !tm_status.is_warn() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Server Constructor Set Max Connection Error, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));

            // LOGGER
            if( this->get_flag().has(_FLAG_SOCKET_LOGGER) )
                this->get_logger().write(level_t::Err, this->get_policy().get_username(), "Max Connection Error, Code: " + std::to_string(tm_status.get_code()));
        }

        // MAX SAME IP
        tm_status = this->get_policy().set_max_same_ip(ar_max_same_ip);
        if( !tm_status.is_ok() && !tm_status.is_warn() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Server Constructor Set Max Same Ip Error, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));

            // LOGGER
            if( this->get_flag().has(_FLAG_SOCKET_LOGGER) )
                this->get_logger().write(level_t::Err, this->get_policy().get_username(), "Max Same Ip Error, Code: " + std::to_string(tm_status.get_code()));
        }

        // HANDLER
        tm_status = this->set_handler(ar_handler);
        if( !tm_status.is_ok() && !tm_status.is_warn() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Server Constructor Set Handler Error, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));

            // LOGGER
            if( this->get_flag().has(_FLAG_SOCKET_LOGGER) )
                this->get_logger().write(level_t::Err, this->get_policy().get_username(), "Handler Error, Code: " + std::to_string(tm_status.get_code()));
        }

        // SET PORT
        tm_status = this->set_port(ar_port);
        if( !tm_status.is_ok() && !tm_status.is_warn() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Server Constructor Set Port Error, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));

            // LOGGER
            if( this->get_flag().has(_FLAG_SOCKET_LOGGER) )
                this->get_logger().write(level_t::Err, this->get_policy().get_username(), "Port Error, Code: " + std::to_string(tm_status.get_code()));
        }

        // INITIALIZE
        tm_status = this->init();
        if( !tm_status.is_ok() && !tm_status.is_warn() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Server Constructor Initialize Error, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
            return;
        }
    }

    /**
     * @brief ~Server
     * 
     * Sınıf sonlandırılmadan önce yapılması
     * gereken son temizlik işlemlerini yapar
     */
    Server::~Server()
    {
        // DECREASE TOTAL SERVER
        {
            std::scoped_lock tm_lock(this->s_port_mtx);

            if( s_total_server )
                --s_total_server;
        }

        // STOP
        this->stop();

        // CLEAR
        this->clear();
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
        // HAS ERROR
        if( Socket::has_error() )
            return Status::err(domain_t::server, status::to_underlying(server_code_t::has_error));

        // CREATE ERROR
        else if( this->m_status.has(_FLAG_ERR_CREATE) )
            return Status::err(domain_t::server, status::to_underlying(server_code_t::server_not_create));

        // ALREADY BINDED
        else if( this->m_status.has(_FLAG_ALREADY_BIND) )
            return Status::warn(domain_t::server, status::to_underlying(server_code_t::already_bind));

        // IS RUNNING
        else if( this->is_running() )
            return Status::err(domain_t::server, status::to_underlying(socket_code_t::socket_already_running));

        // BIND RESULT VARIABLE
        int tm_bind = ss_inv_bind;

        // REUSEADDR
        #if __OS_POSIX__
            int tm_reuse_addr = 1;
            ::setsockopt(this->get_socket(), SOL_SOCKET, SO_REUSEADDR, &tm_reuse_addr, sizeof(tm_reuse_addr));
        #elif __OS_WINDOWS__
            int tm_reuse_addr = 1;
            ::setsockopt(this->get_socket(), SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&tm_reuse_addr), sizeof(tm_reuse_addr));
        #endif

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

                tm_bind = ::bind(this->get_socket(), reinterpret_cast<sockaddr*>(&tm_addrv6), sizeof(tm_addrv6));
                this->get_flag().add(_FLAG_SOCKET_IPV6);
            }
            break;

            // IPV4
            case ipv_t::ipv4:
            {
                sockaddr_in tm_addrv4 {};

                tm_addrv4.sin_family = tcp::ipv4::domain;
                tm_addrv4.sin_addr.s_addr = INADDR_ANY;
                tm_addrv4.sin_port = htons(this->get_port());

                tm_bind = ::bind(this->get_socket(), reinterpret_cast<sockaddr*>(&tm_addrv4), sizeof(tm_addrv4));
                this->get_flag().unset(_FLAG_SOCKET_IPV6);
            }
            break;

            // NONE
            case ipv_t::none:
            default:
                return Status::err(domain_t::server, status::to_underlying(server_code_t::server_has_no_ip_type));
        }

        // BIND
        if( ss_inv_bind == tm_bind )
        {
            // STATUS FLAG
            this->m_status.change(_FLAG_ALREADY_BIND, _FLAG_ERR_BIND);

            // BIND ERROR POSIX
            #if __OS_POSIX__
                if(errno == EADDRINUSE ) {
                    this->m_status.add(_FLAG_ERR_PORT_ALREADY_IN_USE);
                    return Status::err(domain_t::server, status::to_underlying(server_code_t::server_port_already_in_use));
                }
            // BIND ERROR WINDOWS
            #elif __OS_WINDOWS__
                if( WSAGetLastError() == WSAEADDRINUSE ) {
                    this->m_status.add(_FLAG_ERR_PORT_ALREADY_IN_USE);
                    return Status::err(domain_t::server, status::to_underlying(server_code_t::server_port_already_in_use));
                }
            #endif

            return Status::err(domain_t::server, status::to_underlying(server_code_t::server_not_bind));
        }

        // STATUS FLAG
        this->m_status.change(_FLAG_ERR_BIND, _FLAG_ALREADY_BIND);
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
        if( Socket::has_error() )
            return Status::err(domain_t::server, status::to_underlying(server_code_t::has_error));
        // NOT BINDED
        else if( this->m_status.has(_FLAG_ERR_BIND) )
            return Status::err(domain_t::server, status::to_underlying(server_code_t::server_not_bind));
        // HAS LISTEN
        else if( this->m_status.has(_FLAG_ALREADY_LISTEN) )
            return Status::warn(domain_t::server, status::to_underlying(server_code_t::already_listen));
        // IS RUNNING
        else if( this->is_running() )
            return Status::err(domain_t::server, status::to_underlying(socket_code_t::socket_already_running));

        // LISTEN
        int tm_backlog = static_cast<int>(this->get_policy().get_max_connection());
        int tm_res = ::listen(this->get_socket(), tm_backlog);
        if( tm_res != ss_vld_listen )
        {
            this->m_status.change(_FLAG_ALREADY_LISTEN, _FLAG_ERR_LISTEN);
            return Status::err(domain_t::server, status::to_underlying(server_code_t::server_not_listen));
        }

        // STATUS FLAG
        this->m_status.change(_FLAG_ERR_LISTEN, _FLAG_ALREADY_LISTEN);
        return Status::ok(domain_t::server, status::to_underlying(server_code_t::server_listen));
    }

    /**
     * @brief Do Accept
     * 
     * Sunucuya bir soket bağlanmaya çalıştıktan sonra
     * buna sunucunun izin verip vermeyeceğini sağlarız.
     * Hata varsa bunu bildirir
     * 
     * @param socket_t& Client Socket
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
        // LOOP
        while( this->is_running() )
        {
            // CLIENT SOCKET
            socket_t tm_cli_accpt = ss_inv_socket;

            // ACCEPT
            if( !this->do_accept(tm_cli_accpt).is_ok() ) {
                // FOR FREEBSD
                if( !this->is_running() )
                    break;

                continue;
            }

            // CLIENT IP
            std::string tm_ip = Socket::get_ip(tm_cli_accpt);

            // THREAD POOL
            this->m_tpool.enqueue([this, tm_cli_accpt, tm_ip]
            {
                // SOCKET VERSION HANDSHAKE RECEIVE
                Status tm_handshake_recv = Socket::handshake_recv_verify(tm_cli_accpt, false);
                if( !tm_handshake_recv.is_ok() )
                {
                    // DEBUG LOG
                    DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), tm_ip + " Version Hash Receive Error, Code: " + std::to_string(tm_handshake_recv.get_code()), GET_SOURCE));

                    // CLOSE SOCKET
                    Socket::close_socket(tm_cli_accpt);
                    return;
                }

                // SOCKET VERSION HASH SEND
                Status tm_handshake_send = Socket::handshake_send_verify(tm_cli_accpt);
                if( !tm_handshake_send.is_ok() )
                {
                    // DEBUG LOG
                    DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), tm_ip + " Version Hash Send Error, Code: " + std::to_string(tm_handshake_send.get_code()), GET_SOURCE));

                    // CLOSE SOCKET
                    Socket::close_socket(tm_cli_accpt);
                    return;
                }

                // BANNED OR ALLOWED ?
                Status tm_allow = this->get_policy().can_allow(tm_ip);
                if( !tm_allow.is_ok() )
                {
                    switch(tm_allow.get_code())
                    {
                        case status::to_underlying(policy::policy_e::not_allow_ipaddr_already_banned):
                            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), tm_ip + " Ip Banned", GET_SOURCE));
                        break;

                        case status::to_underlying(policy::policy_e::not_allow_ipaddr_not_in_list):
                            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), tm_ip + " Ip Not In The Allowed List", GET_SOURCE));
                        break;
                    }

                    // CLOSE SOCKET
                    Socket::close_socket(tm_cli_accpt);
                    return;
                }

                // SAME IP COUNT VARIABLE
                size_t tm_same_ip_count = 0;

                // INCREASE SAME IP COUNT
                {
                    std::scoped_lock tm_lock(this->m_mtx);

                    if( this->m_ip_count.find(tm_ip) == this->m_ip_count.end() )
                        this->m_ip_count.emplace(tm_ip, 0);

                    ++this->m_ip_count.at(tm_ip);
                    tm_same_ip_count = this->m_ip_count.at(tm_ip);

                    // OVER MAX CONNECTION ?
                    if( this->m_clients.size() > this->get_policy().get_max_connection() )
                    {
                        DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Over total connection limit", GET_SOURCE));

                        Socket::close_socket(tm_cli_accpt);
                        return;
                    }

                    // OVER MAX SAME IP ?
                    if( tm_same_ip_count > this->get_policy().get_max_same_ip() )
                    {
                        DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Over total same ip limit", GET_SOURCE));

                        Socket::close_socket(tm_cli_accpt);
                        return;
                    }

                    // ADD TO CLIENT LIST
                    this->m_clients.emplace(tm_cli_accpt, SocketCtx{ UserPacket{}, tm_ip });
                }

                DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), tm_ip + " Added To Client List", GET_SOURCE));

                // LOG
                if( this->get_flag().has(_FLAG_SOCKET_LOGGER) )
                    this->get_logger().write(level_t::Info, this->get_policy().get_username(), tm_ip + " Connected");

                SocketCtx tm_client;
                {
                    std::scoped_lock tm_lock(this->m_mtx);

                    auto tm_it = this->m_clients.find(tm_cli_accpt);
                    if( tm_it == this->m_clients.end() )
                        return;

                    tm_it->second.m_ip = tm_ip;
                    tm_it->second.m_user.m_same_user_count = tm_same_ip_count;
                    tm_it->second.m_user.m_try_passwd = 0;

                    tm_client = tm_it->second;
                }

                // RUN
                DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Server Handler Starting...", GET_SOURCE));

                if( this->m_handler )
                    this->m_handler(*this, tm_cli_accpt, tm_client);
                
                // CLOSE
                Socket::close_socket(tm_cli_accpt);
                DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Client Socket Closed, Ip/SameUserCount: " + (tm_client.m_ip) + '/' + std::to_string(tm_same_ip_count), GET_SOURCE));
                
                // DECREASE SAME IP
                {
                    std::scoped_lock tm_lock(this->m_mtx);

                    if( this->m_ip_count.find(tm_ip) != this->m_ip_count.end() )
                    {
                        if( this->m_ip_count.at(tm_ip) ) --this->m_ip_count.at(tm_ip);
                        else this->m_ip_count.at(tm_ip) = 0;
                    }
                    else
                        this->m_ip_count.emplace(tm_ip, 0);
                }

                // LOG
                if( this->get_flag().has(_FLAG_SOCKET_LOGGER) )
                    this->get_logger().write(level_t::Info, this->get_policy().get_username(), tm_ip + " Disconnected");

                // REMOVE CLIENT FROM LIST
                {
                    std::scoped_lock tm_lock(this->m_mtx);
                    this->m_clients.erase(tm_cli_accpt);
                }
            });
        }
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
        std::scoped_lock tm_lock(this->m_mtx);
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
        return Socket::has_error() || this->m_status.has_any(
            _FLAG_ERR_CREATE, _FLAG_ERR_BIND,
            _FLAG_ERR_LISTEN, _FLAG_ERR_INIT,
            _FLAG_ERR_PORT_ALREADY_IN_USE
        );
    }

    /**
     * @brief Get Total Server
     * 
     * Programda aktif olarak bulunan
     * sunucu miktarını depolayan değişkenin
     * değerini döndürür
     * 
     * @return size_t
     */
    size_t Server::get_total_server() noexcept
    {
        std::scoped_lock tm_lock(s_port_mtx);
        return s_total_server;
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
        std::scoped_lock tm_lock(this->m_mtx);
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
     * @brief Get Status Flag
     * 
     * Tutulan durum bayrağı nesnesini döndürür
     * 
     * @return const Flag&
     */
    const flag::Flag& Server::get_status_flag() const noexcept
    {
        return this->m_status;
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
        // IS ALREADY RUNNING ?
        if( this->is_running() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Server Port Cannot Change While Server Still Running", GET_SOURCE));
            return Status::err(domain_t::server, status::to_underlying(server_code_t::server_already_running));
        }

        // IS VALID PORT ?
        if( !Socket::is_valid_port(ar_port) )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Server Set Port Not Valid, Port: " + std::to_string(ar_port), GET_SOURCE));
            return Status::err(domain_t::server, status::to_underlying(socket_code_t::port_not_valid));
        }

        // SET THE PORT
        Status tm_status = Socket::set_port(ar_port);
        if( !tm_status.is_ok() && !tm_status.is_warn() )
            return Status::err(domain_t::server, tm_status.get_code());

        // ADD NEW PORT TO LIST
        {
            std::scoped_lock tm_lock(s_port_mtx);

            this->m_status.unset(_FLAG_ERR_PORT_ALREADY_IN_USE);
            this->m_status.change(_FLAG_ALREADY_BIND, _FLAG_ERR_BIND);
            this->m_status.change(_FLAG_ALREADY_LISTEN, _FLAG_ERR_LISTEN);
            this->m_status.change(_FLAG_ALREADY_INIT, _FLAG_ERR_INIT);
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
        // IS RUNNING ?
        if( this->is_running() )
            return Status::err(domain_t::server, status::to_underlying(server_code_t::server_already_running));

        this->m_handler = std::move(ar_handler);
        return Status::ok(domain_t::server, status::to_underlying(server_code_t::handler_set));
    }

    /**
     * @brief Create
     * 
     * Ana sınıf fonksiyonuna ek olarak
     * yapılması gerekenleri ekleyerek daha iyi
     * bir fonksion elde edilmiş olur
     * 
     * @return Status
     */
    Status Server::create() noexcept
    {
        // HAS ALREADY
        if( Socket::is_valid_socket(this->get_socket()) )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Server Already Has Valid Socket", GET_SOURCE));

            // SET FLAG
            this->m_status.change(_FLAG_ERR_CREATE, _FLAG_ALREADY_CREATE);
            return Status::warn(domain_t::server, status::to_underlying(socket_code_t::socket_has_already));
        }

        // RESET FLAG
        this->m_status.change(_FLAG_ALREADY_CREATE, _FLAG_ERR_CREATE);

        // CREATE
        Status tm_status = Socket::create();
        if( !tm_status.is_ok() && !tm_status.is_warn() && !Socket::is_valid_socket(this->get_socket()) )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Server Create Socket Error, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
            return Status::err(domain_t::server, status::to_underlying(socket_code_t::socket_not_create));
        }

        // SET FLAG
        this->m_status.change(_FLAG_ERR_CREATE, _FLAG_ALREADY_CREATE);
        return Status::ok(domain_t::server, status::to_underlying(socket_code_t::socket_create));
    }

    /**
     * @brief Close
     * 
     * Ana sınıf fonksiyonuna ek olarak
     * yapılması gerekenleri ekleyerek daha iyi
     * bir fonksion elde edilmiş olur
     * 
     * @return Status
     */
    Status Server::close() noexcept
    {
        // CLOSE
        Status tm_status = Socket::close();
        return tm_status.is_ok() ?
            Status::ok(domain_t::server, tm_status.get_code()) :
            Status::err(domain_t::server, tm_status.get_code());
    }

    /**
     * @brief Clear
     * 
     * Ana sınıf fonksiyonuna ek olarak
     * yapılması gerekenleri ekleyerek daha iyi
     * bir fonksion elde edilmiş olur
     * 
     * @return Status
     */
    Status Server::clear() noexcept
    {
        // CLEAR
        Status tm_status = Socket::clear();
        if( !tm_status.is_ok() && !tm_status.is_warn() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Server Clear Error, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
            return Status::err(domain_t::server, tm_status.get_code());
        }

        {
            std::scoped_lock tm_lock(this->m_mtx);

            // CLEAR FLAG
            this->m_status.clear();

            // CLEAR CLIENT LIST
            this->m_clients.clear();
        }

        return Status::ok(domain_t::server, status::to_underlying(server_code_t::server_clear));
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
        const DataPacket& ar_datapack
    ) noexcept
    {
        // FIND CLIENT
        if( this->m_clients.find(ar_sock) == this->m_clients.end() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Socket Not Found In Client List", GET_SOURCE));
            return Status::err(domain_t::server, status::to_underlying(server_code_t::socket_not_found_in_client_list));
        }

        // GET IP
        const std::string tm_ip = Socket::get_ip(ar_sock);

        // IS ALLOWED AND NOT BANNED
        if( !this->get_policy().is_connection_allowed(tm_ip) )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), tm_ip + " Ip Address Not Allowed", GET_SOURCE));
            return Status::err(domain_t::server, status::to_underlying(server_code_t::client_not_allowed));
        }
        else if( this->get_policy().is_connection_banned(ar_sock) )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), tm_ip + " Ip Address, " + std::to_string(ar_sock) + " Socket Banned", GET_SOURCE));
            return Status::err(domain_t::server, status::to_underlying(server_code_t::client_banned_from_server));
        }

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

        // CLIENT
        auto& tm_client = tm_client_it->second;

        // GET IP
        const std::string tm_ip = Socket::get_ip(ar_sock);

        // IS ALLOWED AND NOT BANNED
        if( !this->get_policy().is_connection_allowed(tm_ip) )
        {
            // BRUTE FORCE TIMEOUT
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), tm_ip + " Ip Address Not Allowed", GET_SOURCE));
            return Status::err(domain_t::server, status::to_underlying(server_code_t::client_not_allowed));
        }
        else if( this->get_policy().is_connection_banned(ar_sock) )
        {
            // BRUTE FORCE TIMEOUT
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), tm_ip + " Ip Address, " + std::to_string(ar_sock) + " Socket Banned", GET_SOURCE));
            return Status::err(domain_t::server, status::to_underlying(server_code_t::client_banned_from_server));
        }

        // STATUS VARIABLE
        Status tm_status;

        // RECEIVE
        tm_status = Socket::recv(ar_sock, ar_datapack);
        if( !tm_status.is_ok() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Server Receive Error, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
            return Status::err(domain_t::server, status::to_underlying(server_code_t::fail_recv_from_client));
        }

        // CAN AUTH ?
        tm_status = this->get_policy().can_auth(tm_ip, ar_datapack.m_pwd);
        if( tm_status.is_ok() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), tm_ip + " Auth Successful, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
            return Status::ok(domain_t::server, status::to_underlying(server_code_t::client_recv_with_auth));
        }

        // WHY AUTH FAILED
        // IS DATA EMPTY ?
        if( ar_datapack.m_pwd.empty() && ar_datapack.m_msg.empty() && ar_datapack.m_name.empty() )
            return Status::err(domain_t::server, status::to_underlying(server_code_t::client_cannot_auth_no_data));

        // ENTERED WRONG PASSWORD
        ++(tm_client.m_user.m_try_passwd);

        // LOG THE WRONG PASSWORD ATTEMPT
        DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), tm_ip + "/" + std::to_string(tm_client.m_user.m_same_user_count) + " Sent Wrong Password Attempt #" + std::to_string(tm_client.m_user.m_try_passwd)));

        // PASSWORD VARIABLE TYPE
        using pwd_t = decltype(tm_client.m_user.m_try_passwd);
            
        // EQUAL MAX RETRY PASSWORD
        if( tm_client.m_user.m_try_passwd == static_cast<pwd_t>(_MAX_RETRY_PWD) )
        {
            // BAN
            this->get_policy().set_ban(ar_sock, true);

            // CLOSE & SHUTDOWN
            Socket::shutdown_socket(ar_sock);
            Socket::close_socket(ar_sock);

            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), tm_ip + "/" + std::to_string(tm_client.m_user.m_same_user_count) + " Ip Client Reached Try Password Limit", GET_SOURCE));

            // DELETE CLIENT
            this->delete_client(ar_sock);
            return Status::err(domain_t::server, status::to_underlying(server_code_t::server_banned_client));
        }
        // OVER MAX RETRY PASSWORD (POTENTIAL ATTACKER)
        else if( tm_client_it->second.m_user.m_try_passwd > static_cast<pwd_t>(_MAX_RETRY_PWD) )
        {
            // BAN
            this->get_policy().set_ban(ar_sock, true);

            // CLOSE & SHUTDOWN
            Socket::shutdown_socket(ar_sock);
            Socket::close_socket(ar_sock);

            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), tm_ip + "/" + std::to_string(tm_client.m_user.m_same_user_count) + " Ip Client Reached Try Password Limit", GET_SOURCE));

            // DELETE CLIENT
            this->delete_client(ar_sock);
            return Status::err(domain_t::server, status::to_underlying(server_code_t::server_banned_potential_attacker));
        }

        // CLIENT ENTERING OLD PASSWORD
        if( tm_status.get_code() == status::to_underlying(policy::policy_e::cannot_auth_with_old_password))
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), tm_ip + "/" + std::to_string(tm_client.m_user.m_same_user_count) + " Ip Client Trying Old Password", GET_SOURCE));
            return Status::err(domain_t::server, status::to_underlying(server_code_t::client_entered_old_password));
        }
        // WRONG PASSWORD
        else if( tm_status.get_code() == status::to_underlying(policy::policy_e::cannot_auth_password_not_matched) )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), tm_ip + "/" + std::to_string(tm_client.m_user.m_same_user_count) + " Ip Client Password Not Matched", GET_SOURCE));
            return Status::err(domain_t::server, status::to_underlying(server_code_t::client_sent_wrong_password));
        }

        // RECEIVE ERROR
        return Status::err(domain_t::server, status::to_underlying(server_code_t::server_recv_err));
    }

    /**
     * @brief Initialize
     * 
     * Soketin çalışması için gerekli olan ve
     * tekrar tekrar çalıştırmayı deneyebileceğimiz
     * bir yapıyı içerir. Bazen hatalar oluşabilir
     * fakat hata düzeltildikten sonra tekrar
     * başlatmayı sağlayabilmemiz lazım yoksa sunucu
     * sonsuza dek hatalı gibi gözükür ama hata çoktan
     * çözülmüştür
     * 
     * @return Status
     */
    Status Server::init() noexcept
    {
        // ALREADY INITIALIZED
        if( this->m_status.has(_FLAG_ALREADY_INIT) )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Server Already Initialized", GET_SOURCE));
            return Status::warn(domain_t::server, status::to_underlying(server_code_t::server_already_init));
        }

        // PORT ALREADY IN USE
        if( this->m_status.has(_FLAG_ERR_PORT_ALREADY_IN_USE) )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Server Cannot Initialize, Port Already In Use", GET_SOURCE));
            return Status::err(domain_t::server, status::to_underlying(server_code_t::server_port_already_in_use));
        }

        // ERROR FLAG
        this->m_status.change(_FLAG_ALREADY_INIT, _FLAG_ERR_INIT);
        this->m_status.add(_FLAG_ERR_CREATE, _FLAG_ERR_BIND, _FLAG_ERR_LISTEN);

        // STATUS VARIABLE
        Status tm_status;

        // CREATE
        tm_status = this->create();
        if( !tm_status.is_ok() && !tm_status.is_warn() && !Socket::is_valid_socket(this->get_socket()) )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Create Error, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
            return tm_status;
        }

        // BIND
        tm_status = this->do_bind();
        if( !tm_status.is_ok() && !tm_status.is_warn() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Bind Error, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
            return tm_status;
        }

        // LISTEN
        tm_status = this->do_listen();
        if( !tm_status.is_ok() && !tm_status.is_warn() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Listen Error, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
            return tm_status;
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

        // STATUS FLAG
        this->m_status.change(_FLAG_ERR_INIT, _FLAG_ALREADY_INIT);
        return Status::ok(domain_t::server, status::to_underlying(server_code_t::server_init));
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
        if( Socket::has_error() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Run Has Error, Flag: " + std::to_string(this->m_status.get()), GET_SOURCE));
            return Status::err(domain_t::server, status::to_underlying(server_code_t::has_error));
        }

        // INITIALIZE IF NOT
        if( this->m_status.has(_FLAG_ERR_INIT) )
        {
            Status tm_status = this->init();
            if( !tm_status.is_ok() && !tm_status.is_warn() )
            {
                // DEBUG LOG
                DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Run Re-Initialize Error, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
                return Status::err(domain_t::server, tm_status.get_code());
            }
        }

        // HAS ERROR
        if( this->has_error() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Run Has Critical Error, Flag: " + std::to_string(this->m_status.get()), GET_SOURCE));
            return Status::err(domain_t::server, status::to_underlying(server_code_t::critical_error));
        }

        // ALREADY RUNNING
        if( this->is_running() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Is Running Error, Flag: " + std::to_string(this->m_status.get()), GET_SOURCE));
            return Status::err(domain_t::server, status::to_underlying(server_code_t::server_already_running));
        }

        // NOT VALID SOCKET
        if( !Socket::is_valid_socket(this->get_socket()) )
        { 
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Socket Is Not Valid, Flag: " + std::to_string(this->m_status.get()), GET_SOURCE));
            return Status::err(domain_t::server, status::to_underlying(socket_code_t::socket_not_valid));
        }

        // SET RUNNING
        Status tm_status = this->set_running(true);
        if( !tm_status.is_ok() && !this->is_running() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Set Running Error, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
            return Status::err(domain_t::server, status::to_underlying(socket_code_t::socket_no_run));
        }

        // SERVER RUNNER
        try {
            this->m_worker = std::thread([this]{ this->server_runner(); });

            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Server Worker Started To Run: " + std::to_string(this->m_status.get()), GET_SOURCE));
        } catch (...) {
            this->set_running(false);

            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Server Worker Running Error, Flag: " + std::to_string(this->m_status.get()), GET_SOURCE));
            return Status::err(domain_t::server, status::to_underlying(server_code_t::server_run_thread_fail));
        }

        // OK
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
        // STATUS VARIABLE
        Status tm_status = this->set_running(false);

        // TRY THE STOP
        if( !tm_status.is_ok() && !this->is_running() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Stop Error, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
            return Status::err(domain_t::server, tm_status.get_code());
        }

        // CLOSE SOCKET
        tm_status = this->close();
        if( !tm_status.is_ok() && Socket::is_valid_socket(this->get_socket()) )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Close Error, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
            return Status::err(domain_t::server, tm_status.get_code());
        }

        // DEBUG LOG
        DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Waiting For Stop Thread", GET_SOURCE));
        
        // WAIT FOR THREAD
        if( this->m_worker.joinable() )
            this->m_worker.join();

        // STOP THREADS
        this->m_tpool.stop();

        // DEBUG LOG
        DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Thread Stopped", GET_SOURCE));

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
        this->get_logger().write(std::string("Name: ").append(this->get_policy().get_username()));
        this->get_logger().write(std::string("Base Flag: ").append(std::to_string(this->get_flag().get())));
        this->get_logger().write(std::string("Status Flag: ").append(std::to_string(this->get_status_flag().get())));
        this->get_logger().write(std::string("Is Full: ").append(this->is_full() ? "Yes" : "No"));
        this->get_logger().write(std::string("Is Empty: ").append(this->is_empty() ? "Yes" : "No"));
        this->get_logger().write(std::string("Is Running: ").append(this->is_running() ? "Yes" : "No"));
        this->get_logger().write(std::string("Has Error: ").append(this->has_error() ? "Yes" : "No"));
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
        // DEBUG LOG
        DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Server Crash, Flag: " + std::to_string(this->m_status.get()), GET_SOURCE));

        this->stop();
        Socket::crashed();
    }
}
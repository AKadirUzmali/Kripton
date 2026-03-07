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
 */

// Include
#include <functional>
#include <unordered_map>

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

    // Version Hash
    static constexpr Vch<32> ss_verhash("2026.03.07|server", 1303);

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

        succ = 1000,
        server_bind,
        server_listen,
        server_accept,
        server_init,
        accept_client,

        warn = 2000,
        already_bind,
        already_listen,
        already_init,

        info = 3000
    };

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
            static constexpr flag::flag_t _FLAG_SERVER_ERR = { 1 << 31 };
            static constexpr flag::flag_t _FLAG_DATA_SET_WARN = { 1 << 29 };
            static constexpr flag::flag_t _FLAG_RUNNING = { 1 << 28 };
            static constexpr flag::flag_t _FLAG_ALREADY_BIND = { 1 << 27 };
            static constexpr flag::flag_t _FLAG_ALREADY_LISTEN = { 1 << 26 };
            static constexpr flag::flag_t _FLAG_ALREADY_INIT = { 1 << 25 };

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
            Status do_init() noexcept;

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
                const socket_port_t ar_port,
                const ipv_t ar_ipv,
                const policy::max_conn_t ar_max_client = policy::_DEF_CONNECTION,
                const policy::max_conn_t ar_max_same_ip = policy::_DEF_SAME_IP_COUNT,
                const flag::flag_t ar_flag = _FLAG_SOCKET_NULL
            );

            inline bool is_running() const noexcept;
            inline bool is_full() const noexcept;
            inline bool is_empty() const noexcept;

            inline bool has_error() const noexcept override;

            inline const std::unordered_map<socket_t, SocketCtx>& get_clients() const noexcept;
            inline std::size_t get_client_count() const noexcept;

            Status delete_client(const socket_t ar_sock) noexcept;

            Status set_handler(data_handler& ar_handler) noexcept;

            Status send(const socket_t ar_sock, DataPacket& ar_datapack) noexcept override;
            Status recv(const socket_t ar_sock, DataPacket& ar_datapack) noexcept override;

            Status run() noexcept;
            Status stop() noexcept;

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
        const socket_port_t ar_port,
        const ipv_t ar_ipv,
        const policy::max_conn_t ar_max_client,
        const policy::max_conn_t ar_max_same_ip,
        const flag::flag_t ar_flag
    )
    : Socket(ar_cipher, ar_filepath, ar_name, ar_password, ar_port, ar_ipv, ar_flag)
    {
        Status tm_status;

        // MAX CONNECTION
        tm_status = this->get_policy().set_max_connection(ar_max_client);
        if( !tm_status.is_ok() )
            this->m_status.set(_FLAG_DATA_SET_WARN);

        // MAX SAME IP
        tm_status = this->get_policy().set_max_same_ip(ar_max_same_ip);
        if( !tm_status.is_ok() )
            this->m_status.set(_FLAG_DATA_SET_WARN);

        // CREATE
        tm_status = this->create();
        if( !tm_status.is_ok() )
            this->m_status.set(_FLAG_SERVER_ERR);
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
        else if( !this->m_status.has(_FLAG_ALREADY_INIT) )
            return Status::err(domain_t::server, status::to_underlying(server_code_t::server_has_to_be_init));

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
            this->m_status.add(_FLAG_SERVER_ERR);
            return Status::err(domain_t::server, status::to_underlying(server_code_t::server_not_bind));
        }

        this->m_status.add(_FLAG_ALREADY_BIND);
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
        else if( this->m_status.has(_FLAG_ALREADY_LISTEN) )
            return Status::warn(domain_t::server, status::to_underlying(server_code_t::already_listen));
        else if( !this->m_status.has(_FLAG_ALREADY_INIT) )
            return Status::err(domain_t::server, status::to_underlying(server_code_t::server_has_to_be_init));

        // LISTEN
        int tm_backlog = static_cast<int>(this->get_policy().get_max_connection());
        int tm_res = ::listen(this->get_socket(), tm_backlog);
        if( tm_res != ss_vld_listen )
        {
            this->m_status.add(_FLAG_SERVER_ERR);
            return Status::err(domain_t::server, status::to_underlying(server_code_t::server_not_listen));
        }

        this->m_status.add(_FLAG_ALREADY_LISTEN);
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
        else if( !this->m_status.has(_FLAG_ALREADY_INIT) )
            return Status::err(domain_t::server, status::to_underlying(server_code_t::server_has_to_be_init));

        // VARIABLE
        sockaddr_storage tm_sockstore {};
        socklen_t tm_sock_len = sizeof(tm_sockstore);
        socket_t tm_server_sock = this->get_socket();

        // ACCEPT
        socket_t tm_accpt = ::accept(tm_server_sock, reinterpret_cast<sockaddr*>(&tm_sockstore), &tm_sock_len);
        if( !Socket::is_valid_socket(tm_accpt) )
            return Status::err(domain_t::server, status::to_underlying(server_code_t::accept_socket_not_valid));

        ar_cli_sock = tm_accpt;
        return Status::ok(domain_t::server, status::to_underlying(server_code_t::accept_client));
    }

    /**
     * @brief Do Init
     * 
     * Sunucu olduğu gibi direk başlatılamaz.
     * Başlatma kontrolleri ve aşamalarını izleyerek sunucuyu
     * başlatacak ya da hata döndürerek bunu bildirecek
     * 
     * @return Status
     */
    Status Server::do_init() noexcept
    {
        // ERROR
        if( this->has_error() )
            return Status::err(domain_t::server, status::to_underlying(server_code_t::has_error));
        else if( this->m_status.has(_FLAG_ALREADY_INIT) )
            return Status::warn(domain_t::server, status::to_underlying(server_code_t::already_init));

        // VARIABLE
        Status tm_status;

        // CREATE
        tm_status = this->create();
        if( !tm_status.is_ok() )
            return Status::err(domain_t::server, tm_status.get_code());

        // BIND
        tm_status = this->do_bind();
        if( !tm_status.is_ok() )
            return tm_status;

        // LISTEN
        tm_status = this->do_listen();
        if( !tm_status.is_ok() )
            return tm_status;

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
        return Status::ok(domain_t::server, status::to_underlying(server_code_t::server_init));
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
        // ERROR
        if( this->has_error() || !Socket::is_valid_socket(ar_cli_sock))
            return;

        // TIMEOUT VARIABLE
        const auto tm_timeout = std::chrono::seconds(this->get_timeout());
        const std::string tm_msg_timeout = " idle timeout over limit (" + std::to_string(this->get_timeout()) + ")";

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
                if( this->get_flag().has(_FLAG_SOCKET_LOGGER) )
                    this->get_logger().write(level_t::Warn, ar_ipaddr + " banned");
            }
            // ALLOW CHECK
            else if( !this->get_policy().is_connection_allowed(ar_ipaddr) )
            {
                if( this->get_flag().has(_FLAG_SOCKET_LOGGER) )
                    this->get_logger().write(level_t::Warn, ar_ipaddr + " not allowed");
            }

            // DATA PACKET & RECV
            DataPacket tm_datapack;
            Status tm_status = this->recv(ar_cli_sock, tm_datapack);

            // RECV STATUS
            switch( tm_status.get_code() ) {
                case status::to_underlying(socket_code_t::socket_data_recv):            
                    if( this->get_flag().has(_FLAG_SOCKET_LOGGER) )
                        this->get_logger().write(level_t::Warn, "received from " + ar_ipaddr);
                break;

                case status::to_underlying(socket_code_t::packet_no_data):
                    if( this->get_flag().has(_FLAG_SOCKET_LOGGER) )
                        this->get_logger().write(level_t::Warn, "no data from " + ar_ipaddr);
                break;

                default:
                    if( this->get_flag().has(_FLAG_SOCKET_LOGGER) )
                        this->get_logger().write(level_t::Warn, "data couldn't receive from " + ar_ipaddr + " | code: " + std::to_string(tm_status.get_code()));
                return;
            }

            // SEND
            tm_status = this->send(ar_cli_sock, tm_datapack);

            // SEND STATUS
            switch( tm_status.get_code() )
            {
                case status::to_underlying(socket_code_t::socket_data_sent):
                    if( this->get_flag().has(_FLAG_SOCKET_LOGGER) )
                        this->get_logger().write(level_t::Warn, "sent to " + ar_ipaddr, GET_SOURCE);
                    break;

                default:
                    if( this->get_flag().has(_FLAG_SOCKET_LOGGER) )
                        this->get_logger().write(level_t::Warn, "data couldn't send to " + ar_ipaddr + " | code: " + std::to_string(tm_status.get_code()));
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
        return this->m_running.load(std::memory_order_relaxed) && !CrashHandler::is_signal();
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
        return Socket::has_error() || this->m_status.has(_FLAG_SERVER_ERR);
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

    }

    /**
     * @brief Crashed
     * 
     * Sunucunun bir anda çökmesi durumunda yapılacakları
     * işleme alır ve olası donanım israfını önlemeyi amaçlar
     */
    void Server::crashed() noexcept
    {

    }
}
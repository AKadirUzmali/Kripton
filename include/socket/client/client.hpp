// Abdulkadir U. - 2026/02/27
#pragma once

/**
 * Client (İstemci)
 * 
 * İstemci olarak tek bir amaç vardır, sunucuya bağlan
 * ve iletişimde kal. Sunucu ile yapılan işlemlere
 * benzer şekilde işlemler ile sunucu iletişimde kalarak
 * veri akışını sağlamaya çalışırız. Sınıf içinde bulunan
 * fonksiyon tutucu nesne ile de gelecekte de kullanılabilmesini
 * sağlamış olduk
 */

// Include
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <kits/corekit.hpp>
#include <kits/hashkit.hpp>

#include <dev/developer.hpp>

#include <pool/cipherpool.hpp>
#include <pool/threadpool.hpp>

#include <socket/socket.hpp>
#include <socket/policy.hpp>
#include <socket/netpacket.hpp>

// Namespace
namespace netsocket::client
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
        static constexpr int ss_inv_connect = SOCKET_ERROR;
    #elif __OS_POSIX__
        static constexpr int ss_inv_connect = -1;
    #else
        #error "[ERROR] Unsupported Operating System!"
    #endif

    // Enum
    enum class client_code_t : uint16_t
    {
        err = 20000,
        has_error,
        server_ip_not_valid,
        server_ipv6_not_valid,
        server_ipv4_not_valid,
        connection_err,
        server_send_handshake_err,
        server_recv_handshake_err,
        server_not_ip_set,
        stop_socket_before_connection_handler_set,
        client_could_not_stop,
        client_stopped,
        client_already_running,
        client_run_thread_fail,
        client_not_initialized,
        client_init_create_err,
        client_already_init,
        client_already_connect,
        client_reinit_err,
        client_not_running,
        server_ip_type_not_valid,
        
        succ = 21000,
        connected_and_handshake_verified,
        server_ip_set,
        connection_handler_set,
        client_started_to_run,
        client_init,
        client_clear,

        warn = 22000,

        info = 23000
    };

    // Struct
    struct ConnectCtx
    {
        socket_port_t m_port = 0;
        ipv_t m_ipv = ipv_t::none;
        std::string m_ip = "0.0.0.0";
    };

    // Class
    class Client final : public Socket
    {
        public:
            using conn_handler = std::function<void(Client&)>;

        private:
            // Flag
            static constexpr flag::flag_t _FLAG_ERR_SERVER_IP = { 1 << 29 };
            static constexpr flag::flag_t _FLAG_ERR_HANDLER = { 1 << 28 };
            static constexpr flag::flag_t _FLAG_ERR_CREATE = { 1 << 27 };
            static constexpr flag::flag_t _FLAG_ERR_INIT = { 1 << 26 };
            static constexpr flag::flag_t _FLAG_ERR_CONNECT = { 1 << 25 };
            static constexpr flag::flag_t _FLAG_ALREADY_CREATE = { 1 << 24 };
            static constexpr flag::flag_t _FLAG_ALREADY_INIT = { 1 << 23 };
            static constexpr flag::flag_t _FLAG_ALREADY_CONNECT = { 1 << 22 };
            static constexpr flag::flag_t _FLAG_ALREADY_SERVER_IP = { 1 << 21 };

        private:
            static inline std::mutex s_connects_mtx;
            static inline std::unordered_map<socket_t, ConnectCtx> s_connects;

            std::thread m_worker;
            ThreadPool m_tpool;

            std::atomic<bool> m_running { false };
            std::string m_server_ip;

            flag::Flag m_status { _FLAG_SOCKET_NULL };

            conn_handler m_handler;

            mutable std::mutex m_mtx;

        private:
            Status do_connect() noexcept;
            void client_runner() noexcept;

        public:
            explicit Client(
                algorithm::Algorithm& ar_cipher,
                const std::string& ar_filepath,
                const std::string& ar_name,
                const std::string& ar_password,
                const socket_port_t ar_port,
                const ipv_t ar_ipv,
                const std::string& ar_server_ip,
                conn_handler ar_handler,
                const flag::flag_t ar_flag = _FLAG_SOCKET_NULL
            );

            ~Client();
            
            inline bool has_error() const noexcept override;

            inline const std::unordered_map<socket_t, ConnectCtx>& get_connects() const noexcept;
            inline std::size_t get_client_count() const noexcept;
            inline const flag::Flag& get_status_flag() const noexcept;

            inline const std::string& get_server_ip() const noexcept;

            Status set_server_ip(const std::string& ar_ip) noexcept;
            Status set_handler(conn_handler& ar_handler) noexcept;

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
     * @brief Client
     * 
     * Ön kontrolleri yaptıktan sonra ana soket
     * sınıfını kullanarak istemci oluşturur.
     * Bağlanılacak sunucu ip adresini,
     * ip tür sürümünü, işlemi yapacak fonksiyonu
     * ayarlar ve soketi oluştururuz. Sonrasında
     * soket gecikmesini ayarlarız ve başarılı olduğunu
     * belirtecek bayrak değerlerini ayarlayıp sonlandırırız
     * 
     * @param Algorithm& Cipher
     * @param string& Filepath
     * @param string& Name
     * @param string& Password
     * @param socket_port_t Port
     * @param ipv_t Ip Version
     * @param string& Server Ip
     * @param conn_handler& Handler
     * @param flag_t Flag
     */
    Client::Client(
        algorithm::Algorithm& ar_cipher,
        const std::string& ar_filepath,
        const std::string& ar_name,
        const std::string& ar_password,
        const socket_port_t ar_port,
        const ipv_t ar_ipv,
        const std::string& ar_server_ip,
        conn_handler ar_handler,
        const flag::flag_t ar_flag
    )
    : Socket(ar_cipher, ar_filepath, ar_name, ar_password, ar_port, ar_ipv, ar_flag)
    {
        // IS RUNNING
        if( this->is_running() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Client Constructor Running Already Error, Flag: " + std::to_string(this->m_status.get()), GET_SOURCE));

            // LOGGER
            if( this->get_flag().has(_FLAG_SOCKET_LOGGER) )
                this->get_logger().write(level_t::Err, this->get_policy().get_username(), "Client Cannot Create Error, Already Running");

            return;
        }

        // STATUS VARIABLE
        Status tm_status;

        // IP VERSION
        tm_status = this->set_ipv(ar_ipv);
        if( !tm_status.is_ok() && !tm_status.is_warn() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Client Constructor Set Server Ip Version Error, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));

            // LOGGER
            if( this->get_flag().has(_FLAG_SOCKET_LOGGER) )
                this->get_logger().write(level_t::Err, this->get_policy().get_username(), "Client Set Server Ip Version, Code: " + std::to_string(tm_status.get_code()));
        }

        // SERVER IP
        tm_status = this->set_server_ip(ar_server_ip);
        if( !tm_status.is_ok() && !tm_status.is_warn() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Client Constructor Set Server Ip Error, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));

            // LOGGER
            if( this->get_flag().has(_FLAG_SOCKET_LOGGER) )
                this->get_logger().write(level_t::Err, this->get_policy().get_username(), "Client Set Server Ip, Code: " + std::to_string(tm_status.get_code()));
        }

        // HANDLER
        tm_status = this->set_handler(ar_handler);
        if( !tm_status.is_ok() && !tm_status.is_warn() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Client Constructor Set Handler Error, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));

            // LOGGER
            if( this->get_flag().has(_FLAG_SOCKET_LOGGER) )
                this->get_logger().write(level_t::Err, this->get_policy().get_username(), "Client Set Handler, Code: " + std::to_string(tm_status.get_code()));
        }

        // INITIALIZE
        tm_status = this->init();
        if( !tm_status.is_ok() && !tm_status.is_warn() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Client Constructor Initialize Error, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
            return;
        }
    }

    /**
     * @brief ~Client
     * 
     * İstemci sonlandırılmak istendiğinde çalışmayı
     * durdursun. Sonrasında istemciyi listede arasın
     * ve bulunduğunda listeden silsin
     */
    Client::~Client()
    {
        // STOP THIS
        this->stop();

        std::scoped_lock tm_lock(s_connects_mtx);
        
        // ERASE THIS CONNECTION FROM LIST
        auto tm_it = s_connects.find(this->get_socket());
        if( tm_it != s_connects.end() )
            s_connects.erase(tm_it);
    }

    /**
     * @brief Has Error
     * 
     * İstemcinin çalışmasına engel olan ya da
     * olabilecek olası hataların varlığından
     * haberdar olabilmemizi sağlar
     * 
     * @return bool
     */
    bool Client::has_error() const noexcept
    {
        return Socket::has_error() || this->m_status.has_any(
            _FLAG_ERR_CREATE, _FLAG_ERR_HANDLER,
            _FLAG_ERR_SERVER_IP, _FLAG_ERR_INIT
        );
    }

    /**
     * @brief Do Connect
     * 
     * İstemcinin sunucuya bağlanmasını sağlayacak.
     * Bağlanma ya da bağlanmama durumuna göre
     * bir durum döndürecek
     * 
     * @return Status
     */
    Status Client::do_connect() noexcept
    {
        // HAS ERROR
        if( Socket::has_error() )
            return Status::err(domain_t::client, status::to_underlying(client_code_t::has_error));

        // IS NOT RUNNING
        if( !this->is_running() )
            return Status::err(domain_t::client, status::to_underlying(client_code_t::client_not_running));

        // ALREADY CONNECTED
        else if( this->m_status.has(_FLAG_ALREADY_CONNECT) )
            return Status::warn(domain_t::client, status::to_underlying(client_code_t::client_already_connect));

        // NOT INITIALIZED
        else if( !this->m_status.has(_FLAG_ALREADY_INIT) )
            return Status::err(domain_t::client, status::to_underlying(client_code_t::client_not_initialized));

        // CONNECT RESULT VARIABLE
        int tm_connect = ss_inv_connect;

        // IPV6
        if( this->get_ipv() == ipv_t::ipv6 )
        {
            sockaddr_in6 tm_v6 {};
            tm_v6.sin6_family = tcp::ipv6::domain;
            tm_v6.sin6_port = htons(this->get_port());

            if( inet_pton(tcp::ipv6::domain, this->get_server_ip().c_str(), &tm_v6.sin6_addr) != 1 )
                return Status::err(domain_t::client, status::to_underlying(client_code_t::server_ipv6_not_valid));

            tm_connect = ::connect(this->get_socket(), reinterpret_cast<sockaddr*>(&tm_v6), sizeof(tm_v6));
        }
        // IPV4
        else
        {
            sockaddr_in tm_v4 {};
            tm_v4.sin_family = tcp::ipv4::domain;
            tm_v4.sin_port = htons(this->get_port());

            if( inet_pton(tcp::ipv4::domain, this->get_server_ip().c_str(), &tm_v4.sin_addr) != 1 )
                return Status::err(domain_t::client, status::to_underlying(client_code_t::server_ipv4_not_valid));

            tm_connect = ::connect(this->get_socket(), reinterpret_cast<sockaddr*>(&tm_v4), sizeof(tm_v4));
        }

        // CONNECT
        if( tm_connect == ss_inv_connect )
        {
            this->m_status.change(_FLAG_ALREADY_CONNECT, _FLAG_ERR_CONNECT);

            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), this->get_server_ip() + " Ip Connect Error", GET_SOURCE));

            // ERR WINDOWS
            #if __OS_WINDOWS__
                DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Windows Error Code: " + std::to_string(::WSAGetLastError()) ,GET_SOURCE));
            #endif
            
            return Status::err(domain_t::client, status::to_underlying(client_code_t::connection_err));
        }

        // LOG
        if( this->get_flag().has(_FLAG_SOCKET_LOGGER) )
            this->get_logger().write(level_t::Info, this->get_server_ip() + " Connected");

        // STATUS VARIABLE FOR HANDSHAKE
        Status tm_handshake;

        // HANDSHAKE SEND VERIFY
        tm_handshake = Socket::handshake_send_verify(this->get_socket());
        if( !tm_handshake.is_ok() )
        {
            // LOG
            if( this->get_flag().has(_FLAG_SOCKET_LOGGER) )
                this->get_logger().write(level_t::Err, this->get_server_ip() + " Handshake Send Failed");

            return Status::err(domain_t::client, status::to_underlying(client_code_t::server_send_handshake_err));
        }

        // HANDSHAKE RECV VERIFY
        tm_handshake = Socket::handshake_recv_verify(this->get_socket());
        if( !tm_handshake.is_ok() )
        {
            // LOG
            if( this->get_flag().has(_FLAG_SOCKET_LOGGER) )
                this->get_logger().write(level_t::Err, this->get_server_ip() + " Handshake Receive Failed");

            return Status::err(domain_t::client, status::to_underlying(client_code_t::server_recv_handshake_err));
        }

        // ADD TO CONNECTS LIST
        {
            std::scoped_lock tm_lock(s_connects_mtx);

            s_connects.emplace(
                this->get_socket(),
                ConnectCtx{
                    this->get_port(),
                    this->get_ipv(),
                    this->get_server_ip()
                }
            );
        }

        // SET FLAG
        this->m_status.change(_FLAG_ERR_CONNECT, _FLAG_ALREADY_CONNECT);
        return Status::ok(domain_t::client, status::to_underlying(client_code_t::connected_and_handshake_verified));
    }

    /**
     * @brief Client Runner
     * 
     * İstemci işleme başlamadan önce bazı kontrolleri
     * yapar çünkü hatalı şekilde başlamasını istemeyiz.
     * Sonrasında bağlantı durumunu kontrol ederiz ve
     * eğer hata varsa bunu kayıtlarda belirtiriz.
     * Sunucuya bağlandıktan sonra ip adresini ve port
     * adresini kayıtlarda belirtiriz. Sonrasında socket
     * değerini saklarız. İstemci için işlemi başlatırız ve
     * bunu işlem havuzuna ekleriz. İşlem bittikten sonra
     * soketi kapatırız. Bağlantı sağlanan sunucular
     * listesinde istemcinin soket bilgisi hala varsa
     * listeden temizleriz ve sonlandırırız
     */
    void Client::client_runner() noexcept
    {
        // STATUS VARIABLE
        Status tm_status;

        // TRY TO CONNECT
        tm_status = this->do_connect();
        if( !tm_status.is_ok() && !tm_status.is_warn() )
        {
            // NOT RUNNING
            this->set_running(false);

            // LOG
            if( this->get_flag().has(_FLAG_SOCKET_LOGGER) )
                this->get_logger().write(level_t::Err, "Client Connect Error, Code: " + std::to_string(tm_status.get_code()));

            return;
        }

        // LOG
        if( this->get_flag().has(_FLAG_SOCKET_LOGGER) )
            this->get_logger().write(level_t::Info, "Client Connected To Server, Ip/Port: " + this->get_server_ip() + '/' + std::to_string(this->get_port()));

        // FIND SERVER CONNECTION INFO
        const socket_t tm_client_sock = this->get_socket();

        // THREAD POOL
        this->m_tpool.enqueue([this, tm_client_sock]{
            // RUN
            DEBUG_ONLY(this->get_logger().write(level_t::Info, "Client Handler Starting...", GET_SOURCE));

            if( this->m_handler )
                this->m_handler(*this);

            // SOCKET CLOSE
            const Status tm_close = this->close();
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, "Client Socket Close, Code: " + std::to_string(tm_close.get_code()), GET_SOURCE));

            // REMOVE SERVER CONNECTION DATA FROM LIST
            {
                std::scoped_lock tm_lock(s_connects_mtx);
                s_connects.erase(tm_client_sock);
            }

            // NOT RUNNING
            this->set_running(false);
        });
    }

    /**
     * @brief Get Connects
     * 
     * Oluşturulan tüm istemcilerin bağlanacakları ya da 
     * bağlandıkları sunucu ile ilgili bazı bilgilerin olduğu
     * tüm listeyi döndürür
     * 
     * @return const unordered_map<socket_t, ConnectCtx>&
     */
    const std::unordered_map<socket_t, ConnectCtx>& Client::get_connects() const noexcept
    {
        return s_connects;
    }

    /**
     * @brief Get Connect Count
     * 
     * Oluşturulan tüm istemcilerin toplam sayısını döndürür
     * 
     * @return size_t
     */
    std::size_t Client::get_client_count() const noexcept
    {
        std::scoped_lock tm_lock(s_connects_mtx);
        return s_connects.size();
    }

    /**
     * @brief Get Status Flag
     * 
     * Tutulan durum bayrağı nesnesini döndürür
     * 
     * @return const Flag&
     */
    const flag::Flag& Client::get_status_flag() const noexcept
    {
        return this->m_status;
    }

    /**
     * @brief Get Server Ip
     * 
     * Sunucu ip adresini korumalı şekilde döndürür
     * 
     * @return const string&
     */
    const std::string& Client::get_server_ip() const noexcept
    {
        return this->m_server_ip;
    }

    /**
     * @brief Set Server Ip
     * 
     * İstemcinin bağlanacağı sunucunun ip
     * adresini saklayan metini ayarlar
     * 
     * @param string& Ip Address
     * @return Status
     */
    Status Client::set_server_ip(const std::string& ar_ip) noexcept
    {
        // IPV4 CHECK
        switch( this->get_ipv() )
        {
            // IPV4
            case ipv_t::ipv4:
                if( !Socket::is_valid_ipv4(ar_ip) )
                {
                    // DEBUG LOG
                    DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Client Set Server Ipv4 Not Valid Error", GET_SOURCE));
                    return Status::err(domain_t::client, status::to_underlying(client_code_t::server_ipv4_not_valid));
                }
            break;

            // IPV6
            case ipv_t::ipv6:
                if( !Socket::is_valid_ipv6(ar_ip) )
                {
                    // DEBUG LOG
                    DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Client Set Server Ip Not Valid Error", GET_SOURCE));
                    return Status::err(domain_t::client, status::to_underlying(client_code_t::server_ipv6_not_valid));
                }
            break;

            // NONE
            case ipv_t::none:
            default:
                // DEBUG LOG
                DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Client Ip Type Not Supported, Only Ipv4/v6 Supporting", GET_SOURCE));
                return Status::err(domain_t::client, status::to_underlying(client_code_t::server_ip_type_not_valid));
        }

        // SET SERVER IP
        {
            std::scoped_lock tm_lock(this->m_mtx);
            this->m_server_ip = ar_ip;
        }

        // IS VALID IP ?
        if( ar_ip == this->m_server_ip )
        {
            this->m_status.change(_FLAG_ERR_SERVER_IP, _FLAG_ALREADY_SERVER_IP);
            return Status::ok(domain_t::client, status::to_underlying(client_code_t::server_ip_set));
        }

        // NOT VALID
        this->m_status.change(_FLAG_ALREADY_SERVER_IP, _FLAG_ERR_SERVER_IP);

        // DEBUG LOG
        DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Client Set Server Ip Error", GET_SOURCE));
        return Status::err(domain_t::client, status::to_underlying(client_code_t::server_not_ip_set));
    }

    /**
     * @brief Set Handler
     * 
     * İstemci ile sunucu arası veri iletişimi
     * olurken bu verinin işlenmesini sağlayacak
     * fonksiyonu ayarlar
     * 
     * @param conn_handler& Handler
     * @return Status
     */
    Status Client::set_handler(conn_handler& ar_handler) noexcept
    {
        // IS RUNNING
        if( this->is_running() )
            return Status::err(domain_t::client, status::to_underlying(client_code_t::stop_socket_before_connection_handler_set));

        std::scoped_lock tm_lock(this->m_mtx);
        this->m_handler = std::move(ar_handler);

        return Status::ok(domain_t::client, status::to_underlying(client_code_t::connection_handler_set));
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
    Status Client::create() noexcept
    {
        // HAS ALREADY
        if( Socket::is_valid_socket(this->get_socket()) )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Client Already Has Valid Socket", GET_SOURCE));

            // SET FLAG
            this->m_status.change(_FLAG_ERR_CREATE, _FLAG_ALREADY_CREATE);
            return Status::warn(domain_t::client, status::to_underlying(socket_code_t::socket_has_already));
        }

        // RESET FLAG
        this->m_status.change(_FLAG_ALREADY_CREATE, _FLAG_ERR_CREATE);

        // CREATE
        Status tm_status = Socket::create();
        if( !tm_status.is_ok() && !tm_status.is_warn() && !Socket::is_valid_socket(this->get_socket()) )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Client Create Socket Error, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
            return Status::err(domain_t::client, status::to_underlying(socket_code_t::socket_not_create));
        }

        // SET FLAG
        this->m_status.change(_FLAG_ERR_CREATE, _FLAG_ALREADY_CREATE);
        return Status::ok(domain_t::client, status::to_underlying(socket_code_t::socket_create));
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
    Status Client::close() noexcept
    {
        // CLOSE
        Status tm_status = Socket::close();
        return tm_status.is_ok() ?
            Status::ok(domain_t::client, tm_status.get_code()) :
            Status::err(domain_t::client, tm_status.get_code());
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
    Status Client::clear() noexcept
    {
        // CLEAR
        Status tm_status = Socket::clear();
        if( !tm_status.is_ok() && !tm_status.is_warn() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Client Clear Error, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
            return Status::err(domain_t::client, tm_status.get_code());
        }

        {
            std::scoped_lock tm_lock(this->m_mtx);

            // CLEAR FLAG
            this->m_status.clear();

            // CLEAR SERVER IP
            this->m_server_ip.clear();
        }

        return Status::ok(domain_t::client, status::to_underlying(client_code_t::client_clear));
    }

    /**
     * @brief Send
     * 
     * @param socket_t Socket
     * @param DataPacket& Data
     * 
     * @return Status
     */
    Status Client::send(
        const socket_t ar_sock,
        const DataPacket& ar_datapack
    ) noexcept
    {
        return Socket::send(ar_sock, ar_datapack);
    }

    /**
     * @brief Recv
     * 
     * @param socket_t Socket
     * @param DataPacket& Data
     * 
     * @return Status
     */
    Status Client::recv(
        const socket_t ar_sock,
        DataPacket& ar_datapack
    ) noexcept
    {
        return Socket::recv(ar_sock, ar_datapack);
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
    Status Client::init() noexcept
    {
        // ALREADY INITIALIZED
        if( this->m_status.has(_FLAG_ALREADY_INIT) )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Client Already Initialized", GET_SOURCE));
            return Status::err(domain_t::server, status::to_underlying(client_code_t::client_already_init));
        }

        // ALREADY CONNECTED
        if( this->m_status.has(_FLAG_ALREADY_CONNECT) )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Client Already Connected", GET_SOURCE));
            return Status::err(domain_t::server, status::to_underlying(client_code_t::client_already_connect));
        }

        // ERROR FLAG
        this->m_status.change(_FLAG_ALREADY_INIT, _FLAG_ERR_INIT);
        this->m_status.add(_FLAG_ERR_CREATE, _FLAG_ERR_CONNECT);

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
        return Status::ok(domain_t::client, status::to_underlying(client_code_t::client_init));
    }

    /**
     * @brief Run
     * 
     * İstemciyi çalıştırmak için gereklidir. İstemci
     * durdurulana dek çalışmaya devam eder ve sunucular ile
     * bağlantıyı sağlar. Hata oluşması ya da durdurma emri
     * sonrası çalışmayı sonlandırır
     * 
     * @return Status
     */
    Status Client::run() noexcept
    {
        // HAS ERROR
        if( Socket::has_error() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Run Has Error, Flag: " + std::to_string(this->m_status.get()), GET_SOURCE));
            return Status::err(domain_t::client, status::to_underlying(client_code_t::has_error));
        }

        // INITIALIZE IF NOT
        if( this->m_status.has(_FLAG_ERR_INIT) )
        {
            Status tm_status = this->init();
            if( !tm_status.is_ok() && !tm_status.is_warn() )
            {
                // DEBUG LOG
                DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Run Re-Initialize Error, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
                return Status::err(domain_t::client, status::to_underlying(client_code_t::client_reinit_err));
            }
        }

        // STATUS VARIABLE
        Status tm_status;

        // HAS ERROR
        if( this->has_error() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Run Has Critical Error, Flag: " + std::to_string(this->m_status.get()), GET_SOURCE));   
            return Status::err(domain_t::client, status::to_underlying(client_code_t::has_error));
        }

        // ALREADY RUNNING
        if( this->is_running() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Is Running Error, Flag: " + std::to_string(this->m_status.get()), GET_SOURCE));
            return Status::err(domain_t::client, status::to_underlying(client_code_t::client_already_running));
        }

        // NOT VALID SOCKET
        if( !Socket::is_valid_socket(this->get_socket()) )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Socket Is Not Valid, Flag: " + std::to_string(this->m_status.get()), GET_SOURCE));
            return Status::err(domain_t::client, status::to_underlying(socket_code_t::socket_not_valid));
        }

        // SET RUNNING
        tm_status = this->set_running(true);
        if( !tm_status.is_ok() && !this->is_running() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Set Running Error, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
            return Status::err(domain_t::client, status::to_underlying(socket_code_t::socket_no_run));
        }

        // CLIENT RUNNER
        try {
            this->m_worker = std::thread([this]{ this->client_runner(); });
            
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Client Worker Started To Run: " + std::to_string(this->m_status.get()), GET_SOURCE));
        } catch (...) {
            this->set_running(false);

            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Client Worker Running Error, Flag: " + std::to_string(this->m_status.get()), GET_SOURCE));
            return Status::err(domain_t::client, status::to_underlying(client_code_t::client_run_thread_fail));
        }

        // OK
        return Status::ok(domain_t::client, status::to_underlying(client_code_t::client_started_to_run));
    }

    /**
     * @brief Stop
     * 
     * Eğer istemci çalışma durumunu iptal olarak
     * ayarlayamassa hata durumunu döndürsün. 
     * Soketi kapatsın ve çalışma fonksiyonunu durdurmayı
     * sağlasın
     * 
     * @return Status
     */
    Status Client::stop() noexcept
    {
        // STATUS VARIABLE
        Status tm_status = this->set_running(false);

        // TRY THE STOP
        if( !tm_status.is_ok() && !this->is_running() )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Stop Error, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
            return Status::err(domain_t::client, tm_status.get_code());
        }

        // CLOSE SOCKET
        tm_status = this->close();
        if( !tm_status.is_ok() && Socket::is_valid_socket(this->get_socket()) )
        {
            // DEBUG LOG
            DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Close Error, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
            return Status::err(domain_t::client, tm_status.get_code());
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
        return Status::ok(domain_t::client, status::to_underlying(client_code_t::client_stopped));
    }

    /**
     * @brief Print
     * 
     * İstemci hakkında bilgileri çıktı verir.
     * Bağlı olduğu sunucu ip adresi, ip sürümü,
     * port numarası ve dahası hakkında bilgi verir
     */
    void Client::print() noexcept
    {
        this->get_logger().write("========== CLIENT ==========");
        this->get_logger().write(std::string("Build: ").append(Build::c_str()));
        this->get_logger().write(std::string("Name: ").append(this->get_policy().get_username()));
        this->get_logger().write(std::string("Base Flag: ").append(std::to_string(this->get_flag().get())));
        this->get_logger().write(std::string("Status Flag: ").append(std::to_string(this->get_status_flag().get())));
        this->get_logger().write(std::string("Is Running: ").append(this->is_running() ? "Yes" : "No"));
        this->get_logger().write(std::string("Has Error: ").append(this->has_error() ? "Yes" : "No"));
        this->get_logger().write(std::string("Port: ").append(std::to_string(this->get_port())));
        this->get_logger().write(std::string("Ip Version: ").append(this->get_ipv() == ipv_t::ipv6 ? "v6" : "v4"));
        this->get_logger().write(std::string("Server Ip: ").append(this->get_server_ip()));
        this->get_logger().write(std::string("Total Client: ").append(std::to_string(this->get_client_count())));
        this->get_logger().write("============================");
    }

    /**
     * @brief Crashed
     * 
     * İstenmeyen çökme durumunda ilk önce istemciyi
     * durdursun, sonrasında ise istemci listesi dolu ise
     * sırayla soketleri kapatıp listeyi temizlesin
     */
    void Client::crashed() noexcept
    {
        // DEBUG LOG
        DEBUG_ONLY(this->get_logger().write(level_t::Debug, this->get_policy().get_username(), "Client Crash, Flag: " + std::to_string(this->m_status.get()), GET_SOURCE));

        // STOP THIS
        this->stop();

        // CLEAR LIST
        {
            std::scoped_lock tm_lock(s_connects_mtx);

            while( !s_connects.empty() )
            {
                // CONNECTION ITERATOR
                auto tm_it = s_connects.begin();

                // CLOSE SOCKET
                Socket::close_socket(tm_it->first);

                // ERASE CONNECTION FROM LIST
                s_connects.erase(tm_it);
            }
        }

        // BASE CRASH
        Socket::crashed();
    }
}
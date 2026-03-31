// Abdulkadir U. - 2026/01/28
#pragma once

/**
 * Socket (Soket)
 * 
 * Bilgisayarlar arası iletişim için gerekli olan soket
 * bağlantısının temeli olan bu sınıf sayesinde
 * sonradan bir sunucu (server) ya da istemci (client)
 * sınıfı da oluşturabiliriz
 */

// Include
#include <dev/developer.hpp>

#include <kits/corekit.hpp>
#include <kits/toolkit.hpp>
#include <kits/hashkit.hpp>

#include <pool/threadpool.hpp>
#include <pool/cipherpool.hpp>

#include <socket/policy.hpp>
#include <socket/netpacket.hpp>

// Define
#if __OS_WINDOWS__
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <mswsock.h>
    #include <windows.h>
#elif __OS_POSIX__
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <netdb.h>
    #include <poll.h>
    #include <errno.h>
#else
    #error "[ERR] Operating System Not Supporting"
#endif

#include <cstdint>
#include <iostream>
#include <vector>
#include <array>
#include <atomic>
#include <mutex>

// Namespace
namespace netsocket
{
    // Type Definiton
    #if __OS_WINDOWS__
        using socket_t = SOCKET;
        using sockaddr_t = SOCKADDR;
    #elif __OS_POSIX__
        using socket_t = int;
        using sockaddr_t = sockaddr;
    #endif

    using socket_port_t = uint16_t;
    using socket_dom_t = int;
    using socket_conn_t = int;
    using socket_proto_t = int;
    using socket_send_t = int;
    using socket_recv_t = int;

    using wait_time_t = uint16_t;
    using buff_size_t = uint32_t;

    // Enum
    enum class ipv_t : uint8_t
    {
        none = 0,
        ipv4,
        ipv6
    };

    // Enum
    enum class socket_code_t : uint16_t
    {
        err = 0,
        socket_not_valid,
        socket_set_err_not_valid,
        port_not_valid,
        port_set_err_not_valid,
        ipv_not_valid,
        ipv_set_err_not_valid,
        name_not_valid,
        name_set_err_not_valid,
        socket_not_create,
        socket_not_close,
        socket_not_clear,
        target_socket_not_valid,
        not_enough_data_in_packet,
        packet_not_send,
        socket_hash_not_match,
        socket_not_recv_header,
        recv_socket_header_close,
        socket_not_recv,
        recv_socket_close,
        packet_no_data,
        socket_no_run,
        socket_no_stop,
        socket_can_be_close,
        packet_corrupt,
        recv_username_empty,
        recv_message_empty,
        value_under_min,
        value_over_max,
        fail_set_timeout,
        socket_not_set_running,
        socket_already_running,
        socket_no_init,
        socket_has_already,
        recv_length_not_expected,
        recv_socket_close_header,

        succ = 1000,
        socket_set,
        port_set,
        ipv_set,
        name_set,
        socket_create,
        socket_close,
        socket_clear,
        handshake_send_hash,
        socket_hash_match,
        socket_data_sent,
        socket_data_recv,
        set_timeout,
        socket_set_running,

        warn = 2000,
        same_value,

        info = 3000
    };

    // Namespace
    namespace tcp
    {
        // Namespace
        namespace ipv4
        {
            static inline constexpr socket_dom_t domain = AF_INET;
            static inline constexpr socket_conn_t type = SOCK_STREAM;
            static inline constexpr socket_proto_t protocol = IPPROTO_TCP;
        }

        // Namespace
        namespace ipv6
        {
            static inline constexpr socket_dom_t domain = AF_INET6;
            static inline constexpr socket_conn_t type = SOCK_STREAM;
            static inline constexpr socket_proto_t protocol = IPPROTO_TCP;
        }
    }

    // Struct
    struct DataPacket
    {
        std::string m_pwd;
        std::string m_name;
        std::string m_msg;
    };

    // Struct
    struct UserPacket
    {
        uint16_t m_try_passwd { 0 };
        uint16_t m_same_user_count { 0 };
        std::string m_username { "" };
    };

    // Struct
    struct SocketCtx
    {
        UserPacket m_user;
        std::string m_ip;
    };

    #if __OS_WINDOWS__
        static constexpr socket_t ss_inv_socket = INVALID_SOCKET;
        static constexpr socket_port_t ss_inv_port = 0;
        static constexpr int ss_inv_receive = SOCKET_ERROR;
        static constexpr int ss_inv_send = SOCKET_ERROR;
    #elif __OS_POSIX__
        static constexpr socket_t ss_inv_socket = -1;
        static constexpr socket_port_t ss_inv_port = 0;
        static constexpr int ss_inv_receive = -1;
        static constexpr int ss_inv_send = -1;
    #endif

    // Using Namespace
    using namespace core;
    using namespace pool;
    using namespace cipher;
    using namespace dev;
    using namespace tools;
    using namespace output;
    using namespace level;
    using namespace crash;

    using status::Status;
    using status::status_t;
    using status::domain_t;

    // Limit
    static constexpr socket_t _MIN_SOCKET = 1;

    static constexpr socket_port_t _MIN_PORT = 80;
    static constexpr socket_port_t _MAX_PORT = ((socket_port_t)~0) - 1;

    static constexpr wait_time_t _MIN_TIMEOUT = 1;  // second
    static constexpr wait_time_t _DEF_TIMEOUT = 5; // second
    static constexpr wait_time_t _MAX_TIMEOUT = 30; // second

    // Flag
    static constexpr flag::flag_t _FLAG_SOCKET_NULL = { 0 << 0 };
    static constexpr flag::flag_t _FLAG_SOCKET_LOGGER = { 1 << 0 };
    static constexpr flag::flag_t _FLAG_SOCKET_DEBUG = { 1 << 1 };
    static constexpr flag::flag_t _FLAG_SOCKET_IPV6 = { 1 << 2 };

    // Version Hash
    static constexpr uint8_t ss_hash_hex_size = 16;
    static constexpr uint32_t ss_hash_code = (
        netpacket::_SIZE_HEADER +
        netpacket::_SIZE_PASSWORD + netpacket::_SIZE_USERNAME + netpacket::_SIZE_MESSAGE +
        netpacket::_PACK_LEN_PASSWORD + netpacket::_PACK_LEN_PASSWORD + netpacket::_PACK_LEN_PASSWORD +
        netpacket::_PACK_START_LEN_PASSWORD +
        netpacket::_SIZE_OVER_SOCKET
    ) % (uint32_t)~0;

    static constexpr hash::vch::Vch<ss_hash_hex_size> ss_ver_hash("2026-03-31|server-client-fix|test|0530", ss_hash_code);

    // WSA SOCKET
    #if __OS_WINDOWS__
        static inline std::atomic<bool> s_wsa_init { false };
        static inline WSAData s_wsa_data;

        /**
         * @brief Run WSA Socket
         * 
         * Windows için soket işlemlerinde gerekli olan
         * wsa socket güvenli şekilde yapısını başlatıyoruz
         */
        static void run_wsa_socket() noexcept
        {
            // WINDOWS WSA SOCKET
            if( !s_wsa_init.load(std::memory_order_seq_cst) )
            {
                if( ::WSAStartup(MAKEWORD(2,2), &s_wsa_data) == 0 )
                    s_wsa_init.store(true, std::memory_order_seq_cst);
            }
        }
    #endif

    // INIT GLOBAL SOCKET
    #if __OS_POSIX__
        #define _SOCKET_INIT()
    #elif __OS_WINDOWS__
        #define _SOCKET_INIT() (run_wsa_socket())
    #else
        #error "[ERROR] Operating System Not Supporting!"
    #endif

    // Class
    class Socket : public virtual CrashHandler
    {
        public:
            // Function Define
            [[maybe_unused]] [[nodiscard]] static inline bool is_valid_socket(const socket_t ar_sock) noexcept;
            [[maybe_unused]] [[nodiscard]] static inline bool is_valid_port(const socket_port_t ar_port) noexcept;
            [[maybe_unused]] [[nodiscard]] static inline bool is_valid_ipv(const ipv_t ar_ipv) noexcept;

            [[maybe_unused]] [[nodiscard]] static bool is_valid_ip(const std::string& ar_ipaddr) noexcept;
            [[maybe_unused]] [[nodiscard]] static bool is_valid_ipv4(const std::string& ar_ipaddr) noexcept;
            [[maybe_unused]] [[nodiscard]] static bool is_valid_ipv6(const std::string& ar_ipaddr) noexcept;

            [[maybe_unused]] static bool shutdown_socket(const socket_t ar_sock) noexcept;
            [[maybe_unused]] static bool close_socket(const socket_t ar_sock) noexcept;
            [[maybe_unused]] [[nodiscard]] static std::string get_ip(const socket_t ar_sock) noexcept;

            [[maybe_unused]] [[nodiscard]] static Status handshake_send_verify(const socket_t ar_sock) noexcept;
            [[maybe_unused]] [[nodiscard]] static Status handshake_recv_verify(const socket_t ar_sock, const bool ar_close = true) noexcept;

        private:
            algorithm::Algorithm m_cipher;
            log::Logger<file::FileOut> m_logger;
            policy::AccessPolicy m_policy;
            flag::Flag m_flag;

            socket_t m_sock { ss_inv_socket };
            ipv_t m_ipv { ipv_t::none };
            socket_port_t m_port { ss_inv_port };

            std::atomic<bool> m_running { false };

            std::atomic<wait_time_t> m_timeout { _DEF_TIMEOUT };

            mutable std::mutex m_mtx;

            inline static std::atomic<uint32_t> s_total_sock { 0 };

        private:
            inline static void inc_total_socket() noexcept { ++s_total_sock; };
            inline static void dec_total_socket() noexcept { if( s_total_sock ) --s_total_sock; };

        public:
            explicit Socket(
                algorithm::Algorithm& ar_cipher,
                const std::string& ar_filepath,
                const std::string& ar_name,
                const std::string& ar_password,
                const socket_port_t ar_port,
                const ipv_t ar_ipv,
                const flag::flag_t ar_flag = _FLAG_SOCKET_NULL
            );

            virtual ~Socket();

            virtual inline bool is_running() const noexcept;

            virtual inline bool has_error() const noexcept;
            virtual inline bool has_socket() const noexcept;
            virtual inline bool has_port() const noexcept;
            virtual inline bool has_ipv() const noexcept;
            virtual inline bool has_cipher() const noexcept;

            static inline uint32_t get_total_socket() noexcept;

            virtual inline socket_t get_socket() const noexcept;
            virtual inline socket_port_t get_port() const noexcept;
            virtual inline ipv_t get_ipv() const noexcept;
            virtual inline wait_time_t get_timeout() const noexcept;

            virtual Status run() noexcept { return Status::warn(domain_t::socket, status::to_underlying(socket_code_t::socket_no_run)); }
            virtual Status stop() noexcept { return Status::warn(domain_t::socket, status::to_underlying(socket_code_t::socket_no_stop)); }

            virtual void print() noexcept;

            virtual inline algorithm::Algorithm& get_cipher() noexcept;
            virtual inline log::Logger<output::file::FileOut>& get_logger() noexcept;
            virtual inline policy::AccessPolicy& get_policy() noexcept;
            virtual inline flag::Flag& get_flag() noexcept;

            virtual inline const algorithm::Algorithm& get_cipher() const noexcept;
            virtual inline const log::Logger<output::file::FileOut>& get_logger() const noexcept;
            virtual inline const policy::AccessPolicy& get_policy() const noexcept;
            virtual inline const flag::Flag& get_flag() const noexcept;

            virtual Status set_socket(const socket_t ar_sock) noexcept;
            virtual Status set_port(const socket_port_t ar_port) noexcept;
            virtual Status set_ipv(const ipv_t ar_ipv) noexcept;
            virtual Status set_timeout(const wait_time_t ar_timeout = _DEF_TIMEOUT) noexcept;

            virtual Status create() noexcept;
            virtual Status close() noexcept;
            virtual Status clear() noexcept;

            virtual Status send(const socket_t ar_sock, const DataPacket& ar_datapack) noexcept;
            virtual Status recv(const socket_t ar_sock, DataPacket& ar_datapack) noexcept;

        protected:
            virtual Status init() noexcept { return Status::warn(domain_t::socket, status::to_underlying(socket_code_t::socket_no_init)); }
            virtual Status set_running(const bool ar_running = true) noexcept;

            virtual void crashed() noexcept override;
    };

    /**
     * @brief Socket
     * 
     * Soket oluşturulurken bir şifreleme yapısı,
     * kayıt yapısı, sokete verilecek isim, port numarası,
     * ip sürüm türü ve bayrak değeri olur. Bunları alaraktan
     * gerekli atamaları yaparak soketi oluşturur. Eğer
     * Windows kullanılıyor ise, ek bir kontrol sağlar
     * 
     * @param Algorithm& Cipher
     * @param string& Filepath
     * @param string& Name
     * @param string& Password
     * @param socket_port_t Port
     * @param ipv_t Ip Version
     * @param flag_t Flag
     */
    Socket::Socket(
        algorithm::Algorithm& ar_cipher,
        const std::string& ar_filepath,
        const std::string& ar_name,
        const std::string& ar_password,
        const socket_port_t ar_port,
        const ipv_t ar_ipv,
        const flag::flag_t ar_flag
    )
    :   m_cipher(ar_cipher),
        m_logger(ar_filepath),
        m_policy(ar_name, ar_password),
        m_flag(ar_flag)
    {
        // PORT & IP TYPE
        this->set_port(ar_port);
        this->set_ipv(ar_ipv);

        // GLOBAL SOCKET INIT
        _SOCKET_INIT();

        // INCREASE TOTAL SOCKET COUNTER
        inc_total_socket();
    }

    /**
     * @brief ~Socket
     * 
     * Sınıfı sonlandıracağı zaman ilk kapatır, sonra
     * temizler ve en sonda toplam soket sayısında eksiltme
     * yaparak sonlandırır
     */
    Socket::~Socket()
    {
        // CLOSE AND CLEAR SOCKET
        this->close();
        this->clear();

        // DECREASE TOTAL SOCKET COUNTER
        dec_total_socket();

        // WINDOWS WSA SUPPORT
        #if __OS_WINDOWS__
            if( s_total_sock < 1 && CrashHandler::has_signal() )
            {
                ::WSACleanup();
                s_wsa_init.store(false, std::memory_order_seq_cst);
            }
        #endif
    }

    /**
     * @brief Is Running
     * 
     * Soketin işleme alınıp çalışır durumda olup
     * olmadığı bilgisini döndürecek
     * 
     * @return bool
     */
    bool Socket::is_running() const noexcept
    {
        return this->m_running.load(std::memory_order_seq_cst) && !CrashHandler::has_signal();
    }

    /**
     * @brief Has Error
     * 
     * Hata olup olmadığını döndürür
     * 
     * @return bool
     */
    bool Socket::has_error() const noexcept
    {
        return !this->has_socket() || !this->has_port() || !this->has_ipv() || !this->has_cipher();
    }

    /**
     * @brief Has Socket
     * 
     * Geçerli bir soket olup olmadığı bilgisini döndürür
     * 
     * @return bool
     */
    bool Socket::has_socket() const noexcept
    {
        return Socket::is_valid_socket(this->m_sock);
    }

    /**
     * @brief Has Port
     * 
     * Geçerli bir port numarası olup olmadığını döndürür
     * 
     * @return bool
     */
    bool Socket::has_port() const noexcept
    {
        return Socket::is_valid_port(this->m_port);
    }

    /**
     * @brief Has Ipv
     * 
     * Geçerli bir ip sürümü olup olmadığını döndürür
     * 
     * @return bool
     */
    bool Socket::has_ipv() const noexcept
    {
        return Socket::is_valid_ipv(this->m_ipv);
    }

    /**
     * @brief Has Cipher
     * 
     * Geçerli bir şifreleyici algoritma olup olmamasını döndürür
     * 
     * @return bool
     */
    bool Socket::has_cipher() const noexcept
    {
        return !this->m_cipher.has_error();
    }

    /**
     * @brief Get Total Socket
     * 
     * Oluşturulmuş toplam soket sayısını döndürür
     * 
     * @return uint32_t
     */
    uint32_t Socket::get_total_socket() noexcept
    {
        return s_total_sock.load(std::memory_order_seq_cst);
    }

    /**
     * @brief Get Socket
     * 
     * Soket değerini döndürür
     * 
     * @return socket_t
     */
    socket_t Socket::get_socket() const noexcept
    {
        return this->m_sock;
    }

    /**
     * @brief Get Port
     * 
     * Port numarasını döndürür
     * 
     * @return socket_port_t
     */
    socket_port_t Socket::get_port() const noexcept
    {
        return this->m_port;
    }

    /**
     * @brief Get Ipv
     * 
     * IP sürüm türünü döndürür
     * 
     * @return ipv_t
     */
    ipv_t Socket::get_ipv() const noexcept
    {
        return this->m_ipv;
    }

    /**
     * @brief Get Timeout
     * 
     * Soket için ayarlanan bekleme süresini döndürür
     * 
     * @return wait_time_t
     */
    wait_time_t Socket::get_timeout() const noexcept
    {
        return this->m_timeout.load(std::memory_order_acquire);
    }

    /**
     * @brief Print
     * 
     * Soket ile ilgili bilgileri kullanarak çıktı verir
     * 
     * @return print
     */
    void Socket::print() noexcept
    {
        this->m_logger.print();
    }

    /**
     * @brief Get Cipher
     * 
     * Şifreleme yapısının adresini referans olarak döndürür bu
     * sayede performans ve güvenlik kaygısı olmadan işleme devam
     * edilebilir
     * 
     * @return Algorithm&
     */
    algorithm::Algorithm& Socket::get_cipher() noexcept
    {
        return this->m_cipher;
    }

    /**
     * @brief Get Cipher
     * 
     * Şifreleme yapısının adresini korumalı referans olarak döndürür,
     * bu sayede güvenlik kaygısı olmadan işleme devam edilebilir
     * 
     * @return const Algorithm&
     */
    const algorithm::Algorithm& Socket::get_cipher() const noexcept
    {
        return this->m_cipher;
    }

    /**
     * @brief Get Logger
     * 
     * Kayıt yapısının adresini referans olarak döndürür bu
     * sayede performans ve güvenlik kaygısı olmadan işleme devam
     * edilebilir
     * 
     * @return Logger&
     */
    log::Logger<output::file::FileOut>& Socket::get_logger() noexcept
    {
        return this->m_logger;
    }

    /**
     * @brief Get Logger
     * 
     * Kayıt yapısının adresini korumalı referans olarak döndürür,
     * bu sayede güvenlik kaygısı olmadan işleme devam edilebilir
     * 
     * @return const Logger&
     */
    const log::Logger<output::file::FileOut>& Socket::get_logger() const noexcept
    {
        return this->m_logger;
    }

    /**
     * @brief Get Policy
     * 
     * Policy (Politika) yapısının adresini korumalı referans olarak döndürür,
     * bu sayede güvenlik kaygısı olmadan işleme devam edilebilir
     * 
     * @param AccessPolicy&
     */
    policy::AccessPolicy& Socket::get_policy() noexcept
    {
        return this->m_policy;
    }

    /**
     * @brief Get Policy
     * 
     * Policy (Politika) yapısının adresini referans olarak döndürür,
     * bu sayede performans ve güvenlik kaygısı olmadan işleme devam
     * edilebilir
     * 
     * @param AccessPolicy&
     */
    const policy::AccessPolicy& Socket::get_policy() const noexcept
    {
        return this->m_policy;
    }

    /**
     * @brief Get Flag
     * 
     * Flag (bayrak) yapısının adresini referans olarak döndürür,
     * bu sayede performans ve güvenlik kaygısı olmadan işleme devam
     * edilebilir
     * 
     * @return flag_t
     */
    flag::Flag& Socket::get_flag() noexcept
    {
        return this->m_flag;
    }

    /**
     * @brief Get Flag
     * 
     * Flag (bayrak) yapısının adresini korumalı referans olarak döndürür,
     * bu sayede güvenlik kaygısı olmadan işleme devam edilebilir
     * 
     * @return flag_t
     */
    const flag::Flag& Socket::get_flag() const noexcept
    {
        return this->m_flag;
    }

    /**
     * @brief Set Socket
     * 
     * Verilen soket numarasını belirli kontrollerden geçirerek
     * soket numarasını ayarlar. Hata veya başarı sonucunda
     * durum döndürür
     * 
     * @param socket_t Socket
     * @return Status
     */
    Status Socket::set_socket(
        const socket_t ar_sock
    ) noexcept
    {
        if( !Socket::is_valid_socket(ar_sock) )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::socket_not_valid));

        std::scoped_lock tm_lock(this->m_mtx);
        this->m_sock = ar_sock;

        return Socket::is_valid_socket(this->m_sock) ?
            Status::ok(domain_t::socket, status::to_underlying(socket_code_t::socket_set))
            : Status::err(domain_t::socket, status::to_underlying(socket_code_t::socket_set_err_not_valid));
    }

    /**
     * @brief Set Port
     * 
     * Verilen port numarasını belirli kontrollerden geçirerek
     * port numarasını ayarlar. Hata veya başarı sonucunda durum döndürür
     * 
     * @param socket_port_t Port
     * @return Status
     */
    Status Socket::set_port(
        const socket_port_t ar_port
    ) noexcept
    {
        if( !Socket::is_valid_port(ar_port) )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::port_not_valid));

        std::scoped_lock tm_lock(this->m_mtx);
        this->m_port = ar_port;

        return Socket::is_valid_port(this->m_port) ?
            Status::ok(domain_t::socket, status::to_underlying(socket_code_t::port_set))
            : Status::err(domain_t::socket, status::to_underlying(socket_code_t::port_set_err_not_valid));
    }

    /**
     * @brief Set Ipv
     * 
     * Verilen ip sürüm türünü belirli kontrollerden geçirerek ayarlar.
     * Hata veya başarı sonucunda durum döndürür
     * 
     * @param ipv_t Ipv
     * @return Status
     */
    Status Socket::set_ipv(
        const ipv_t ar_ipv
    ) noexcept
    {
        if( !Socket::is_valid_ipv(ar_ipv) )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::ipv_not_valid));

        std::scoped_lock tm_lock(this->m_mtx);
        this->m_ipv = ar_ipv;

        return Socket::is_valid_ipv(this->m_ipv) ?
            Status::ok(domain_t::socket, status::to_underlying(socket_code_t::ipv_set))
            : Status::err(domain_t::socket, status::to_underlying(socket_code_t::ipv_set_err_not_valid));
    }

    /**
     * @brief Set Timeout
     * 
     * Soket sürekli veriyi beklemek zorunda değil çünkü bu
     * hem işlem yükünü arttırır hem de boşuna beklemeye
     * neden olabilir. Bekleme süresi ile bunun belirli
     * bir süre aşımı sonrasında sonlanmasını sağlayabiliriz
     * 
     * @param wait_time_t Timeout
     * @return Status
     */
    Status Socket::set_timeout(const wait_time_t ar_timeout) noexcept
    {
        if( ar_timeout < _MIN_TIMEOUT )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::value_under_min));
        else if( ar_timeout > _MAX_TIMEOUT )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::value_over_max));
        else if( ar_timeout == this->m_timeout.load(std::memory_order_seq_cst) )
            return Status::warn(domain_t::socket, status::to_underlying(socket_code_t::same_value));

        this->m_timeout.store(ar_timeout, std::memory_order_seq_cst);
        return ar_timeout == this->m_timeout.load(std::memory_order_seq_cst) ?
            Status::ok(domain_t::socket, status::to_underlying(socket_code_t::set_timeout)) :
            Status::err(domain_t::socket, status::to_underlying(socket_code_t::fail_set_timeout));
    }

    /**
     * @brief Set Running
     * 
     * Soketin çalışıp çalışmadığı bilgisini tutacak olan
     * değişkeni ayarlamayı sağlar
     * 
     * @param bool Running
     * @return Status
     */
    Status Socket::set_running(const bool ar_running) noexcept
    {
        // SAME VALUE
        if( ar_running == this->m_running.load(std::memory_order_seq_cst) )
            return Status::ok(domain_t::socket, status::to_underlying(socket_code_t::same_value));
        
        // SET
        this->m_running.store(ar_running, std::memory_order_seq_cst);
        return ar_running == this->m_running.load(std::memory_order_seq_cst) ?
            Status::ok(domain_t::socket, status::to_underlying(socket_code_t::socket_set_running)) :
            Status::err(domain_t::socket, status::to_underlying(socket_code_t::socket_not_set_running));
    }

    /**
     * @brief Create
     * 
     * Ayarlanmış soket verilerini kullanarak soket bağlantısını
     * başlatır. Sonucu durum ile döndürür
     * 
     * @param Status
     */
    Status Socket::create() noexcept
    {
        if( Socket::is_valid_socket(this->m_sock) )
            return Status::warn(domain_t::socket, status::to_underlying(socket_code_t::socket_has_already));

        socket_dom_t tm_sock_domain;
        socket_conn_t tm_sock_type;
        socket_proto_t tm_sock_proto;

        switch(this->m_ipv)
        {
            case ipv_t::ipv4:
                tm_sock_domain = tcp::ipv4::domain;
                tm_sock_type   = tcp::ipv4::type;
                tm_sock_proto  = tcp::ipv4::protocol;
                break;

            case ipv_t::ipv6:
            default:
                tm_sock_domain = tcp::ipv6::domain;
                tm_sock_type   = tcp::ipv6::type;
                tm_sock_proto  = tcp::ipv6::protocol;                
                break;
        }

        socket_t tm_sock = ::socket(tm_sock_domain, tm_sock_type, tm_sock_proto);
        if( !Socket::is_valid_socket(tm_sock) )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::socket_not_valid));

        this->set_socket(tm_sock);

        {
            std::scoped_lock tm_lock(this->m_mtx);

            if( this->m_ipv == ipv_t::ipv6 )
            {
                #if __OS_POSIX__
                    int tm_off = 0;
                    ::setsockopt(this->m_sock,
                        IPPROTO_IPV6,
                        IPV6_V6ONLY,
                        &tm_off,
                        sizeof(tm_off)
                    );
                #elif __OS_WINDOWS__
                    int tm_off = 0;
                    ::setsockopt(this->m_sock,
                        IPPROTO_IPV6,
                        IPV6_V6ONLY,
                        reinterpret_cast<const char*>(&tm_off),
                        sizeof(tm_off)
                    );
                #endif
            }
        }

        return Socket::is_valid_socket(this->m_sock) ?
            Status::ok(domain_t::socket, status::to_underlying(socket_code_t::socket_create))
            : Status::err(domain_t::socket, status::to_underlying(socket_code_t::socket_not_create));
    }

    /**
     * @brief Close
     * 
     * Soket bağlantısını kapatmayı sağlar. Geçerli kontrol sonrası soketi kapatır.
     * Durum kodu ile de işlem sonucunu döndürür
     * 
     * @return Status
     */
    Status Socket::close() noexcept
    {
        if( !Socket::is_valid_socket(this->m_sock) )
            return Status::warn(domain_t::socket, status::to_underlying(socket_code_t::socket_not_valid));
        else if( !Socket::close_socket(this->m_sock) )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::socket_not_close));

        socket_t tm_socket = this->m_sock;

        {
            std::scoped_lock tm_lock(this->m_mtx);
            this->m_sock = ss_inv_socket;
        }

        // FREEBSD FIX
        #if __OS_POSIX__
            Socket::shutdown_socket(tm_socket);
	#elif __OS_WINDOWS__
	    (void)tm_socket;
        #endif

        return Status::ok(domain_t::socket, status::to_underlying(socket_code_t::socket_close));
    }

    /**
     * @brief Clear
     * 
     * Soket bağlantısını her ihtimale karşı kapatır. Kapatma zaten yapılmışsa
     * ya da daha yeni kapatmışsa bilgileri temizler aksi halde soketin
     * kapatılmadığına dair ya da başka bir hatayı temsil eden durum döndürür
     * 
     * @return Status
     */
    Status Socket::clear() noexcept
    {
        if( Socket::is_valid_socket(this->m_sock) )
        {
            Status tm_close_status = this->close();
            if( !tm_close_status.is_ok() && !tm_close_status.is_warn() )
                return Status::err(domain_t::socket, tm_close_status.get_code());
        }

        {
            std::scoped_lock tm_lock(this->m_mtx);

            this->m_sock = ss_inv_socket;
            this->m_port = ss_inv_port;
            this->m_flag.clear();
        }

        return Status::ok(domain_t::socket, status::to_underlying(socket_code_t::socket_clear));
    }

    /**
     * @brief Send
     * 
     * Oluşturduğumuz soket ile veri göndermek istenilen soketler kontrol edilir.
     * Veri aktif olan sürüme göre ayarlanır. Sonrasında belirtilen socket
     * numarasına iletilir
     * 
     * @param socket_t Target Socket
     * @param DataPacket& Data
     * 
     * @return Status
     */
    Status Socket::send(
        const socket_t ar_target_sock,
        const DataPacket& ar_datapack
    ) noexcept
    {
        // NOT VALID SOCKET
        if( !Socket::is_valid_socket(ar_target_sock) )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::target_socket_not_valid));

        // IS NAME OR MESSAGE EMPTY?
        if( ar_datapack.m_name.empty() || ar_datapack.m_msg.empty() )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::not_enough_data_in_packet));

        // ENCRYPT THE ALL DATA
        auto tm_pwd = ar_datapack.m_pwd;
        auto tm_name = ar_datapack.m_name;
        auto tm_msg = ar_datapack.m_msg;

        this->get_cipher().encrypt(tm_pwd);
        this->get_cipher().encrypt(tm_name);
        this->get_cipher().encrypt(tm_msg);

        // NET PACKET BUFFER
        netpacket::NetPacket tm_netpack(tm_pwd, tm_name, tm_msg);
        const auto& tm_buffer = tm_netpack.get();

        // TOTAL SENT DATA SIZE VARIABLE
        uint32_t tm_total_sent = 0;
        const uint32_t tm_total_size = tm_buffer.size();

        // SEND
        while( tm_total_sent < tm_total_size ) {
            int tm_sent = ::send(ar_target_sock,
                reinterpret_cast<const char*>(tm_buffer.data()) + tm_total_sent,
                static_cast<int>(tm_total_size - tm_total_sent),
                0
            );

            // NOT SENT OR SOCKET CLOSED
            if( tm_sent < 0 )
                return Status::err(domain_t::socket, status::to_underlying(socket_code_t::packet_not_send));
            else if ( tm_sent == 0 )
                return Status::err(domain_t::socket, status::to_underlying(socket_code_t::socket_can_be_close));

            // ADD SENT DATA SIZE TO VARIABLE UNTIL SENT ENDS UP
            tm_total_sent += tm_sent;
        }

        // LOGGER
        DEBUG_ONLY(
            const std::string tm_ip = get_ip(ar_target_sock);
            this->m_logger.write(level_t::Debug, this->get_policy().get_username(), tm_ip + " Sent", GET_SOURCE)
        );

        return Status::ok(domain_t::socket, status::to_underlying(socket_code_t::socket_data_sent));
    }

    /**
     * @brief Receive
     * 
     * Oluşturduğumuz soket ile veri alınmak istenilen soketler kontrol edilir.
     * Sorun bulunmaması dahilinde veri belirtilen soketten alınır. Sürüm kontrolü
     * sonrasında hata bulunursa hata durumu döndürülür aksi halde başarı döndürülür
     * 
     * @param socket_t Target Socket
     * @param DataPacket& Data
     */
    Status Socket::recv(
        const socket_t ar_target_sock,
        DataPacket& ar_datapack
    ) noexcept
    {
        // NOT VALID SOCKET
        if( !Socket::is_valid_socket(ar_target_sock) )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::target_socket_not_valid));

        // HEADER DATA ARRAY
        constexpr size_t size_header = netpacket::_SIZE_HEADER;
        char tm_header[size_header] {};

        // RECEIVE NEXT DATA LENGTH
        // RECEIVE THE REAL DATA
        int tm_total_header = 0;
        const int tm_expected_header = static_cast<int>(size_header);
        while( tm_total_header < tm_expected_header )
        {
            int tm_recv = ::recv(ar_target_sock, tm_header + tm_total_header, tm_expected_header - tm_total_header, 0);

            if( tm_recv < 0 )
                return Status::err(domain_t::socket, status::to_underlying(socket_code_t::socket_not_recv_header));
            else if( tm_recv == 0 )
                return Status::err(domain_t::socket, status::to_underlying(socket_code_t::recv_socket_close_header));

            tm_total_header += tm_recv;
        }

        // LENGTH VARIABLES
        uint16_t tm_len_pwd = 0;
        uint16_t tm_len_name = 0;
        uint16_t tm_len_msg = 0;

        // READ NEXT DATA LENGTH
        if( std::sscanf(tm_header, "%03hu%03hu%04hu", &tm_len_pwd, &tm_len_name, &tm_len_msg) != 3 )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::packet_corrupt));

        // DATA LENGTH IS VALID?
        const uint32_t tm_payload_len = tm_len_pwd + tm_len_name + tm_len_msg;
        if( tm_payload_len == 0 )
            return Status::warn(domain_t::socket, status::to_underlying(socket_code_t::packet_no_data));

        // VECTOR FOR DATA
        std::vector<char> tm_payload(tm_payload_len);

        // RECEIVE THE REAL DATA
        int tm_total_recv = 0;
        const int tm_expected_len = static_cast<int>(tm_payload_len);
        while( tm_total_recv < tm_expected_len )
        {
            int tm_recv = ::recv(ar_target_sock, tm_payload.data() + tm_total_recv, tm_expected_len - tm_total_recv, 0);

            if( tm_recv < 0 )
                return Status::err(domain_t::socket, status::to_underlying(socket_code_t::socket_not_recv));
            else if( tm_recv == 0 )
                return Status::err(domain_t::socket, status::to_underlying(socket_code_t::recv_socket_close));

            tm_total_recv += tm_recv;
        }

        // OFFSET
        uint32_t tm_offset = 0;

        // PASSWORD LENGTH
        std::string tm_pwd(tm_payload.data() + tm_offset, tm_len_pwd);
        tm_offset += tm_len_pwd;

        // NAME LENGTH
        std::string tm_name(tm_payload.data() + tm_offset, tm_len_name);
        tm_offset += tm_len_name;

        // MESSAGE LENGTH
        std::string tm_msg(tm_payload.data() + tm_offset, tm_len_msg);
        tm_offset += tm_len_msg;

        // DECRYPT ALL
        this->get_cipher().decrypt(tm_pwd);
        this->get_cipher().decrypt(tm_name);
        this->get_cipher().decrypt(tm_msg);

        // ASSIGN TO DATA PACKET
        ar_datapack.m_pwd = tm_pwd;
        ar_datapack.m_name = tm_name;
        ar_datapack.m_msg = tm_msg;

        // IS NAME OR MESSAGE EMPTY?
        if( ar_datapack.m_name.empty() )
            return Status::warn(domain_t::socket, status::to_underlying(socket_code_t::recv_username_empty));
        else if( ar_datapack.m_msg.empty() )
            return Status::warn(domain_t::socket, status::to_underlying(socket_code_t::recv_message_empty));

        // LOGGER
        DEBUG_ONLY(
            const std::string tm_ip = get_ip(ar_target_sock);
            this->m_logger.write(level_t::Debug, this->get_policy().get_username(), tm_ip + " Receive", GET_SOURCE)
        );

        return Status::ok(domain_t::socket, status::to_underlying(socket_code_t::socket_data_recv));
    }

    /**
     * @brief Crashed
     * 
     * Olası bir program hatası ya da ani kesinti durumunda
     * yapılması gerekenleri çalıştırarak sistem sızıntılarını
     * önlemeye çalışır
     */
    void Socket::crashed() noexcept
    {
        // LOGGER
        if( this->m_flag & _FLAG_SOCKET_LOGGER ) {
            const std::string tm_msg = "Crash Code: " + std::to_string(CrashHandler::get_signal());
            this->m_logger.write(level_t::Err, tm_msg, GET_SOURCE);
        }

        // CLOSE AND CLEAR
        this->close();
        this->clear();

        // WSA CLEANUP FOR WINDOWS
        #if __OS_WINDOWS__
            if( s_total_sock < 1 && s_wsa_init.load(std::memory_order_seq_cst) )
                ::WSACleanup();
        #endif
    }

    /**
     * @brief Is Valid Socket
     * 
     * Verilen socket numarasının geçerli olup olmadığını
     * kontrol edecek. Geçersiz soket numarası değilse ve
     * belirlenen en az limitinden yüksek numara ise socket
     * geçerlidir
     * 
     * @param socket_t Socket
     * @return bool
     */
    [[maybe_unused]] [[nodiscard]]
    bool Socket::is_valid_socket(
        const socket_t ar_sock
    ) noexcept
    {
        return ar_sock != ss_inv_socket && ar_sock >= _MIN_SOCKET;
    }

    /**
     * @brief Is Valid Port
     * 
     * Verilen port numarasının geçerli olup olmadığını
     * kontrol edecek. Geçersiz port numarası değilse ve
     * belirlenen en az limitinden yüksek numara ise port
     * geçerlidir
     * 
     * @param socket_port_t Port
     * @return bool
     */
    [[maybe_unused]] [[nodiscard]]
    bool Socket::is_valid_port(
        const socket_port_t ar_port
    ) noexcept
    {
        return ar_port != ss_inv_port && ar_port >= _MIN_PORT && ar_port <= _MAX_PORT;
    }

    /**
     * @brief Is Valid Ipv
     * 
     * Verilen ip türünün numaralandırılmış değerini alır.
     * Geçerli olanlardan herhangi birisi ise başarı, değilse
     * hata döndürür
     * 
     * @note Şuan olan ip türlerinin hepsi geçerli
     * 
     * @param ipv_t Ip Type
     * @return bool
     */
    [[maybe_unused]] [[nodiscard]]
    bool Socket::is_valid_ipv(const ipv_t ar_ipv) noexcept
    {
        switch(ar_ipv)
        {
            case ipv_t::ipv4:
            case ipv_t::ipv6:
                return true;
            case ipv_t::none:
            default:
                return false;
        }
    }

    /**
     * @brief Is Valid IP
     * 
     * Verilen ip adresinin geçerli olup olmadığını
     * kontrol ederek yanıt döndürecek
     * 
     * @param string& IP
     * @return bool
     */
    [[maybe_unused]] [[nodiscard]]
    bool Socket::is_valid_ip(const std::string& ar_ipaddr) noexcept
    {
        if( ar_ipaddr.empty() )
            return false;

        _SOCKET_INIT();

        addrinfo tm_addrinfo {};
        tm_addrinfo.ai_family = AF_UNSPEC;
        tm_addrinfo.ai_flags = AI_NUMERICHOST;
        tm_addrinfo.ai_socktype = SOCK_STREAM;

        addrinfo* tm_res = nullptr;
        int tm_ret = ::getaddrinfo(ar_ipaddr.c_str(), nullptr, &tm_addrinfo, &tm_res);

        if( tm_res )
            ::freeaddrinfo(tm_res);

        return tm_ret == 0;
    }

    /**
     * @brief Is Valid IPv4
     * 
     * Verilen ipv4 adresinin geçerli olup olmadığını
     * kontrol ederek yanıt döndürecek
     * 
     * @param string& IP
     * @return bool
     */
    [[maybe_unused]] [[nodiscard]]
    bool Socket::is_valid_ipv4(const std::string& ar_ipaddr) noexcept
    {
        if( ar_ipaddr.empty() )
            return false;

        _SOCKET_INIT();

        addrinfo tm_addrinfo {};
        tm_addrinfo.ai_family = tcp::ipv4::domain;
        tm_addrinfo.ai_flags  = AI_NUMERICHOST;

        addrinfo* tm_res = nullptr;
        int tm_ret = ::getaddrinfo(ar_ipaddr.c_str(), nullptr, &tm_addrinfo, &tm_res);

        if( tm_res )
            ::freeaddrinfo(tm_res);

        return tm_ret == 0;
    }

    /**
     * @brief Is Valid IPv6
     * 
     * Verilen ipv6 adresinin geçerli olup olmadığını
     * kontrol ederek yanıt döndürecek
     * 
     * @param string& IP
     * @return bool
     */
    [[maybe_unused]] [[nodiscard]]
    bool Socket::is_valid_ipv6(const std::string& ar_ipaddr) noexcept
    {
        if( ar_ipaddr.empty() )
            return false;

        _SOCKET_INIT();

        addrinfo tm_addrinfo {};
        tm_addrinfo.ai_family = tcp::ipv6::domain;
        tm_addrinfo.ai_flags = AI_NUMERICHOST;

        addrinfo* tm_res = nullptr;
        int tm_ret = ::getaddrinfo(ar_ipaddr.c_str(), nullptr, &tm_addrinfo, &tm_res);

        if( tm_res )
            ::freeaddrinfo(tm_res);

        return tm_ret == 0;
    }

    /**
     * @brief Shutdown Socket
     * 
     * İşletim sistemi türüne göre farklı olan soket sonlandırma
     * işlemini yapar. Geçersiz soket kontrolü sonrası
     * soketi sonlandırır
     * 
     * @param socket_t Socket
     * @return bool
     */
    [[maybe_unused]]
    bool Socket::shutdown_socket(
        const socket_t ar_sock
    ) noexcept
    {
        if( !Socket::is_valid_socket(ar_sock) )
            return false;

        #if __OS_WINDOWS__
            ::shutdown(ar_sock, SD_BOTH);
        #elif __OS_POSIX__
            ::shutdown(ar_sock, SHUT_RDWR);
        #endif

	    return true;
    }

    /**
     * @brief Close Socket
     * 
     * İşletim sistemi türüne göre farklı olan soket kapatma
     * işlemini yapar. Geçersiz soket kontrolü sonrası
     * soketi kapatır
     * 
     * @param socket_t Socket
     * @return bool
     */
    [[maybe_unused]]
    bool Socket::close_socket(
        const socket_t ar_sock
    ) noexcept
    {
        if( !Socket::is_valid_socket(ar_sock) )
            return false;

        #if __OS_WINDOWS__
            return ::closesocket(ar_sock) != SOCKET_ERROR;
        #elif __OS_POSIX__
            return ::close(ar_sock) == 0;
        #endif
    }

    /**
     * @brief Get Ip
     * 
     * Verilen soketin geçerliliğini kontrol ettikten
     * sonra soketi çözümler ve ip adresini verir
     * 
     * @param socket_t Socket
     * @return string
     */
    [[maybe_unused]] [[nodiscard]]
    std::string Socket::get_ip(
        const socket_t ar_sock
    ) noexcept
    {
        if( !Socket::is_valid_socket(ar_sock) )
            return std::string{};

        sockaddr_storage tm_addr {};
        socklen_t tm_len = sizeof(tm_addr);

        if (::getpeername(ar_sock, reinterpret_cast<sockaddr*>(&tm_addr), &tm_len) != 0)
            return {};

        char tm_ipstr[INET6_ADDRSTRLEN] {};

        if (tm_addr.ss_family == AF_INET)
        {
            auto* tm_s = reinterpret_cast<sockaddr_in*>(&tm_addr);
            inet_ntop(AF_INET, &tm_s->sin_addr, tm_ipstr, sizeof(tm_ipstr));
        }
        else if (tm_addr.ss_family == AF_INET6)
        {
            auto* tm_s = reinterpret_cast<sockaddr_in6*>(&tm_addr);
            inet_ntop(AF_INET6, &tm_s->sin6_addr, tm_ipstr, sizeof(tm_ipstr));
        }

        return std::string{ tm_ipstr };
    }

    /**
     * @brief Handshake Send
     * 
     * Soket vb. bazı özel işlemlerde uyuşma kontrolü gerekir, yoksa aksi
     * halde beklenmedik durumlar oluşabilir. İki soket arasındaki sürümün
     * aynı hash numarasına sahip olacağını biliyoruz, bu hash numaralarını
     * client (istemci) için connect sonrası şifreleme yapılmadan yapılmalı
     * 
     * @param socket_t Socket
     * @return Status
     */
    [[maybe_unused]] [[nodiscard]]
    Status Socket::handshake_send_verify(const socket_t ar_sock) noexcept
    {
        if( !Socket::is_valid_socket(ar_sock) )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::socket_not_valid));

        char tm_hash[ss_hash_hex_size] {};
        std::memcpy(tm_hash, ss_ver_hash.c_str(), ss_hash_hex_size);

        int tm_sent = ::send(ar_sock, reinterpret_cast<const char*>(tm_hash), sizeof(tm_hash), 0);
        if( tm_sent != sizeof(tm_hash) )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::packet_not_send));

        return Status::ok(domain_t::socket, status::to_underlying(socket_code_t::handshake_send_hash));
    }

    /**
     * @brief Handshake Receive Verify
     * 
     * Soket vb. bazı özel işlemlerde uyuşma kontrolü gerekir, yoksa aksi
     * halde beklenmedik durumlar oluşabilir. İki soket arasındaki sürümün
     * aynı hash numarasına sahip olacağını biliyoruz, bu hash numaralarını
     * server (sunucu) için accept sonrası yapılmalı. Eğer istenirse direk
     * soket bağlantısı sonlandırılıp kapatılabilir.
     * 
     * @param socket_t Socket
     * @param bool Close
     * @return Status
     */
    [[maybe_unused]] [[nodiscard]]
    Status Socket::handshake_recv_verify(const socket_t ar_sock, const bool ar_close) noexcept
    {
        if( !Socket::is_valid_socket(ar_sock) )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::socket_not_valid));

        char tm_hash[ss_hash_hex_size] {};

        int tm_recv = ::recv(ar_sock, reinterpret_cast<char*>(tm_hash), sizeof(tm_hash), MSG_WAITALL);
        if( tm_recv != sizeof(tm_hash))
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::not_enough_data_in_packet));

        if( std::strncmp(tm_hash, ss_ver_hash.c_str(), sizeof(tm_hash)) != 0 )
        {
            if( ar_close )
            {
                Socket::shutdown_socket(ar_sock);
                Socket::close_socket(ar_sock);
            }

            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::socket_hash_not_match));
        }

        return Status::ok(domain_t::socket, status::to_underlying(socket_code_t::socket_hash_match));
    }
}
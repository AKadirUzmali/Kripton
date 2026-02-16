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
#include <pool/cryptopool.hpp>

#include <crypto/crypto.hpp>

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
        ipv4 = 0,
        ipv6,
        dual
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
        
        warn = 2000,

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
        std::string m_username { "" };
        uint16_t m_try_passwd { 0 };
        uint16_t m_same_user_count { 0 };
    };

    // Struct
    struct SocketCtx
    {
        std::string m_ip;
        UserPacket m_user;
    };

    #if __OS_WINDOWS
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
    using namespace crypto;
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

    static constexpr wait_time_t _MIN_WAIT_TIME = 0;    // microsec
    static constexpr wait_time_t _DEF_WAIT_TIME = 100;  // microsec
    static constexpr wait_time_t _MAX_WAIT_TIME = 1000; // microsec

    // Flag
    static constexpr flag::flag_t _FLAG_SOCKET_LOGGER = { 1 << 0 };

    // Version Hash
    static constexpr uint8_t ss_hash_hex_size = 16;

    static constexpr uint32_t ss_hash_code = (
        netpacket::_SIZE_HEADER +
        netpacket::_SIZE_PASSWORD + netpacket::_SIZE_USERNAME + netpacket::_SIZE_MESSAGE +
        netpacket::_PACK_LEN_PASSWORD + netpacket::_PACK_LEN_PASSWORD + netpacket::_PACK_LEN_PASSWORD +
        netpacket::_PACK_START_LEN_PASSWORD +
        netpacket::_SIZE_OVER_SOCKET
    ) % (uint32_t)~0;

    static constexpr hash::vch::Vch<ss_hash_hex_size> ss_ver_hash("2026-02-09|socket", ss_hash_code);

    // Function Define
    [[maybe_unused]] [[nodiscard]] static inline constexpr bool is_valid_socket(const socket_t ar_sock) noexcept;
    [[maybe_unused]] [[nodiscard]] static inline constexpr bool is_valid_port(const socket_port_t ar_port) noexcept;
    [[maybe_unused]] [[nodiscard]] static inline constexpr bool is_valid_ipv(const ipv_t ar_ipv) noexcept;

    [[maybe_unused]] static bool close_socket(const socket_t ar_sock) noexcept;
    [[maybe_unused]] [[nodiscard]] static std::string_view get_ip(const socket_t ar_sock) noexcept;

    [[maybe_unused]] [[nodiscard]] static Status handshake_send(const socket_t ar_sock) noexcept;
    [[maybe_unused]] [[nodiscard]] static Status handshake_recv_verify(const socket_t ar_sock) noexcept;

    // Class
    template <class AlgoT>
    class Socket : public virtual CrashHandler
    {
        static_assert(std::is_base_of_v<algorithm::Algorithm, AlgoT>, "Socket<AlgoT>: AlgoT must derive from Algorithm");

        private:
            stream::Crypto<AlgoT>& m_cipher;
            log::Logger<file::FileOut> m_logger;

            policy::AccessPolicy m_policy;

            flag::Flag m_flag;

            socket_t m_sock;
            ipv_t m_ipv;
            socket_port_t m_port;

            mutable std::mutex m_mtx;

            inline static std::atomic<uint32_t> s_total_sock { 0 };

            #if __OS_WINDOWS__
                inline std::atomic<bool> s_wsa_init { false };
            #endif

            inline static void inc_total_socket() noexcept { ++s_total_sock; };
            inline static void dec_total_socket() noexcept { if( s_total_sock ) --s_total_sock; };

        public:
            explicit Socket(
                stream::Crypto<AlgoT>& ar_cipher,
                const std::string& ar_filepath,
                const std::string& ar_name,
                const std::string& ar_password,
                const socket_port_t ar_port,
                const ipv_t ar_ipv,
                const flag::flag_t ar_flag = 0
            );

            virtual ~Socket();

            virtual inline bool has_error() const noexcept;
            virtual inline bool has_socket() const noexcept;
            virtual inline bool has_port() const noexcept;
            virtual inline bool has_ipv() const noexcept;
            virtual inline bool has_cipher() const noexcept;

            static inline uint32_t get_total_socket() noexcept;

            virtual inline socket_t get_socket() const noexcept;
            virtual inline socket_port_t get_port() const noexcept;
            virtual inline ipv_t get_ipv() const noexcept;
            virtual inline const std::string& get_name() const noexcept;

            virtual Status run() noexcept { return Status::warn(domain_t::socket, status::to_underlying(socket_code_t::socket_no_run)); }
            virtual Status stop() noexcept { return Status::warn(domain_t::socket, status::to_underlying(socket_code_t::socket_no_stop)); }

            virtual void print() noexcept;

            virtual inline stream::Crypto<AlgoT>& get_cipher() noexcept;
            virtual inline log::Logger<output::file::FileOut>& get_logger() noexcept;
            virtual inline policy::AccessPolicy& get_policy() noexcept;
            virtual inline flag::Flag& get_flag() noexcept;

            virtual Status set_socket(const socket_t ar_sock) noexcept;
            virtual Status set_port(const socket_port_t ar_port) noexcept;
            virtual Status set_ipv(const ipv_t ar_ipv) noexcept;

            virtual Status create() noexcept;
            virtual Status close() noexcept;
            virtual Status clear() noexcept;

            virtual Status send(const socket_t ar_sock, DataPacket& ar_datapack) noexcept;
            virtual Status recv(const socket_t ar_sock, DataPacket& ar_datapack) noexcept;

        protected:
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
     * @param Crypto<AlgoT>& Cipher
     * @param string& Filepath
     * @param string& Name
     * @param string& Password
     * @param socket_port_t Port
     * @param ipv_t IP Version
     * @param flag_t Flag
     */
    template <class AlgoT>
    Socket<AlgoT>::Socket(
        stream::Crypto<AlgoT>& ar_cipher,
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
        this->set_port(ar_port);
        this->set_ipv(ar_ipv);

        #if __OS_WINDOWS__
            if( !s_wsa_init.load(std::memory_order_relaxed) && ::WSAGetLastError() != WSAONINITALISED )
            {
                WSAData tm_wsadata;
                if( ::WSAStartup(MAKEWORD(2,2), &tm_wsadata) != 0 )
                    this->m_flag.set(_FLAG_SOCKET_ERR);

                s_wsa_init.store(true);
            }
        #endif

        inc_total_socket();
    }

    /**
     * @brief ~Socket
     * 
     * Sınıfı sonlandıracağı zaman ilk kapatır, sonra
     * temizler ve en sonda toplam soket sayısında eksiltme
     * yaparak sonlandırır
     */
    template <class AlgoT>
    Socket<AlgoT>::~Socket()
    {
        this->close();
        this->clear();

        dec_total_socket();
    }

    /**
     * @brief Has Error
     * 
     * Hata olup olmadığını döndürür
     * 
     * @return bool
     */
    template <class AlgoT>
    bool Socket<AlgoT>::has_error() const noexcept
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
    template <class AlgoT>
    bool Socket<AlgoT>::has_socket() const noexcept
    {
        return is_valid_socket(this->m_sock);
    }

    /**
     * @brief Has Port
     * 
     * Geçerli bir port numarası olup olmadığını döndürür
     * 
     * @return bool
     */
    template <class AlgoT>
    bool Socket<AlgoT>::has_port() const noexcept
    {
        return is_valid_port(this->m_port);
    }

    /**
     * @brief Has Ipv
     * 
     * Geçerli bir ip sürümü olup olmadığını döndürür
     * 
     * @return bool
     */
    template <class AlgoT>
    bool Socket<AlgoT>::has_ipv() const noexcept
    {
        return is_valid_ipv(this->m_ipv);
    }

    /**
     * @brief Has Cipher
     * 
     * Geçerli bir şifreleyici algoritma olup olmamasını döndürür
     * 
     * @return bool
     */
    template <class AlgoT>
    bool Socket<AlgoT>::has_cipher() const noexcept
    {
        return !this->m_cipher.algorithm().has_error();
    }

    /**
     * @brief Get Total Socket
     * 
     * Oluşturulmuş toplam soket sayısını döndürür
     * 
     * @return uint32_t
     */
    template <class AlgoT>
    uint32_t Socket<AlgoT>::get_total_socket() noexcept
    {
        return s_total_sock.load(std::memory_order_relaxed);
    }

    /**
     * @brief Get Socket
     * 
     * Soket değerini döndürür
     * 
     * @return socket_t
     */
    template <class AlgoT>
    socket_t Socket<AlgoT>::get_socket() const noexcept
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
    template <class AlgoT>
    socket_port_t Socket<AlgoT>::get_port() const noexcept
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
    template <class AlgoT>
    ipv_t Socket<AlgoT>::get_ipv() const noexcept
    {
        return this->m_ipv;
    }

    /**
     * @brief Get Name
     * 
     * Sokete verilen isimi döndürür
     * 
     * @return string&
     */
    template <class AlgoT>
    const std::string& Socket<AlgoT>::get_name() const noexcept
    {
        return this->m_policy.get_username();
    }

    /**
     * @brief Print
     * 
     * Soket ile ilgili bilgileri kullanarak çıktı verir
     * 
     * @return print
     */
    template <class AlgoT>
    void Socket<AlgoT>::print() noexcept
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
     * @return Crypto<AlgoT>&
     */
    template <class AlgoT>
    stream::Crypto<AlgoT>& Socket<AlgoT>::get_cipher() noexcept
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
    template <class AlgoT>
    log::Logger<output::file::FileOut>& Socket<AlgoT>::get_logger() noexcept
    {
        return this->m_logger;
    }

    /**
     * @brief Get Policy
     * 
     * Policy (Politika) yapısının adresini referans olarak döndürür bu
     * sayede performans ve güvenlik kaygısı olmadan işleme devam
     * edilebilir
     * 
     * @param AccessPolicy&
     */
    template <class AlgoT>
    policy::AccessPolicy& Socket<AlgoT>::get_policy() noexcept
    {
        return this->m_policy;
    }

    /**
     * @brief Get Flag
     * 
     * Flag (bayrak) değerini döndürür
     * 
     * @return flag_t
     */
    template <class AlgoT>
    flag::Flag& Socket<AlgoT>::get_flag() noexcept
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
    template <class AlgoT>
    Status Socket<AlgoT>::set_socket(
        const socket_t ar_sock
    ) noexcept
    {
        if( !is_valid_socket(ar_sock) )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::socket_not_valid));

        std::scoped_lock tm_lock(this->m_mtx);
        this->m_sock = ar_sock;

        return is_valid_socket(this->m_sock) ?
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
    template <class AlgoT>
    Status Socket<AlgoT>::set_port(
        const socket_port_t ar_port
    ) noexcept
    {
        if( !is_valid_port(ar_port) )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::port_not_valid));

        std::scoped_lock tm_lock(this->m_mtx);
        this->m_port = ar_port;

        return is_valid_port(this->m_port) ?
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
    template <class AlgoT>
    Status Socket<AlgoT>::set_ipv(
        const ipv_t ar_ipv
    ) noexcept
    {
        if( !is_valid_ipv(ar_ipv) )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::ipv_not_valid));

        std::scoped_lock tm_lock(this->m_mtx);
        this->m_ipv = ar_ipv;

        return is_valid_ipv(this->m_ipv) ?
            Status::ok(domain_t::socket, status::to_underlying(socket_code_t::ipv_set))
            : Status::err(domain_t::socket, status::to_underlying(socket_code_t::ipv_set_err_not_valid));
    }

    /**
     * @brief Create
     * 
     * Ayarlanmış soket verilerini kullanarak soket bağlantısını
     * başlatır. Sonucu durum ile döndürür
     * 
     * @param Status
     */
    template <class AlgoT>
    Status Socket<AlgoT>::create() noexcept
    {
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
            case ipv_t::dual:
            default:
                tm_sock_domain = tcp::ipv6::domain;
                tm_sock_type   = tcp::ipv6::type;
                tm_sock_proto  = tcp::ipv6::protocol;                
                break;
        }

        socket_t tm_sock = ::socket(tm_sock_domain, tm_sock_type, tm_sock_proto);
        if( !is_valid_socket(tm_sock) )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::socket_not_valid));

        this->set_socket(tm_sock);

        {
            std::scoped_lock tm_lock(this->m_mtx);

            if( this->m_ipv != ipv_t::ipv4 )
            {
                int tm_off = 0;
                ::setsockopt(this->m_sock, IPPROTO_IPV6, IPV6_V6ONLY, reinterpret_cast<const char*>(&tm_off), sizeof(tm_off));
            }
        }

        return is_valid_socket(this->m_sock) ?
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
    template <class AlgoT>
    Status Socket<AlgoT>::close() noexcept
    {
        if( !is_valid_socket(this->m_sock) )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::socket_not_valid));

        if( !close_socket(this->m_sock) )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::socket_not_close));

        {
            std::scoped_lock tm_lock(this->m_mtx);
            this->m_sock = ss_inv_socket;
        }

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
    template <class AlgoT>
    Status Socket<AlgoT>::clear() noexcept
    {
        Status tm_close_status = this->close();
        if( !tm_close_status.is_ok() )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::socket_not_clear));

        {
            std::scoped_lock tm_lock(this->m_mtx);

            this->m_sock = ss_inv_socket;
            this->m_port = ss_inv_port;
        }

        this->m_flag.clear();

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
    template <class AlgoT>
    Status Socket<AlgoT>::send(
        const socket_t ar_target_sock,
        DataPacket& ar_datapack
    ) noexcept
    {
        if( !is_valid_socket(ar_target_sock) )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::target_socket_not_valid));

        if( ar_datapack.m_name.empty() || ar_datapack.m_msg.empty() )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::not_enough_data_in_packet));

        this->get_cipher().encrypt(ar_datapack.m_pwd);
        this->get_cipher().encrypt(ar_datapack.m_name);
        this->get_cipher().encrypt(ar_datapack.m_msg);

        netpacket::NetPacket tm_netpack(ar_datapack.m_pwd, ar_datapack.m_name, ar_datapack.m_msg);
        const auto& tm_buffer = tm_netpack.get();

        uint32_t tm_total_sent = 0;
        const uint32_t tm_total_size = tm_buffer.size();

        while( tm_total_sent < tm_total_size ) {
            int tm_sent = ::send(ar_target_sock,
                reinterpret_cast<const char*>(tm_buffer.data()) + tm_total_sent,
                static_cast<int>(tm_total_size - tm_total_sent),
                0
            );

            if( tm_sent < 0 )
                return Status::err(domain_t::socket, status::to_underlying(socket_code_t::packet_not_send));
            else if ( tm_sent == 0 )
                return Status::err(domain_t::socket, status::to_underlying(socket_code_t::socket_can_be_close));

            tm_total_sent += tm_sent;
        }

        if( this->m_flag & _FLAG_SOCKET_LOGGER ) {
            const std::string tm_ip(get_ip(ar_target_sock).data());
            this->m_logger.write(level_t::Info, tm_ip + " sent", GET_SOURCE);
        }

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
    template <class AlgoT>
    Status Socket<AlgoT>::recv(
        const socket_t ar_target_sock,
        DataPacket& ar_datapack
    ) noexcept
    {
        if( !is_valid_socket(ar_target_sock) )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::target_socket_not_valid));

        char tm_header[netpacket::_SIZE_HEADER] {};

        int tm_recv = ::recv(ar_target_sock, tm_header, netpacket::_SIZE_HEADER, MSG_WAITALL);
        if( tm_recv < 0 ) return Status::err(domain_t::socket, status::to_underlying(socket_code_t::socket_not_recv_header));
        else if( tm_recv == 0 ) return Status::err(domain_t::socket, status::to_underlying(socket_code_t::recv_socket_header_close));

        uint16_t tm_len_pwd = 0;
        uint16_t tm_len_name = 0;
        uint16_t tm_len_msg = 0;

        if( std::sscanf(tm_header, "%03hu%03hu%04hu", &tm_len_pwd, &tm_len_name, &tm_len_msg) != 3 )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::packet_corrupt));

        const uint32_t tm_payload_len = tm_len_pwd + tm_len_name + tm_len_msg;
        if( tm_payload_len == 0 )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::packet_no_data));

        std::vector<char> tm_payload(tm_payload_len);

        tm_recv = ::recv(ar_target_sock, tm_payload.data(), static_cast<int>(tm_payload_len), MSG_WAITALL);
        if( tm_recv < 0 ) return Status::err(domain_t::socket, status::to_underlying(socket_code_t::socket_not_recv));
        else if( tm_recv == 0 ) return Status::err(domain_t::socket, status::to_underlying(socket_code_t::recv_socket_close));

        uint32_t tm_offset = 0;

        std::string tm_pwd(tm_payload.data() + tm_offset, tm_len_pwd);
        tm_offset += tm_len_pwd;

        std::string tm_name(tm_payload.data() + tm_offset, tm_len_name);
        tm_offset += tm_len_name;

        std::string tm_msg(tm_payload.data() + tm_offset, tm_len_msg);
        tm_offset += tm_len_msg;

        this->get_cipher().decrypt(tm_pwd);
        this->get_cipher().decrypt(tm_name);
        this->get_cipher().decrypt(tm_msg);

        ar_datapack.m_pwd = tm_pwd;
        ar_datapack.m_name = tm_name;
        ar_datapack.m_msg = tm_msg;

        if( ar_datapack.m_name.empty() )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::recv_username_empty));
        else if( ar_datapack.m_msg.empty() )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::recv_message_empty));

        if( this->m_flag & _FLAG_SOCKET_LOGGER ) {
            const std::string tm_ip(get_ip(ar_target_sock).data());
            this->m_logger.write(level_t::Info, tm_ip + " received", GET_SOURCE);
        }

        return Status::ok(domain_t::socket, status::to_underlying(socket_code_t::socket_data_recv));
    }

    /**
     * @brief Crashed
     * 
     * Olası bir program hatası ya da ani kesinti durumunda
     * yapılması gerekenleri çalıştırarak sistem sızıntılarını
     * önlemeye çalışır
     */
    template <class AlgoT>
    void Socket<AlgoT>::crashed() noexcept
    {
        if( this->m_flag & _FLAG_SOCKET_LOGGER ) {
            const std::string tm_msg = "Crash Code: " + std::to_string(CrashHandler::get_signal());
            this->m_logger.write(level_t::Err, tm_msg, GET_SOURCE);
        }

        this->close();
        this->clear();
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
    constexpr bool is_valid_socket(
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
    constexpr bool is_valid_port(
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
    constexpr bool is_valid_ipv(
        const ipv_t ar_ipv
    ) noexcept
    {
        switch(ar_ipv)
        {
            case ipv_t::ipv4:
            case ipv_t::ipv6:
            case ipv_t::dual:
                return true;
        }

        return false;
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
    bool close_socket(
        const socket_t ar_sock
    ) noexcept
    {
        if( !is_valid_socket(ar_sock) )
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
     * @return string_view
     */
    [[maybe_unused]] [[nodiscard]]
    std::string_view get_ip(
        const socket_t ar_sock
    ) noexcept
    {
        if( !is_valid_socket(ar_sock) )
            return std::string_view{};

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

        return std::string_view{ tm_ipstr };
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
    Status handshake_send(
        const socket_t ar_sock
    ) noexcept
    {
        if( !is_valid_socket(ar_sock) )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::socket_not_valid));

        char tm_hash[ss_hash_hex_size] {};
        std::strncpy(tm_hash, ss_ver_hash.c_str(), sizeof(tm_hash));

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
     * server (sunucu) için accept sonrası thread (işlem) açılmadan önce yapmalı
     * 
     * @param socket_t Socket
     * @return Status
     */
    [[maybe_unused]] [[nodiscard]]
    Status handshake_recv_verify(
        const socket_t ar_sock
    ) noexcept
    {
        if( !is_valid_socket(ar_sock) )
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::socket_not_valid));

        char tm_hash[ss_hash_hex_size] {};

        int tm_recv = ::recv(ar_sock, reinterpret_cast<char*>(tm_hash), sizeof(tm_hash), MSG_WAITALL);
        if( tm_recv != sizeof(tm_hash))
            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::not_enough_data_in_packet));

        if( std::strncmp(tm_hash, ss_ver_hash.c_str(), sizeof(tm_hash)) != 0 )
        {
            ::shutdown(ar_sock, SHUT_RDWR);
            close_socket(ar_sock);

            return Status::err(domain_t::socket, status::to_underlying(socket_code_t::socket_hash_not_match));
        }

        return Status::ok(domain_t::socket, status::to_underlying(socket_code_t::socket_hash_match));
    }
}
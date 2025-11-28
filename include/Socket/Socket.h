// Abdulkadir U. - 18/11/2025
#pragma once

/**
 * Socket (Soket)
 * 
 * Veri iletişiminin en önemli parçası olan
 * soket yapısının tasarımını içerir.
 * Proje yapısı gereği şifreleme desteği de sunar.
 * Bu sayede güvenli veri iletişimi sağlanabilir.
 * Sunucu ve istemci tarafında kullanılabilir.
 */

// Include:
#include <Platform/Platform.h>
#include <Algorithm/AlgorithmPool.h>
#include <Flag/Flag.h>
#include <Tool/Utf/Utf.h>
#include <Socket/AccessPolicy.h>

#include <mutex>
#include <atomic>
#include <iostream>

// Using Namespace:
using namespace core::algorithmpool;

// Os Support:
#if defined(__PLATFORM_WINDOWS__)
    #include <winsock2.h>
    #include <ws2tcpip.h>
    
    using socket_t = SOCKET;
    using socket_port_t = uint16_t;
    using socket_domain_t = int;
    using socket_conn_t = int;
    using socket_proto_t = int;

    constexpr socket_t invalid_socket = INVALID_SOCKET;
    constexpr socket_port_t invalid_port = 0;

    inline socket_t close_socket(socket_t _sock) { return static_cast<socket_t>(closesocket(_sock)); }
#elif defined(__PLATFORM_LINUX__) || defined(__PLATFORM_UNIX__)
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <unistd.h>

    using socket_t = int;
    using socket_port_t = uint16_t;
    using socket_domain_t = int;
    using socket_conn_t = int;
    using socket_proto_t = int;

    using socket_addr = sockaddr;

    constexpr socket_t invalid_socket = -1;
    constexpr socket_port_t invalid_port = 0;

    inline socket_t close_socket(socket_t _sock) { return static_cast<socket_t>(close(_sock)); }
#endif

// Namespace: Core::VirtualBase
namespace core::virbase
{
    // Namespace: Socket
    namespace socket
    {
        // Using:
        using buffer_size_t = size_t;

        // Enum Class: Socket Code
        enum class e_socket : size_t
        {
            err_socket_create,
            err_socket_close,
            err_socket_clear,
            err_socket_print,
            err_socket_set,
            err_socket_set_type,
            err_socket_set_type_unknown,
            err_socket_set_algorithm,
            err_socket_port_invalid,
            err_socket_set_port,
            err_buffer_size_under_limit,
            err_buffer_size_over_limit,
            err_set_buffer_size_fail,
            err_set_nickname_len_under_limit,
            err_set_nickname_len_over_limit,
            err_set_nickname_fail,

            succ_socket_create,
            succ_socket_close,
            succ_socket_clear,
            succ_socket_print,
            succ_socket_set,
            succ_socket_set_type_udp,
            succ_socket_set_type_tcp,
            succ_socket_set_algorithm,
            succ_socket_set_port,
            succ_set_buffer_size,
            succ_set_nickname,

            warn_socket_type_same
        };

        // Limit:
        constexpr size_t MAX_SOCKET_BUFFER_SIZE = 8192;

        constexpr size_t MIN_SOCKET_BACKLOG = 1;
        constexpr size_t DEF_SOCKET_BACKLOG = 4;
        constexpr size_t MAX_SOCKET_BACKLOG = 1024;

        constexpr size_t MIN_LEN_NICKNAME = 2;
        constexpr size_t MAX_LEN_NICKNAME = 32;

        constexpr size_t MIN_SIZE_BUFFER = sizeof(char32_t);
        constexpr size_t DEF_SIZE_BUFFER = 4096;
        constexpr size_t MAX_SIZE_BUFFER = (uint16_t)~0;

        // Flag: Socket Status
        inline constexpr flag_t flag_socket_free            { 1 << 0 };
        inline constexpr flag_t flag_socket_created         { 1 << 1 };
        inline constexpr flag_t flag_socket_error           { 1 << 2 };
        inline constexpr flag_t flag_socket_open            { 1 << 3 };

        // Class: Tcp
        class Tcp final
        {
            public:
                Tcp() = delete;

                static constexpr socket_domain_t domain = AF_INET;
                static constexpr socket_conn_t type = SOCK_STREAM;
                static constexpr socket_proto_t protocol = IPPROTO_TCP;
        };
    }

    // Using Namespace:
    using namespace socket;

    // Class: Socket
    template<class Algo>
    class Socket
    {
        static_assert(std::is_base_of<Algorithm, Algo>::value, "<Algo> Has To Be At The Base Of The <Algorithm> Class");

        private:
            Algo algo;
            Flag flag;

            socket_t sock;
            socket_port_t port;

            mutable std::mutex sock_mtx;

            std::atomic<buffer_size_t> buffer_size;
            std::u32string nickname;

        public:
            explicit Socket
            (
                Algo&& _algorithm,
                const std::u32string& _nickname,
                const socket_port_t _port = invalid_port,
                const buffer_size_t _buffsize = DEF_SIZE_BUFFER
            ) noexcept;

            ~Socket() noexcept;

            virtual inline bool hasError() const noexcept;

            virtual inline bool isFree() const noexcept;
            virtual inline bool isNotFree() const noexcept;
            virtual inline bool isCreate() const noexcept;
            virtual inline bool isNotCreate() const noexcept;
            virtual inline bool isOpen() const noexcept;
            virtual inline bool isClose() const noexcept;

            virtual inline buffer_size_t getBufferSize() const noexcept;
            virtual e_socket setBufferSize(const buffer_size_t = DEF_SIZE_BUFFER) noexcept;

            virtual inline const socket_t& getSocket() const noexcept;
            virtual e_socket setSocket(const socket_t) noexcept;

            virtual inline const socket_port_t& getPort() const noexcept;
            virtual e_socket setPort(const socket_port_t) noexcept;

            virtual inline const std::u32string& getNickname() const noexcept;
            virtual e_socket setNickname(const std::u32string&, const bool = true) noexcept;

            virtual inline Algo& getAlgorithm() noexcept;

            virtual e_socket create() noexcept;
            virtual e_socket close() noexcept;
            virtual e_socket clear() noexcept;
            virtual e_socket print() noexcept;
    };
}

// Using Namespace:
using namespace core::virbase::socket;

/**
 * [Public] Constructor
 * 
 * Soket yapısını oluşturacak ve
 * şifreleme algoritması referansını
 * alarak güvenli iletişim için
 * gerekli altyapıyı hazırlayacak
 * 
 * @param Algorithm& Şifreleme Algoritması
 * @return Socket
 */
template<class Algo>
Socket<Algo>::Socket
(
    Algo&& _algorithm,
    const std::u32string& _nickname,
    const socket_port_t _port,
    const buffer_size_t _buffsize
) noexcept
:   algo(std::forward<Algo>(_algorithm)),
    flag(flag_socket_free),
    sock(invalid_socket),
    port(invalid_port),
    buffer_size(DEF_SIZE_BUFFER)
{
    this->setPort(_port);
    this->setBufferSize(_buffsize);
    this->setNickname(_nickname);
}

/**
 * @brief [Public] Destructor
 * 
 * Soket yapısını temizleyecek
 * ve bağlı kaynakları serbest bırakacak
 */
template<class Algo>
Socket<Algo>::~Socket() noexcept
{
    this->clear();
}

/**
 * @brief [Public] Has Error
 * 
 * Hata olup olmadığının bilgisini döndürsün
 * 
 * @return bool
 */
template<class Algo>
bool Socket<Algo>::hasError() const noexcept
{
    constexpr flag_t error_flags = flag_socket_error;
    return static_cast<bool>(this->flag.get() & error_flags);
}

/**
 * @brief [Public] Is Free
 * 
 * Soketin boş olup olmadığı bilgisini döndürsün
 * 
 * @return bool
 */
template<class Algo>
bool Socket<Algo>::isFree() const noexcept
{
    return static_cast<bool>(this->flag.get() & flag_socket_free);
}

/**
 * @brief [Public] Is Not Free
 * 
 * Soketin boş olup olmadığı bilgisini döndürsün
 * 
 * @return bool
 */
template<class Algo>
bool Socket<Algo>::isNotFree() const noexcept
{
    return !this->isFree();
}

/**
 * @brief [Public] Is Create
 * 
 * Soketin oluşturulup oluşturulmadığı bilgisini döndürsün
 * 
 * @return bool
 */
template<class Algo>
bool Socket<Algo>::isCreate() const noexcept
{
    return static_cast<bool>(this->flag.get() & flag_socket_created);
}

/**
 * @brief [Public] Is Not Create
 * 
 * Soketin oluşturulup oluşturulmadığı bilgisini döndürsün
 * 
 * @return bool
 */
template<class Algo>
bool Socket<Algo>::isNotCreate() const noexcept
{
    return !this->isCreate();
}

/**
 * @brief [Public] Is Open
 * 
 * Soketin açık olup olmadığı bilgisini döndürsün
 * 
 * @return bool
 */
template<class Algo>
bool Socket<Algo>::isOpen() const noexcept
{
    return static_cast<bool>(this->flag.get() & flag_socket_open) &&
        this->getSocket() != invalid_socket;
}

/**
 * @brief [Public] Is Close
 * 
 * Soketin kapalı olup olmadığı bilgisini döndürsün
 * 
 * @return bool
 */
template<class Algo>
bool Socket<Algo>::isClose() const noexcept
{
    return !this->isOpen();
}

/**
 * @brief [Public] Get Buffer Size
 * 
 * Soketde veri akışını depolamak için
 * gerekli olan geçici depo (buffer) için
 * verilen boyutu döndürür
 * 
 * @return buffer_size_t
 */
template<class Algo>
buffer_size_t Socket<Algo>::getBufferSize() const noexcept
{
    return this->buffer_size;
}

/**
 * @brief [Public] Set Buffer Size
 * 
 * Geçici veri saklama deposu (buffer) için
 * yeni boyut ayarı yapmayı sağlamak
 * 
 * @param buffer_size_t Buffer Size
 * @return e_socket
 */
template<class Algo>
e_socket Socket<Algo>::setBufferSize
(
    const buffer_size_t _buffsize
) noexcept
{
    if( _buffsize < MIN_SIZE_BUFFER ) return e_socket::err_buffer_size_under_limit;
    else if( _buffsize > MAX_SIZE_BUFFER ) return e_socket::err_buffer_size_over_limit;

    this->buffer_size.store(_buffsize);
    return this->buffer_size.load() == _buffsize ?
        e_socket::succ_set_buffer_size :
        e_socket::err_set_buffer_size_fail;
}

/**
 * @brief [Public] Get Socket
 * 
 * Soket tanımlayıcısını döndürecek
 * 
 * @return socket_t& Socket
 */
template<class Algo>
const socket_t& Socket<Algo>::getSocket() const noexcept
{
    return this->sock;
}

/**
 * @brief [Public] Set Socket
 * 
 * Soket tanımlayıcısını belirleyecek
 * 
 * @param socket_t Socket
 * @return e_socket Status
 */
template<class Algo>
e_socket Socket<Algo>::setSocket
(
    const socket_t _sock
) noexcept
{
    {
        std::scoped_lock<std::mutex> lock(this->sock_mtx);
        this->sock = _sock;
    }

    return (_sock != invalid_socket) ?
        e_socket::succ_socket_set :
        e_socket::err_socket_set;
}

/**
 * @brief [Public] Get Port
 * 
 * Sokete ait port numarasını içeren
 * nesneyi yanıt olarak döndürebilsin
 * 
 * @return socket_port_t
 */
template<class Algo>
const socket_port_t& Socket<Algo>::getPort() const noexcept
{
    return this->port;
}

/**
 * @brief [Public] Set Port
 * 
 * Soketin port numarası geçersiz olmadığı sürece
 * güncelleme imkanı sunabilsin
 * 
 * @param socket_port_t Socket Port
 * @return e_socket
 */
template<class Algo>
e_socket Socket<Algo>::setPort
(
    const socket_port_t _sockport
) noexcept
{
    if( _sockport <= invalid_port )
        return e_socket::err_socket_port_invalid;

    {
        std::scoped_lock<std::mutex> lock(this->sock_mtx);
        this->port = _sockport;
    }

    return this->port == _sockport ?
        e_socket::succ_socket_set_port :
        e_socket::err_socket_set_port;
}

/**
 * @brief [Public] Get Nickname
 * 
 * Soket bağlantısını kullanan kişiye ait
 * özel bir nickname vermesi amaçlanır ve
 * bu kullanıcıya ait adı döndürecek
 * 
 * @return u32string&
 */
template<class Algo>
const std::u32string& Socket<Algo>::getNickname() const noexcept
{
    return this->nickname;
}

/**
 * @brief [Public] Set Nickname
 * 
 * Soket bağlantısı sağlayan kullanıcıya ait
 * özel isimi ayarlamasını sağlayacak ve bu
 * sayede kullanıcı istediği durumda isimini
 * değiştirebilecek. Güvenlik aktif olduğunda
 * sadece belirlenen sınır arasında ise kabul
 * edilecek ama eğer güvenlik kapalı ise, sınırı
 * aşsa bile kırpılıp kabul edilecek
 * 
 * @param u32string& Nickname
 * @param bool Secure
 * @return e_socket
 */
template<class Algo>
e_socket Socket<Algo>::setNickname
(
    const std::u32string& _nickname,
    const bool _secure
) noexcept
{
    if( _nickname.empty() || _nickname.length() < MIN_LEN_NICKNAME )
        return e_socket::err_set_nickname_len_under_limit;

    else if( _secure && _nickname.length() > MAX_LEN_NICKNAME )
        return e_socket::err_set_nickname_len_over_limit;

    this->nickname = _nickname;
    return this->nickname == _nickname ?
        e_socket::succ_set_nickname :
        e_socket::err_set_nickname_fail;
}

/**
 * @brief [Public] Get Algorithm
 * 
 * Şifreleme algoritmasını döndürecek
 *
 * @return Algo& Algorithm
 */
template<class Algo>
Algo& Socket<Algo>::getAlgorithm() noexcept
{
    return this->algo;
}

/**
 * @brief [Public] Create
 * 
 * Soket için verilmiş olan bağlantı türüne (tcp, udp)
 * göre bağlantı oluşturmasını sağlıyoruz
 */
template<class Algo>
e_socket Socket<Algo>::create() noexcept
{
    socket_t fd = ::socket(Tcp::domain, Tcp::type, Tcp::protocol);

    if(fd <= invalid_socket) {
        this->flag.set(flag_socket_error);
        return e_socket::err_socket_create;
    }

    {
        std::scoped_lock<std::mutex> lock(this->sock_mtx);

        this->sock = fd;
        
        this->flag.set(flag_socket_created);
        this->flag.set(flag_socket_open);
        this->flag.unset(flag_socket_free);
        this->flag.unset(flag_socket_error);
    }

    return this->sock != invalid_socket ?
        e_socket::succ_socket_create :
        e_socket::err_socket_create;
}

/**
 * @brief [Public] Close
 * 
 * Socket yapısına ait bağlantıyı kontrol etsin.
 * Bağlantı yoksa hata, varsa kapatsın
 * 
 * @return e_socket
 */
template<class Algo>
e_socket Socket<Algo>::close() noexcept
{
    if( this->sock == invalid_socket )
        return e_socket::err_socket_close;

    {
        std::scoped_lock<std::mutex> lock(this->sock_mtx);

        close_socket(this->sock);

        this->sock = invalid_socket;

        this->flag.unset(flag_socket_open);
        this->flag.unset(flag_socket_created);
        this->flag.set(flag_socket_free);
    }

    return this->sock == invalid_socket ?
        e_socket::succ_socket_close :
        e_socket::err_socket_close;
}

/**
 * @brief [Public] Clear
 * 
 * Socket yapısına ait bağlantı, port,
 * bayrak değerlerini sıfırlayarak
 * temizleme yapsın
 * 
 * @return e_socket
 */
template<class Algo>
e_socket Socket<Algo>::clear() noexcept
{
    {
        std::scoped_lock<std::mutex> lock(this->sock_mtx);

        this->close();

        this->port = invalid_port;

        this->flag.clear();
        this->flag.set(flag_socket_free);
    }

    return this->sock == invalid_socket ?
        e_socket::succ_socket_clear :
        e_socket::err_socket_clear;
}

template<class Algo>
e_socket Socket<Algo>::print() noexcept
{
    std::cout << "\n==================== TCP SOCKET ====================\n";
    std::cout << std::setw(20) << std::left << "Socket " << " => " << (this->sock != invalid_socket ? "Valid" : "Invalid") << "\n";
    std::cout << std::setw(20) << std::left << "Has Error " << " => " << (this->hasError() ? "yes" : "no") << "\n";
    std::cout << std::setw(20) << std::left << "Is Open " << " => " << (this->isOpen() ? "yes" : "no") << "\n";
    std::cout << std::setw(20) << std::left << "Is Free " << " => " << (this->isFree() ? "yes" : "no") << "\n";
    std::cout << "======================================================\n\n";

    return e_socket::succ_socket_print;
}
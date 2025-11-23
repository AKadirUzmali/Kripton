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
    #include <unistd.h>

    using socket_t = int;
    using socket_port_t = uint16_t;
    using socket_domain_t = int;
    using socket_conn_t = int;
    using socket_proto_t = int;

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
        // Enum Class: Socket Code
        enum class e_socket : size_t
        {
            err_socket_create = 1000,
            err_socket_close,
            err_socket_clear,
            err_socket_print,
            err_socket_set,
            err_socket_set_type,
            err_socket_set_type_unknown,
            err_socket_set_algorithm,
            err_socket_set_port,

            succ_socket_create = 2000,
            succ_socket_close,
            succ_socket_clear,
            succ_socket_print,
            succ_socket_set,
            succ_socket_set_type_udp,
            succ_socket_set_type_tcp,
            succ_socket_set_algorithm,
            succ_socket_set_port
        };

        // Enum Class: Socket Type
        enum class e_socket_type : int
        {
            unknown = 0,
            tcp = SOCK_STREAM,
            udp = SOCK_DGRAM
        };

        // Limit:
        constexpr size_t MAX_SOCKET_BUFFER_SIZE = 8192;

        constexpr int DEF_SOCKET_BACKLOG = 4;
        constexpr int MAX_SOCKET_BACKLOG = 1024;

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

        // Class: Udp
        class Udp final
        {
            public:
                Udp() = delete;

                static constexpr socket_domain_t domain = AF_INET;
                static constexpr socket_conn_t type = SOCK_DGRAM;
                static constexpr socket_proto_t protocol = IPPROTO_UDP;
        };

        template<typename> struct is_socket_type : std::false_type {};
        template<> struct is_socket_type<Tcp> : std::true_type {};
        template<> struct is_socket_type<Udp> : std::true_type {};
    }

    // Using Namespace:
    using namespace socket;

    // Class: Socket
    template<class Algo, typename Type>
    class Socket
    {
        static_assert(std::is_base_of<Algorithm, Algo>::value, "<Algo> Has To Be At The Base Of The <Algorithm> Class");
        static_assert(socket::is_socket_type<Type>::value, "<Type> Has To Be <Tcp> Or <Udp> Class");

        private:
            Algo algo;
            Flag flag;

            socket_t sock;
            e_socket_type sock_type;
            socket_port_t port;

        public:
            explicit Socket(Algo&&) noexcept;
            ~Socket() noexcept;

            virtual inline bool hasError() const noexcept;

            virtual inline bool isFree() const noexcept;
            virtual inline bool isNotFree() const noexcept;
            virtual inline bool isCreate() const noexcept;
            virtual inline bool isNotCreate() const noexcept;
            virtual inline bool isOpen() const noexcept;
            virtual inline bool isClose() const noexcept;
            virtual inline bool isTcp() const noexcept;
            virtual inline bool isUdp() const noexcept;

            virtual inline const socket_t& getSocket() const noexcept;
            virtual e_socket setSocket(const socket_t) noexcept;

            virtual inline const socket_port_t& getPort() const noexcept;
            virtual e_socket setPort(const socket_port_t) noexcept;

            virtual inline const Algo& getAlgorithm() const noexcept;

            virtual inline const e_socket_type& getSocketType() const noexcept;
            virtual e_socket setSocketType(const e_socket_type) noexcept;

            virtual e_socket create() noexcept;
            virtual e_socket close() noexcept;
            virtual e_socket clear() noexcept;
            virtual e_socket print() noexcept;
    };
}

// Using Namespace:
using namespace core::virbase;
using namespace socket;

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
template<class Algo, typename Type>
Socket<Algo, Type>::Socket(Algo&& _algo) noexcept
:   algo(std::forward<Algo>(_algo)),
    flag(flag_socket_free),
    sock(invalid_socket),
    sock_type(e_socket_type::tcp),
    port(invalid_port)
{
    if constexpr ( std::is_same<Type, Udp>::value )
        this->setSocketType(e_socket_type::udp);
}

/**
 * @brief [Public] Destructor
 * 
 * Soket yapısını temizleyecek
 * ve bağlı kaynakları serbest bırakacak
 */
template<class Algo, typename Type>
Socket<Algo, Type>::~Socket() noexcept
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
template<class Algo, typename Type>
bool Socket<Algo, Type>::hasError() const noexcept
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
template<class Algo, typename Type>
bool Socket<Algo, Type>::isFree() const noexcept
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
template<class Algo, typename Type>
bool Socket<Algo, Type>::isNotFree() const noexcept
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
template<class Algo, typename Type>
bool Socket<Algo, Type>::isCreate() const noexcept
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
template<class Algo, typename Type>
bool Socket<Algo, Type>::isNotCreate() const noexcept
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
template<class Algo, typename Type>
bool Socket<Algo, Type>::isOpen() const noexcept
{
    return static_cast<bool>(this->flag.get() & flag_socket_open) && this->getSocket() != invalid_socket;
}

/**
 * @brief [Public] Is Close
 * 
 * Soketin kapalı olup olmadığı bilgisini döndürsün
 * 
 * @return bool
 */
template<class Algo, typename Type>
bool Socket<Algo, Type>::isClose() const noexcept
{
    return !this->isOpen();
}

/**
 * @brief [Public] Is Udp
 * 
 * Soketin bağlantı türünün udp olup olmadığı bilgisini döndürsün
 * 
 * @return bool
 */
template<class Algo, typename Type>
bool Socket<Algo, Type>::isUdp() const noexcept
{
    return static_cast<bool>(this->sock_type == e_socket_type::udp);
}

/**
 * @brief [Public] Is Tcp
 * 
 * Soketin bağlantı türünün tcp olup olmadığı bilgisini döndürsün
 * 
 * @return bool
 */
template<class Algo, typename Type>
bool Socket<Algo, Type>::isTcp() const noexcept
{
    return static_cast<bool>(this->sock_type == e_socket_type::tcp);
}

/**
 * @brief [Public] Get Socket
 * 
 * Soket tanımlayıcısını döndürecek
 * 
 * @return socket_t& Socket
 */
template<class Algo, typename Type>
const socket_t& Socket<Algo, Type>::getSocket() const noexcept
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
template<class Algo, typename Type>
e_socket Socket<Algo, Type>::setSocket(const socket_t _sock) noexcept
{
    this->sock = _sock;

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
template<class Algo, typename Type>
const socket_port_t& Socket<Algo, Type>::getPort() const noexcept
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
template<class Algo, typename Type>
e_socket Socket<Algo, Type>::setPort(const socket_port_t _sockport) noexcept
{
    if( _sockport <= invalid_port )
        return e_socket::err_socket_set_port;

    this->port = _sockport;
    return e_socket::succ_socket_set_port;
}

/**
 * @brief [Public] Get Algorithm
 * 
 * Şifreleme algoritmasını döndürecek
 *
 * @return Algo& Algorithm
 */
template<class Algo, typename Type>
const Algo& Socket<Algo, Type>::getAlgorithm() const noexcept
{
    return this->algo;
}

/**
 * @brief [Public] Get Socket Type
 * 
 * Soket türünün bilgisini döndürür
 * 
 * @return e_socket_type
 */
template<class Algo, typename Type>
const e_socket_type& Socket<Algo, Type>::getSocketType() const noexcept
{
    return this->sock_type;
}

/**
 * @brief [Public] Set Socket Type
 * 
 * Belirlenen soket türüne göre
 * soket tipini (tcp, udp) diye ayarlar.
 * Bu sayede iletişimi sağlayan tür tanımlanmış olur.
 * 
 * @param e_socket_type Socket Type
 * @return e_socket
 */
template<class Algo, typename Type>
e_socket Socket<Algo, Type>::setSocketType(const e_socket_type _socktype) noexcept
{
    switch( _socktype )
    {
        case e_socket_type::tcp:
            this->sock_type = e_socket_type::tcp;
            return e_socket::succ_socket_set_type_tcp;
        case e_socket_type::udp:
            this->sock_type = e_socket_type::udp;
            return e_socket::succ_socket_set_type_udp;
        default:
            return e_socket::err_socket_set_type_unknown;
    }

    return e_socket::err_socket_set_type;
}

/**
 * @brief [Public] Create
 * 
 * Soket için verilmiş olan bağlantı türüne (tcp, udp)
 * göre bağlantı oluşturmasını sağlıyoruz
 */
template<class Algo, typename Type>
e_socket Socket<Algo, Type>::create() noexcept
{
    if constexpr (std::is_same<Type, Tcp>::value)
        this->setSocketType(e_socket_type::tcp);
    else if constexpr (std::is_same<Type, Udp>::value)
        this->setSocketType(e_socket_type::udp);

    socket_t fd = ::socket(Type::domain, Type::type, Type::protocol);

    if(fd == invalid_socket) {
        this->flag.set(flag_socket_error);
        return e_socket::err_socket_create;
    }

    this->sock = fd;
    this->flag.set(flag_socket_created);
    this->flag.set(flag_socket_open);
    this->flag.unset(flag_socket_free);

    return e_socket::succ_socket_create;
}

/**
 * @brief [Public] Close
 * 
 * Socket yapısına ait bağlantıyı kontrol etsin.
 * Bağlantı yoksa hata, varsa kapatsın
 * 
 * @return e_socket
 */
template<class Algo, typename Type>
e_socket Socket<Algo, Type>::close() noexcept
{
    if( this->sock == invalid_socket )
        return e_socket::err_socket_close;

    close_socket(this->sock);

    this->sock = invalid_socket;
    this->flag.unset(flag_socket_open);
    this->flag.unset(flag_socket_created);
    this->flag.set(flag_socket_free);

    return e_socket::succ_socket_close;
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
template<class Algo, typename Type>
e_socket Socket<Algo, Type>::clear() noexcept
{
    this->close();

    this->port = invalid_port;
    this->flag.clear();
    this->flag.set(flag_socket_free);

    return e_socket::succ_socket_clear;
}

template<class Algo, typename Type>
e_socket Socket<Algo, Type>::print() noexcept
{
    std::cout << "\n==================== SOCKET ====================\n";
    std::cout << std::setw(20) << std::left << "Socket " << " => " << (this->sock != invalid_socket ? "Valid" : "Invalid") << "\n";
    std::cout << std::setw(20) << std::left << "Has Error " << " => " << (this->hasError() ? "yes" : "no") << "\n";
    std::cout << std::setw(20) << std::left << "Is Open " << " => " << (this->isOpen() ? "yes" : "no") << "\n";
    std::cout << std::setw(20) << std::left << "Is Tcp " << " => " << (this->isTcp() ? "yes" : "no") << "\n";
    std::cout << std::setw(20) << std::left << "Is Udp " << " => " << (this->isUdp() ? "yes" : "no") << "\n";
    std::cout << std::setw(20) << std::left << "Is Free " << " => " << (this->isFree() ? "yes" : "no") << "\n";
    std::cout << "================================================\n";
    std::cout << std::endl;

    return e_socket::succ_socket_print;
}
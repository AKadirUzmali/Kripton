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
 * 
 * ====================================================================================================
 * 
 * Güncelleme: 16/12/2025
 * 
 * Veri gönderimi yapılırken bir yapı kullanılacak.
 * Bu yapıda ilk 384 bayt (96 u32) şifre için ayrılacak,
 * sonraki 128 (32 u32) bayt gönderim yapanın ismi için ayrılacak,
 * ve en son gönderilen 2048 bayt (512 u32) ise en fazla 512 karakter
 * u32 içerebilen mesaj olacak ve bu gönderilen verilerin hepsi
 * u32 işlenip buna göre değerlendirme yapılacak. Sunucuya gönderilen
 * veri, sunucudaki anahtar ile çözülmeye başlanılacak. Eğer engellenmiş
 * ise zaten buralara kadar gelemeden verisi pas geçilecek ama eğer
 * engellenmemiş ise, şifre verisi çözümlenilecek ve şifre uyuşmaz ise
 * veri yine pas edilecek. Ek güvenlik amaçlı olarak, eğer 3 defadan fazla
 * kez şifre yanlış girilmiş ise, kişi engellenecek ve bu sayede
 * bruteforce saldırıları önlenmiş olacak. Engelleme durumu ise kişiye
 * bildirilecek ama normal kullanıcı sunucunun şifre değişimi sonrası
 * engelleme almaması için bağlantısı kesilecek ve listeden silinecek
 * bu sayede istenmeyen kullanıcı engellenmiş olacak. Anahtar verisi doğru
 * ise eğer eski şifreyi verdiği için kullanıcı, sunucu bu şifrenin kayıtlı son
 * şifre olduğunu tespit edecek ve şifre değişimi olduğunu belirten şifreli
 * bir mesaj kullanıcılara toplu şekilde iletilecek. Ama sadece en son şifre
 * kayıtlı tutulacak sunucuda, bu sayede daha önceki şifrelin tahmin edilmemesi
 * sağlanacak (ek güvenlik). Tüm bu şifre işleminden sonra anahtar eğer
 * geçersiz ise zaten şifre aşaması da başarısız olacağından sonrasında
 * anahtar ile kullanıcının isimi çözülecek ve aynı isime sahip başkaları
 * da varsa, kullanıcının isminin sonuna o isimden kaç tane kullanıcı varsa
 * o sayıdan bir fazlası parantez içine eklenerek yazılacak.
 * 
 * Örnek: client_user (21)
 * 
 * Bu sayede saldırılara karşı güvenlikli bir soket yapısı oluşacak.
 * Tabi pek akıllı kullanıcımız string işlemlerinde sınır aşımı yapamasın diye
 * bu veriler parçalara ayrılacağından güvenlik açıklarını da es geçecek
 * ve fazla veri kabul edilmediği gibi eğer eksik veri göndermişse de
 * bu veri yine değerlendirilecek ve ona göre yanıt dönülecek.
 * 
 * Örnek: 380 bayt şifre, 70 bayt isim, 3400 bayt mesaj
 * 
 * Bu gönderilen veride ilk 384 bayt alınacak ve işlencek,
 * uyuşma olmazsa zaten elenecek. 384 bayt'dan sonra sıradaki
 * 128 bayt alınacak ve işlenip kullanıcı adı olarak belirlenecek.
 * en son da kalan 2048 bayt ise alınıp mesaj olarak belirlenecek ve
 * bu sayede fazlalık bayt umursanmayacak. Eğer olurda aşırı fazla
 * bayt gönderip sunucuya saldırı yapmak isteyen olursa, en fazla bayt
 * sayısının az bir miktar üstü limiti geçen tüm herkes anında
 * engellenecek ve engelleme sebebi olarak "sunucu saldırısı" koduna
 * işaret eden bir kod olacak.
 * 
 * ====================================================================================================
 */

// Include:
#include <Global.h>

// Using Namespace:
using namespace core;
using namespace algorithmpool;

// Os Support:
#if defined __PLATFORM_DOS__
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <mswsock.h>
    #include <windows.h>
    
    using socket_t = SOCKET;
    using sockaddr_t = SOCKADDR;

    bool close_socket(socket_t _sock) { return ::closesocket(_sock) != SOCKET_ERROR; }
#elif defined __PLATFORM_POSIX__
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <unistd.h>

    using socket_t = int;
    using sockaddr_t = sockaddr;

    bool close_socket(socket_t _sock) { return ::close(_sock) == 0; }
#endif

#include <Algorithm/AlgorithmPool.h>
#include <Flag/Flag.h>
#include <Tool/Utf/Utf.h>
#include <Socket/AccessPolicy.h>
#include <File/Logger/Logger.h>
#include <Handler/Crash/CrashBase.h>
#include <Socket/NetPacket.h>

#include <iostream>
#include <vector>
#include <array>
#include <atomic>
#include <mutex>

// Namespace: Core::VirtualBase
namespace core::virbase
{
    // Namespace: Socket
    namespace socket
    {
        // Type Definition
        using socket_port_t = uint16_t;
        using socket_domain_t = int;
        using socket_conn_t = int;
        using socket_proto_t = int;
        using socket_send_t = int;
        using socket_recv_t = int;

        using wait_time_t = uint16_t;

        // Windows / DOS (Winsock)
        #if defined __PLATFORM_DOS__
            static inline constexpr socket_t inv_socket = INVALID_SOCKET;
            static inline constexpr socket_port_t inv_port = 0;
            static inline constexpr int inv_receive = SOCKET_ERROR;
            static inline constexpr int inv_send = SOCKET_ERROR;
        #elif defined __PLATFORM_POSIX__
            static inline constexpr socket_t inv_socket = -1;
            static inline constexpr socket_port_t inv_port = 0;
            static inline constexpr int inv_receive = -1;
            static inline constexpr int inv_send = -1;
        #endif

        // Using:
        using buffer_size_t = size_t;

        // Enum Class: Socket Code
        enum class e_socket : glo::status_t
        {
            unknwn = static_cast<glo::status_t>(glo::e_status_t::socket),

            err = unknwn + static_cast<glo::status_t>(glo::e_status::err),
            err_has_error,
            err_socket_invalid,
            err_socket_closed,
            err_socket_create,
            err_win_socket_create,
            err_socket_close,
            err_socket_clear,
            err_socket_send,
            err_socket_recv,
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
            err_socket_close_function,
            err_send_has_no_input,
            err_invalid_utf8,
            err_send_message_convert_fail,
            err_socket_message_empty,
            err_recv_passwd_not_correct,
            err_socket_timeout_under_limit,
            err_socket_timeout_over_limit,
            err_socket_set_timeout,
            err_client_socket_invalid,
            err_packet_no_data,
            err_send_pack_over_size,
            err_socket_send_packet,
            err_recv_socket_closed,
            err_packet_corrupt,
            err_recv_username_empty,
            err_recv_message_empty,

            succ = unknwn + static_cast<glo::status_t>(glo::e_status::succ),
            succ_socket_create,
            succ_socket_close,
            succ_socket_clear,
            succ_socket_send,
            succ_socket_recv,
            succ_socket_print,
            succ_socket_set,
            succ_socket_set_type_udp,
            succ_socket_set_type_tcp,
            succ_socket_set_algorithm,
            succ_socket_set_port,
            succ_set_buffer_size,
            succ_socket_set_timeout,

            warn = unknwn + static_cast<glo::status_t>(glo::e_status::warn),
            warn_socket_type_same,
            warn_socket_timeout_value_same,
            warn_socket_can_be_close,
            warn_send_not_all_data
        };

        constexpr bool operator==(glo::status_t _first, e_socket _second) noexcept { return _first == static_cast<glo::status_t>(_second); }
        constexpr bool operator!=(glo::status_t _first, e_socket _second) noexcept { return _first != static_cast<glo::status_t>(_second); }

        // Limit:
        static inline constexpr size_t MAX_SOCKET_RETRY_PASSWD = 4;

        static inline constexpr size_t MAX_SOCKET_BUFFER_SIZE = 8192;

        static inline constexpr size_t MIN_SOCKET_BACKLOG = 1;
        static inline constexpr size_t DEF_SOCKET_BACKLOG = 4;
        static inline constexpr size_t MAX_SOCKET_BACKLOG = 1024;

        static inline constexpr size_t MIN_SIZE_BUFFER = sizeof(char32_t);
        static inline constexpr size_t DEF_SIZE_BUFFER = 4096;
        static inline constexpr size_t MAX_SIZE_BUFFER = (uint16_t)~0;

        static inline constexpr wait_time_t MIN_SOCKET_TIMEOUT = 1;     // saniye
        static inline constexpr wait_time_t DEF_SOCKET_TIMEOUT = 5;     // saniye
        static inline constexpr wait_time_t MAX_SOCKET_TIMEOUT = 30;    // saniye

        // Flag: Socket Status
        static inline constexpr flag_t flag_socket_free { 1 << 0 };
        static inline constexpr flag_t flag_socket_created { 1 << 1 };
        static inline constexpr flag_t flag_socket_error { 1 << 2 };
        static inline constexpr flag_t flag_socket_open { 1 << 3 };

        // Namespace: Tcp
        namespace tcp
        {
            static inline constexpr socket_domain_t domain = AF_INET;
            static inline constexpr socket_conn_t type = SOCK_STREAM;
            static inline constexpr socket_proto_t protocol = IPPROTO_TCP;
        }

        // Struct: Data Packet
        typedef struct datapacket_t
        {
            std::u32string pwd;
            std::u32string name;
            std::u32string msg;
        } datapacket_t;

        // Function:
        /**
         * @brief Get IP
         * 
         * Sockete bağlantı sağlayan ip adresini
         * bulmayı sağlayarak o ip adresi ile ilgili
         * işlem yapabilmemizi sağlamasını istiyoruz
         * 
         * @return std::string
         */
        [[maybe_unused]]
        static std::string get_ip(
            const socket_t _socket
        ) noexcept
        {
            sockaddr_storage addr {};
            socklen_t len = sizeof(addr);

            if( ::getpeername(_socket, reinterpret_cast<sockaddr_t*>(&addr), &len) != 0 )
                return {};

            char ipstr[INET6_ADDRSTRLEN] {};

            if( addr.ss_family == AF_INET )
            {
                auto* s = reinterpret_cast<sockaddr_in*>(&addr);
                ::inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof(ipstr));
            }
            else if( addr.ss_family == AF_INET6 )
            {
                auto* s = reinterpret_cast<sockaddr_in6*>(&addr);
                ::inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof(ipstr));
            }

            return ipstr;
        }

        /**
         * @brief [Static] Set Socket Timeout
         * 
         * Soket için bir zamanlayıcı oluşturacağız ve
         * veri gelmemesi durumunda boş boş beklememesi için
         * bu zamanlayıcı sayesinde beklemeyi bırakıp
         * kalan işlemleri yapmaya devam edebileceğiz
         * 
         * @param socket_t Socket
         * @param uint16_t Second
         * 
         * @return bool
         */
        [[maybe_unused]]
        static bool set_socket_timeout(
            socket_t _sock,
            uint16_t _sec
        ) noexcept
        {
            if( _sock == inv_socket )
                return false;

            timeval timeval {};
            timeval.tv_sec = _sec;
            timeval.tv_usec = 0;

            ::setsockopt(_sock, SOL_SOCKET, SO_RCVTIMEO, &timeval, sizeof(timeval));
            ::setsockopt(_sock, SOL_SOCKET, SO_SNDTIMEO, &timeval, sizeof(timeval));

            return true;
        }
    }

    // Using Namespace:
    using namespace socket;

    // Struct: UserPacket
    typedef struct UserPacket_t
    {
        std::u32string username { U"" };
        uint16_t try_passwd { 0 };
        uint16_t same_user_count { 0 };
    } UserPacket_t;

    // Class: Socket
    template<class Algo>
    class Socket : virtual public handler::CrashBase
    {
        static_assert(std::is_base_of<Algorithm, Algo>::value, "<Algo> Has To Be At The Base Of The <Algorithm> Class");

        private:
            Algo algo;
            Flag flag;
            AccessPolicy policy;

            socket_t sock;
            std::atomic<socket_port_t> port;

            Logger socklog;
            std::atomic<bool> active_log;

            mutable std::mutex sock_mtx;

            std::atomic<buffer_size_t> buffer_size { DEF_SIZE_BUFFER };
            std::atomic<wait_time_t> timeout;
            
            inline static std::atomic<size_t> total_socket { 0 };

            #if defined __PLATFORM_DOS__
                inline static std::atomic<bool> wsa_started { false };
            #endif

        private:
            inline static void incTotalSocket() noexcept { ++total_socket; }
            inline static void decTotalSocket() noexcept { if( total_socket ) --total_socket; };

        public:
            explicit Socket
            (
                Algo&& _algorithm,
                const std::u32string& _nickname,
                const socket_port_t _port = inv_port,
                const bool _log = false,
                const std::u32string& _logheader = U"Socket",
                const std::u32string& _logfilename = U"socket",
                const buffer_size_t _buffsize = DEF_SIZE_BUFFER,
                const wait_time_t _timeoutsec = DEF_SOCKET_TIMEOUT
            ) noexcept;

            ~Socket() noexcept;

            inline AccessPolicy& getPolicy() noexcept;

            virtual inline bool hasError() const noexcept;

            virtual inline void enableLog() noexcept;
            virtual inline void disableLog() noexcept;
            
            virtual inline bool isLog() const noexcept;
            virtual inline bool isFree() const noexcept;
            virtual inline bool isCreate() const noexcept;
            virtual inline bool isClose() const noexcept;

            inline static size_t getTotalSocket() noexcept { return Socket::total_socket; };

            virtual inline buffer_size_t getBufferSize() const noexcept;
            virtual glo::status_t setBufferSize(const buffer_size_t = DEF_SIZE_BUFFER) noexcept;

            virtual inline wait_time_t getTimeout() const noexcept;
            virtual glo::status_t setTimeout(const wait_time_t = DEF_SOCKET_TIMEOUT) noexcept;

            virtual inline socket_t getSocket() const noexcept;
            virtual glo::status_t setSocket(const socket_t) noexcept;

            virtual inline socket_port_t getPort() const noexcept;
            virtual glo::status_t setPort(const socket_port_t) noexcept;

            virtual inline Algo& getAlgorithm() noexcept;
            virtual inline Logger& getLogger() noexcept;

            virtual glo::status_t create() noexcept;
            virtual glo::status_t close() noexcept;
            virtual glo::status_t clear() noexcept;
            
            virtual glo::status_t send(const socket_t, datapacket_t&) noexcept;
            virtual glo::status_t receive(const socket_t, datapacket_t&) noexcept;
            virtual void print() noexcept;

            virtual void onCrash() noexcept override;
    };

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
        const bool _log,
        const std::u32string& _logheader,
        const std::u32string& _logfilename,
        const buffer_size_t _buffsize,
        const wait_time_t _timeoutsec
    ) noexcept
    :   algo(std::forward<Algo>(_algorithm)),
        flag(flag_socket_free),
        sock(inv_socket),
        port(inv_port),
        socklog(_logheader, _logfilename),
        buffer_size(DEF_SIZE_BUFFER),
        timeout(_timeoutsec)
    {
        #if defined __PLATFORM_DOS__
            if( !Socket::wsa_started.load() && !total_socket )
            {
                WSAData wsadata;
                if( ::WSAStartup(MAKEWORD(2,2), &wsadata) != 0 )
                    this->flag.set(flag_socket_error);

                Socket::wsa_started.store(true);
            }
        #endif

        if( _log ) this->enableLog();
        else this->disableLog();

        this->setPort(_port);
        this->setBufferSize(_buffsize);
        this->getPolicy().setUsername(_nickname);
        this->incTotalSocket();
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
        this->decTotalSocket();

        #if defined __PLATFORM_DOS__
            if( this->wsa_started.load() && !total_socket ) {
                ::WSACleanup();
                this->wsa_started.store(false);
            }
        #endif
    }

    /**
     * @brief [Public] Get Policy
     * 
     * Erişim kontrolü durumlarına erişebilmeyi
     * sağlamak için nesneyi referans olarak döndürür
     * 
     * @return AccessPolicy&
     */
    template<class Algo>
    AccessPolicy& Socket<Algo>::getPolicy() noexcept
    {
        return this->policy;
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
     * @brief [Public] Enable Log
     * 
     * Kayıt tutup tutmamamızı belirleyen durum değişkenini
     * aktif olarak ayarlayıp kayıt tutmasını belirtiyoruz
     */
    template<class Algo>
    inline void Socket<Algo>::enableLog() noexcept
    {
        this->active_log.store(true);
    }

    /**
     * @brief [Public] Disable Log
     * 
     * Kayıt tutup tutmamamızı belirleyen durum değişkenini
     * devre dışı olarak ayarlayıp kayıt tutmamasını
     * belirtiyoruz
     */
    template<class Algo>
    inline void Socket<Algo>::disableLog() noexcept
    {
        this->active_log.store(false);
    }

    /**
     * @brief [Public] Is Log
     * 
     * Kayıt tutup tutmama durumunu döndürür
     * 
     * @return bool
     */
    template<class Algo>
    inline bool Socket<Algo>::isLog() const noexcept
    {
        return this->active_log.load();
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
        return static_cast<bool>((this->flag.get() & flag_socket_free) && (this->getSocket() == inv_socket));
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
        return static_cast<bool>((this->flag.get() & flag_socket_created) && (this->getSocket() != inv_socket));
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
        return !this->isCreate();
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
        return this->buffer_size.load();
    }

    /**
     * @brief [Public] Set Buffer Size
     * 
     * Geçici veri saklama deposu (buffer) için
     * yeni boyut ayarı yapmayı sağlamak
     * 
     * @param buffer_size_t Buffer Size
     * @return glo::status_t
     */
    template<class Algo>
    glo::status_t Socket<Algo>::setBufferSize(
        const buffer_size_t _buffsize
    ) noexcept
    {
        if( _buffsize < MIN_SIZE_BUFFER ) return glo::to_status<e_socket>(e_socket::err_buffer_size_under_limit);
        else if( _buffsize > MAX_SIZE_BUFFER ) return glo::to_status<e_socket>(e_socket::err_buffer_size_over_limit);

        this->buffer_size.store(_buffsize);
        return this->buffer_size.load() == _buffsize ?
            glo::to_status<e_socket>(e_socket::succ_set_buffer_size) :
            glo::to_status<e_socket>(e_socket::err_set_buffer_size_fail);
    }

    /**
     * @brief [Public] Get Timeout
     * 
     * Socket'den gidecek ya da gelecek veri için
     * belirlenen zaman aşımı süresinin değerini
     * döndürecek
     * 
     * @return wait_time_t
     */
    template<class Algo>
    wait_time_t Socket<Algo>::getTimeout() const noexcept
    {
        return this->timeout.load();
    }

    /**
     * @brief [Public] Set Timeout
     * 
     * Socket veri alma/gönderme zaman aşımı için
     * gerekli olan süreyi belirlenen sınırlar
     * içerisinde ayarlayacak, düzgün ve belirli
     * bir süre olması için saniye cinsinden yapıyoruz
     * 
     * @param wait_time_t Timeout
     * @return glo::status_t
     */
    template<class Algo>
    glo::status_t Socket<Algo>::setTimeout(const wait_time_t _timeout) noexcept
    {
        if( _timeout < MIN_SOCKET_TIMEOUT )
            return glo::to_status<e_socket>(e_socket::err_socket_timeout_under_limit);
        else if( _timeout > MAX_SOCKET_TIMEOUT )
            return glo::to_status<e_socket>(e_socket::err_socket_timeout_over_limit);
        else if( this->timeout.load() == _timeout )
            return glo::to_status<e_socket>(e_socket::warn_socket_timeout_value_same);

        this->timeout.store(_timeout);
        return this->timeout.load() == _timeout ?
            glo::to_status<e_socket>(e_socket::succ_socket_set_timeout) :
            glo::to_status<e_socket>(e_socket::err_socket_set_timeout);
    }

    /**
     * @brief [Public] Get Socket
     * 
     * Soket tanımlayıcısını döndürecek
     * 
     * @return socket_t Socket
     */
    template<class Algo>
    socket_t Socket<Algo>::getSocket() const noexcept
    {
        return this->sock;
    }

    /**
     * @brief [Public] Set Socket
     * 
     * Soket tanımlayıcısını belirleyecek
     * 
     * @param socket_t Socket
     * @return glo::status_t Status
     */
    template<class Algo>
    glo::status_t Socket<Algo>::setSocket(
        const socket_t _sock
    ) noexcept
    {
        {
            std::scoped_lock<std::mutex> lock(this->sock_mtx);
            this->sock = _sock;
        }

        return (_sock != inv_socket) ?
            glo::to_status<e_socket>(e_socket::succ_socket_set) :
            glo::to_status<e_socket>(e_socket::err_socket_set);
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
    socket_port_t Socket<Algo>::getPort() const noexcept
    {
        return this->port.load();
    }

    /**
     * @brief [Public] Set Port
     * 
     * Soketin port numarası geçersiz olmadığı sürece
     * güncelleme imkanı sunabilsin
     * 
     * @param socket_port_t Socket Port
     * @return glo::status_t
     */
    template<class Algo>
    glo::status_t Socket<Algo>::setPort
    (
        const socket_port_t _sockport
    ) noexcept
    {
        if( _sockport <= inv_port )
            return glo::to_status<e_socket>(e_socket::err_socket_port_invalid);

        this->port.store(_sockport);
        return this->port == _sockport ?
            glo::to_status<e_socket>(e_socket::succ_socket_set_port) :
            glo::to_status<e_socket>(e_socket::err_socket_set_port);
    }

    /**
     * @brief [Public] Get Algorithm
     * 
     * Şifreleme algoritmasını döndürecek
     *
     * @return Algo&
     */
    template<class Algo>
    Algo& Socket<Algo>::getAlgorithm() noexcept
    {
        return this->algo;
    }

    /**
     * @brief [Public] Get Logger
     * 
     * Kayıt nesnesini döndürecek
     * 
     * @return Logger&
     */
    template<class Algo>
    Logger& Socket<Algo>::getLogger() noexcept
    {
        return this->socklog;
    }

    /**
     * @brief [Public] Create
     * 
     * Soket için verilmiş olan bağlantı türüne (tcp, udp)
     * göre bağlantı oluşturmasını sağlıyoruz
     * 
     * @return glo::status_t
     */
    template<class Algo>
    glo::status_t Socket<Algo>::create() noexcept
    {
        socket_t fd = ::socket(tcp::domain, tcp::type, tcp::protocol);
        if(fd == inv_socket) {
            this->flag.set(flag_socket_error);
            return glo::to_status<e_socket>(e_socket::err_socket_invalid);
        }

        {
            std::scoped_lock<std::mutex> lock(this->sock_mtx);

            this->sock = fd;

            this->flag.set(flag_socket_created);
            this->flag.set(flag_socket_open);
            this->flag.unset(flag_socket_free);
            this->flag.unset(flag_socket_error);
        }

        return this->sock != inv_socket ?
            glo::to_status<e_socket>(e_socket::succ_socket_create) :
            glo::to_status<e_socket>(e_socket::err_socket_create);
    }

    /**
     * @brief [Public] Close
     * 
     * Socket yapısına ait bağlantıyı kontrol etsin.
     * Bağlantı yoksa hata, varsa kapatsın
     * 
     * @return glo::status_t
     */
    template<class Algo>
    glo::status_t Socket<Algo>::close() noexcept
    {
        if( this->sock == inv_socket )
            return glo::to_status<e_socket>(e_socket::err_socket_invalid);

        close_socket(this->sock);

        {
            std::scoped_lock<std::mutex> lock(this->sock_mtx);

            this->sock = inv_socket;

            this->flag.unset(flag_socket_open);
            this->flag.unset(flag_socket_created);

            this->flag.set(flag_socket_free);
        }

        return this->sock == inv_socket ?
            glo::to_status<e_socket>(e_socket::succ_socket_close) :
            glo::to_status<e_socket>(e_socket::err_socket_close);
    }

    /**
     * @brief [Public] Clear
     * 
     * Socket yapısına ait bağlantı, port,
     * bayrak değerlerini sıfırlayarak
     * temizleme yapsın
     * 
     * @return glo::status_t
     */
    template<class Algo>
    glo::status_t Socket<Algo>::clear() noexcept
    {
        this->close();

        this->port = inv_port;

        this->flag.clear();
        this->flag.set(flag_socket_free);

        return this->sock == inv_socket ?
            glo::to_status<e_socket>(e_socket::succ_socket_clear) :
            glo::to_status<e_socket>(e_socket::err_socket_clear);
    }

    template<class Algo>
    glo::status_t Socket<Algo>::send(
        const socket_t _socket,
        datapacket_t& _rawpacket
    ) noexcept
    {
        if (_socket == inv_socket)
            return glo::to_status<e_socket>(e_socket::err_client_socket_invalid);
    
        if (_rawpacket.name.empty() || _rawpacket.msg.empty())
            return glo::to_status<e_socket>(e_socket::err_packet_no_data);
    
        const std::string ipaddr = get_ip(_socket);
    
        this->getAlgorithm().encrypt(_rawpacket.pwd);
        this->getAlgorithm().encrypt(_rawpacket.name);
        this->getAlgorithm().encrypt(_rawpacket.msg);
    
        NetPacket net_packet(_rawpacket.pwd, _rawpacket.name, _rawpacket.msg);
        const auto& buffer = net_packet.get();
    
        std::size_t total_sent = 0;
        const std::size_t total_size = buffer.size();
    
        while (total_sent < total_size)
        {
            int sent = ::send(
                _socket,
                reinterpret_cast<const char*>(buffer.data()) + total_sent,
                static_cast<int>(total_size - total_sent),
                0
            );
        
            if (sent < 0)
                return glo::to_status<e_socket>(e_socket::err_socket_send_packet);
            else if (sent == 0)
                return glo::to_status<e_socket>(e_socket::warn_socket_can_be_close);
        
            total_sent += sent;
        }
    
        if (this->isLog())
        {
            LOG_MSG(
                this->getLogger(),
                U"Data sent to (" + utf::to_utf32(ipaddr) + U")",
                test::e_status::information,
                false
            );
        }
    
        return glo::to_status<e_socket>(e_socket::succ_socket_send);
    }

    template<class Algo>
    glo::status_t Socket<Algo>::receive(
        const socket_t _socket,
        datapacket_t& _rawpacket
    ) noexcept
    {
        if (_socket == inv_socket)
            return glo::to_status<e_socket>(e_socket::err_client_socket_invalid);

        constexpr std::size_t HEADER_LEN = 10;
        char header[HEADER_LEN] {};

        int r = ::recv(_socket, header, HEADER_LEN, MSG_WAITALL);
        if (r <= 0)
            return r == 0
                ? glo::to_status<e_socket>(e_socket::err_recv_socket_closed)
                : glo::to_status<e_socket>(e_socket::err_socket_recv);

        std::size_t len_pwd = 0, len_usr = 0, len_msg = 0;

        if (std::sscanf(header, "%03zu%03zu%04zu", &len_pwd, &len_usr, &len_msg) != 3)
            return glo::to_status<e_socket>(e_socket::err_packet_corrupt);

        const std::size_t payload_len = len_pwd + len_usr + len_msg;
        if (payload_len == 0)
            return glo::to_status<e_socket>(e_socket::err_packet_no_data);

        std::vector<char> payload(payload_len);

        r = ::recv(_socket, payload.data(), static_cast<int>(payload_len), MSG_WAITALL);
        if (r <= 0)
            return r == 0
                ? glo::to_status<e_socket>(e_socket::err_recv_socket_closed)
                : glo::to_status<e_socket>(e_socket::err_socket_recv);

        std::size_t offset = 0;

        std::string pwd(payload.data() + offset, len_pwd);
        offset += len_pwd;

        std::string usr(payload.data() + offset, len_usr);
        offset += len_usr;

        std::string msg(payload.data() + offset, len_msg);

        _rawpacket.pwd  = utf::to_utf32(pwd);
        _rawpacket.name = utf::to_utf32(usr);
        _rawpacket.msg  = utf::to_utf32(msg);

        this->getAlgorithm().decrypt(_rawpacket.pwd);
        this->getAlgorithm().decrypt(_rawpacket.name);
        this->getAlgorithm().decrypt(_rawpacket.msg);

        if( _rawpacket.name.empty() )
            return glo::to_status<e_socket>(e_socket::err_recv_username_empty);
        else if( _rawpacket.msg.empty() )
            return glo::to_status<e_socket>(e_socket::err_recv_message_empty);

        if ( this->isLog() )
        {
            const std::string ipaddr = get_ip(_socket);
            LOG_MSG(
                this->getLogger(),
                U"Data received from (" + utf::to_utf32(ipaddr) + U")",
                test::e_status::information,
                false
            );
        }

        return glo::to_status<e_socket>(e_socket::succ_socket_recv);
    }

    /**
     * @brief [Public] Print
     * 
     * Socket yapısına ait bilgileri
     * konsola yazdırarak durumu
     * hakkında bilgi versin
     */
    template<class Algo>
    void Socket<Algo>::print() noexcept
    {
        std::cout << "\n==================== TCP SOCKET ====================\n";
        std::cout << std::setw(20) << std::left << "Socket " << std::right << " => " << (this->sock != inv_socket ? "Valid" : "Invalid") << "\n";
        std::cout << std::setw(20) << std::left << "Has Error " << std::right << " => " << (this->hasError() ? "yes" : "no") << "\n";
        std::cout << std::setw(20) << std::left << "Is Created " << std::right << " => " << (this->isCreate() ? "yes" : "no") << "\n";
        std::cout << std::setw(20) << std::left << "Is Free " << std::right << " => " << (this->isFree() ? "yes" : "no") << "\n\n";
    }

    /**
     * @brief [Public] On Crash
     * 
     * Çökme durumunda soket bağlantısını
     * güvenli bir şekilde kapatmayı sağlar
     */
    template<class Algo>
    void Socket<Algo>::onCrash() noexcept
    {
        this->close();

        #if defined __PLATFORM_DOS__
            if( this->wsa_started.load() && total_socket ) {
                ::WSACleanup();
                this->wsa_started.store(false);
            }
        #endif
    }
}
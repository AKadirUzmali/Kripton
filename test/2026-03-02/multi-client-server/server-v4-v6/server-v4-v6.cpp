// Abdulkadir U. - 2026/03/02

/**
 * Server v4/v6 (Sunucu v4/v6)
 * 
 * Geliştirdiğimiz sunucu ipv4/v6 testlerini başarıyla geçti.
 * Şimdi ise, sunucu çalıştırılırken istenilen sürüm seçilip
 * ona göre çalıştırılabilir hale getirilecektir.
 * Çoklu istemci bağlantısı yapılarak sunucu yükü test edilecektir.
 * 
 * 
 * Normalde test amaçlı geliştirdiğimiz istemci
 * bağlantı sağlar, veri gönderir ve sonlanırdı ama
 * biz bunu bir döngü halinde yapacağız ve çoklu istemci
 * testini kontrol edeceğiz. 10 saniye içinde her saniyede
 * 1 kez veri alış-verişi yapacak sunucu ile ve süre
 * bitiminde sonlanacak. Saniye içindeki veri akışı,
 * bu testten sonra ek 1 test ile daha yapılacak
 * ve saniyede 100 kez veri gönderecek fakat kayıtların
 * okunmasını zorlaştırmamak adına farklı olarak yapılacaktır.
 * 
 * Burada yapılması amaçlananlar:
 * 
 * 1) Sunucunun birden çok istemciye karşı direnci
 * 2) Sunucunun toplam çekirdek miktarından fazla istemci ile dayanımı
 * 3) Sunucu performans analizi
 * 4) Aynı ip adres max limiti testi
 * 5) Toplam bağlantı limit testi
 * 6) Aynı isime sahip istemcileri isimlendirme testi
 * 7) Çoklu istemci sırasında gönderilen veriye düzgünce erişilebilinecek mi testi
 * 
 * Bunlar hem sunucu için hem de istemci için (tam uygun olmasa da)
 * yapabileceğimiz testlerdir.
 * 
 * Derleme:
 *  Bsd     :: g++ -I../../../../include -std=c++17 -Wall -Werror -Wextra server-v4-v6.cpp -pthread -o bsd/server-v4-v6.bsd
 *  Linux   :: g++ -I../../../../include -std=c++17 -Wall -Werror -Wextra server-v4-v6.cpp -o linux/server-v4-v6.linux
 *  Windows :: g++ -I../../../../include -std=c++17 -Wall -Werror -Wextra server-v4-v6.cpp -o windows/server-v4-v6.exe -lws2_32
 * 
 * Çalıştırma:
 *  Bsd     :: ./bsd/server-v4-v6.bsd <ip version>
 *  Linux   :: ./linux/server-v4-v6.linux <ip version>
 *  Windows :: ./windows/server-v4-v6.exe <ip version>
 * 
 * Örnek:
 *  Bsd     :: ./bsd/server-v4-v6.bsd v6
 *  Linux   :: ./linux/server-v4-v6.linux v4
 *  Windows :: ./windows/server-v4-v6.exe v4
 */

// Include
#define __BUILD_TYPE__ built_t::Debug
#define __SIGNAL_DETECT__ SigInterrupt | SigAbort | SigIllegal | SigSegFault | SigTerminate
#include <kits/corekit.hpp>
#include <kits/hashkit.hpp>
#include <kits/outputkit.hpp>
#include <kits/toolkit.hpp>

#include <dev/developer.hpp>

#include <pool/cryptopool.hpp>
#include <pool/threadpool.hpp>

#include <socket/policy.hpp>
#include <socket/netpacket.hpp>
#include <socket/socket.hpp>

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

using namespace pool::cryptopool;
using namespace pool::threadpool;

using namespace netsocket;

// Define
#if __OS_WINDOWS__
    static constexpr int ss_inv_accept = SOCKET_ERROR;
    static constexpr int ss_inv_bind = -1;
#elif __OS_POSIX__
    static constexpr int ss_inv_accept = -1;
    static constexpr int ss_inv_bind = -1;
#endif

// Static
static constexpr Version ss_ver(0, 8, 4);
static constexpr Vch<16> ss_verhash("20260302|test|client|loop", 20260302);

static const std::string ss_osname = utf::to_lower(current_os_name());
static const std::string ss_logname = "server-loop-" + ss_osname;

static ThreadPool ss_tpool;

// Function
template<typename CipherT>
Status server_initializer(
    std::atomic<bool>& ar_running,
    Socket<CipherT>& ar_server
) noexcept;

template<typename CipherT>
void server_listener(
    const std::atomic<bool>& ar_running,
    std::unordered_map<socket_t, SocketCtx>& ar_clients,
    Socket<CipherT>& ar_server
) noexcept;

template<typename CipherT>
void client_work(
    const std::atomic<bool>& ar_running,
    const socket_t ar_cli_socket,
    const std::string ar_ipaddr,
    const SocketCtx& ar_client,
    Socket<CipherT>& ar_server
) noexcept;

/**
 * @brief main
 * 
 * @param ipv_t Ip Version
 */
int main(int argc, char* argv[])
{
    if( argc < 2 )
        return EXIT_FAILURE;

    using CipherEnc = Xor;

    Logger<FileOut, ConsoleOut> vv_servlog("logs/" + ss_logname, "console-" + ss_logname);
    Crypto<CipherEnc> vv_cipher("loop-server-cipher", utf::to_utf8(U"key-cipher-server-loop@20260302"));
    
    // VALID IP VERSION?
    const std::string vv_ipv(argv[1]);
    if( vv_ipv.compare("v4") != 0 && vv_ipv.compare("v6") != 0 )
    {
        vv_servlog.write(level_t::Err, vv_ipv + " ip version is not valid, not v4/v6", GET_SOURCE);
        return EXIT_FAILURE;
    }
    const ipv_t vv_iptype = vv_ipv.compare("v4") ? ipv_t::ipv4 : ipv_t::ipv6;
    
    // SOCKET
    Socket<CipherEnc> vv_server(vv_cipher,
        "logs/socket-" + ss_logname,
        "netsocket-server",
        "pwd@server.loop!test",
        2026,
        vv_iptype,
        _FLAG_SOCKET_LOGGER | _FLAG_SOCKET_LOG_TITLE
    );

    // STATUS
    Status vv_status;

    // MAX CONNECTION, MAX SAME IP
    const policy::max_conn_t vv_max_conn = 4;
    const policy::max_conn_t vv_max_same_ip = 3;

    vv_status = vv_server.get_policy().set_max_connection(vv_max_conn);
    vv_servlog.write(
        vv_status.is_ok() ? level_t::Succ : level_t::Err,
        "New Max Connection Limit is: " + std::to_string(vv_server.get_policy().get_max_connection()),
        GET_SOURCE);

    vv_status = vv_server.get_policy().set_max_same_ip(vv_max_same_ip);
    vv_servlog.write(
        vv_status.is_ok() ? level_t::Succ : level_t::Err,
        "New Max Same Ip Limit is: " + std::to_string(vv_server.get_policy().get_max_same_ip()),
        GET_SOURCE);

    // CLIENTS
    std::unordered_map<socket_t, SocketCtx> vv_clients;

    // SERVER INITIALIZER
    std::atomic<bool> vv_running = false;
    vv_status = server_initializer(vv_running, vv_server);
    if( !vv_status.is_ok() ) {
        vv_running.store(false);
        vv_servlog.write(level_t::Err, "Ipv6 Server Initialize Failed. Error Code: " + std::to_string(vv_status.get_code()), GET_SOURCE);
        
        return EXIT_FAILURE;
    }

    vv_servlog.write(level_t::Succ, "Ipv6 Server Succesfully Initalized", GET_SOURCE);

    // SERVER LISTENER
    server_listener<CipherEnc>(vv_running, vv_clients, vv_server);

    // PRINT
    vv_servlog.print();

    return EXIT_SUCCESS;
}

/**
 * @brief Server Initializer
 * 
 * @param atomic<bool>& Running
 * @tparam Socket<CipherT>& Server
 * 
 * @return Status
 */
template<typename CipherT>
Status server_initializer(
    std::atomic<bool>& ar_running,
    Socket<CipherT>& ar_server
) noexcept
{
    // CREATE
    Status tm_status = ar_server.create();
    if( !tm_status.is_ok() )
        return Status::err(domain_t::server, tm_status.get_code());

    // BIND
    sockaddr* tm_addr = nullptr;
    socklen_t tm_len_addr = 0;

    switch( ar_server.get_ipv() ) {
        case ipv_t::ipv6: {
            sockaddr_in6 tm_addrv6 {};
            tm_addrv6.sin6_family = tcp::ipv6::domain;
            tm_addrv6.sin6_addr = in6addr_any;
            tm_addrv6.sin6_port = htons(ar_server.get_port());

            tm_addr = reinterpret_cast<sockaddr*>(&tm_addrv6);
            tm_len_addr = sizeof(tm_addrv6);

            if( ar_server.get_flag().has(_FLAG_SOCKET_LOGGER) )
                ar_server.get_logger().write(level_t::Info, "Ip type is v6", GET_SOURCE);

            break;
        }
        case ipv_t::ipv4:
        default: {
            sockaddr_in tm_addrv4 {};
            tm_addrv4.sin_family = tcp::ipv4::domain;
            tm_addrv4.sin_addr.s_addr = INADDR_ANY;
            tm_addrv4.sin_port = htons(ar_server.get_port());

            tm_addr = reinterpret_cast<sockaddr*>(&tm_addrv4);
            tm_len_addr = sizeof(tm_addrv4);

            if( ar_server.get_flag().has(_FLAG_SOCKET_LOGGER) )
                ar_server.get_logger().write(level_t::Info, "Ip type is v4", GET_SOURCE);

            break;
        }
    }

    int tm_res = ::bind(ar_server.get_socket(), tm_addr, tm_len_addr);
    if( tm_res == ss_inv_bind )
        return Status::err(domain_t::server, 11000);

    if( ar_server.get_flag().has(_FLAG_SOCKET_LOGGER) )
        ar_server.get_logger().write(level_t::Info, "Server binded", GET_SOURCE);

    // LISTEN
    int tm_backlog = static_cast<int>(ar_server.get_policy().get_max_connection());
    if( ::listen(ar_server.get_socket(), tm_backlog) != 0 )
        return Status::err(domain_t::server, 11001);

    if( ar_server.get_flag().has(_FLAG_SOCKET_LOGGER) )
        ar_server.get_logger().write(level_t::Info, "Server backlog: " + std::to_string(tm_backlog), GET_SOURCE);

    // SET RUNNING
    ar_running.store(true);

    // TIMEOUT
    #if __OS_POSIX__
        timeval tm_val {};
        tm_val.tv_sec = 5;
        tm_val.tv_usec = 0;

        setsockopt(ar_server.get_socket(), SOL_SOCKET, SO_RCVTIMEO, &tm_val, sizeof(tm_val));
        setsockopt(ar_server.get_socket(), SOL_SOCKET, SO_SNDTIMEO, &tm_val, sizeof(tm_val));
    #elif __OS_WINDOWS__
        DWORD timeout tm_val = 5000;

        setsockopt(ar_server.get_socket(), SOL_SOCKET, SO_RCVTIMEO, (const char*)&tm_val, sizeof(tm_val));
        setsockopt(ar_server.get_socket(), SOL_SOCKET, SO_SNDTIMEO, (const char*)&tm_val, sizeof(tm_val));
    #endif

    if( ar_server.get_flag().has(_FLAG_SOCKET_LOGGER) )
        ar_server.get_logger().write(level_t::Succ, "Server running", GET_SOURCE);

    return Status::ok(domain_t::server, 12000);
}

/**
 * @brief Server Listener
 * 
 * Sunucuyu dinleme işlemi yapacak olan fonksiyondur.
 * Çoklu çekirdek işlemi olacağı için işlem numarasını
 * da argüman olarak alır. Bağlantı yapılacak Socket
 * ve Kayıt yapılacak nesneyi alır. Yapılan işlemleri
 * numara ve tarih ile çıktı verir.
 * 
 * Burada çalışıp çalışmadığını kontrol eden kontrolcüyü
 * her döngüde kontrol etmesini istemedik çünkü bu bir
 * sürekli zaman aşımı demek ama onun ana fonksiyonda
 * bunu kontrol edip ona göre davranmak performans için
 * daha iyi olacaktır. Sürekli işlemci gücü tüketmemesi için
 * de döngülere bekleme süresi eklendi
 * 
 * @param atomic<bool>& Running
 * @param unordered_map<socket_t, SocketCtx>& Clients
 * @tparam Socket<CipherT>& Server
 */
template<typename CipherT>
void server_listener(
    const std::atomic<bool>& ar_running,
    std::unordered_map<socket_t, SocketCtx>& ar_clients,
    Socket<CipherT>& ar_server
) noexcept
{
    // IS VALID SOCKET
    if( !is_valid_socket(ar_server.get_socket()) )
        return;

    // SOCKET
    const socket_t tm_sock = ar_server.get_socket();

    // IP STR
    std::string tm_ipaddr;
    tm_ipaddr.reserve(64);

    // SOCKET STARTING TO RUN
    while( ar_running.load() && !CrashHandler::is_signal() )
    {
        // ACCEPT
        sockaddr_storage tm_sockstore {};
        socklen_t tm_sock_len = sizeof(tm_sockstore);
        socket_t tm_cli_accpt = ::accept(tm_sock, reinterpret_cast<sockaddr*>(&tm_sockstore), &tm_sock_len);

        // IS VALID SOCKET
        if( !is_valid_socket(tm_cli_accpt) ) {
            // FOR TEST, SERVER FINISHER
            static int tm_total = 0;
            ++tm_total;

            if( tm_total > 3 )
                return;

            if( ar_server.get_flag().has(_FLAG_SOCKET_LOGGER) )
                ar_server.get_logger().write(level_t::Warn, "Socket is not valid", GET_SOURCE);

            continue;
        }

        // CLIENT IP
        tm_ipaddr.clear();
        tm_ipaddr = get_ip(tm_cli_accpt);

        // SOCKET VERSION HASH VERIFY
        Status tm_handshake = handshake_recv_verify(tm_cli_accpt, false);
        if( !tm_handshake.is_ok() ) {
            if( ar_server.get_flag().has(_FLAG_SOCKET_LOGGER) )
                ar_server.get_logger().write(level_t::Warn, tm_ipaddr + " version hash didn't match | code: " + std::to_string(tm_handshake.get_code()), GET_SOURCE);

            tm_handshake = handshake_send_verify(tm_cli_accpt);
            if( !tm_handshake.is_ok() ) {
                if( ar_server.get_flag().has(_FLAG_SOCKET_LOGGER) )
                    ar_server.get_logger().write(level_t::Warn, tm_ipaddr + " recv version hash sent for warning | code: " + std::to_string(tm_handshake.get_code()), GET_SOURCE);
            }

            continue;
        }

        // SOCKET VERSION HASH SEND VERIFY
        tm_handshake = handshake_send_verify(tm_cli_accpt);
        if( !tm_handshake.is_ok() ) {
            if( ar_server.get_flag().has(_FLAG_SOCKET_LOGGER) )
                ar_server.get_logger().write(level_t::Warn, tm_ipaddr + " version hash sent for warning | code: " + std::to_string(tm_handshake.get_code()), GET_SOURCE);

            continue;
        }

        if( ar_server.get_flag().has(_FLAG_SOCKET_LOGGER) )
            ar_server.get_logger().write(level_t::Succ, tm_ipaddr + " Handshake Verified", GET_SOURCE);

        // BANNED OR ALLOWED ?
        Status tm_allow = ar_server.get_policy().can_allow(tm_ipaddr);
        if( !tm_allow.is_ok() )
        {
            switch(tm_allow.get_code())
            {
                case status::to_underlying(policy::policy_e::not_allow_ipaddr_already_banned):
                    if( ar_server.get_flag().has(_FLAG_SOCKET_LOGGER) )
                        ar_server.get_logger().write(level_t::Succ, tm_ipaddr + " ip address banned", GET_SOURCE);
                    break;
                
                case status::to_underlying(policy::policy_e::not_allow_ipaddr_not_in_list):
                    if( ar_server.get_flag().has(_FLAG_SOCKET_LOGGER) )
                        ar_server.get_logger().write(level_t::Succ, tm_ipaddr + " ip address not in the allowed ips", GET_SOURCE);
                    break;
            }

            close_socket(tm_cli_accpt);
            continue;
        }

        // SAME IP
        size_t tm_count_same_ip = 0;
        for(const auto& [sock, ctx] : ar_clients)
        {
            if(ctx.m_ip == tm_ipaddr)
                ++tm_count_same_ip;
        }

        if( ar_clients.size() >= ar_server.get_policy().get_max_connection() )
        {
            if( ar_server.get_flag().has(_FLAG_SOCKET_LOGGER) )
                ar_server.get_logger().write(level_t::Warn, "Over total connection limit", GET_SOURCE);

            close_socket(tm_cli_accpt);
            continue;
        }
        else if( tm_count_same_ip >= ar_server.get_policy().get_max_same_ip() )
        {
            if( ar_server.get_flag().has(_FLAG_SOCKET_LOGGER) )
                ar_server.get_logger().write(level_t::Warn, "Over total same ip limit", GET_SOURCE);

            close_socket(tm_cli_accpt);
            continue;
        }

        ar_clients.emplace(tm_cli_accpt, SocketCtx{ tm_ipaddr, UserPacket{} });

        if( ar_server.get_flag().has(_FLAG_SOCKET_LOGGER) )
            ar_server.get_logger().write(level_t::Info, tm_ipaddr + " added client list", GET_SOURCE);

        ss_tpool.enqueue([&, tm_cli_accpt, tm_ipaddr]{
            auto tm_it = ar_clients.find(tm_cli_accpt);
            if( tm_it == ar_clients.end() )
                return;

            SocketCtx& tm_client = tm_it->second;
            tm_client.m_user.m_same_user_count = tm_count_same_ip;

            client_work<CipherT>(ar_running, tm_cli_accpt, tm_ipaddr, tm_client, ar_server);

            close_socket(tm_cli_accpt);

            if( ar_server.get_flag().has(_FLAG_SOCKET_LOGGER) )
            {
                ar_server.get_logger().write(
                    level_t::Warn,
                    "Socket closed | " + tm_client.m_ip + '_' + std::to_string(tm_client.m_user.m_same_user_count),
                    GET_SOURCE
                );
            }

            ar_clients.erase(tm_cli_accpt);
        });
    }
}

/**
 * @brief Client Work
 * 
 * Sunucu da verilen istemci ile ilgili veri alma
 * ve veri gönderme işlerinin yönetimini sağlayacak
 * 
 * @param atomic<bool> Running
 * @param socket_t Client Socket
 * @param string IP
 * @param SocketCtx& Client
 * @param Socket<CipherT>& Server
 */
template<typename CipherT>
void client_work(
    const std::atomic<bool>& ar_running,
    const socket_t ar_cli_socket,
    const std::string ar_ipaddr,
    const SocketCtx& ar_client,
    Socket<CipherT>& ar_server
) noexcept
{
    const auto tm_timeout = std::chrono::seconds(ar_server.get_timeout());
    const std::string tm_msg_timeout = " idle timeout over limit (" + std::to_string(ar_server.get_timeout()) + ")";

    // RUNNING
    while( ar_running.load(std::memory_order_relaxed) )
    {
        fd_set tm_readfs;
        FD_ZERO(&tm_readfs);
        FD_SET(ar_cli_socket, &tm_readfs);

        timeval tm_tv {};
        tm_tv.tv_sec = tm_timeout.count();
        tm_tv.tv_usec = 0;

        // SELECT
        int tm_ready = select(ar_cli_socket + 1, &tm_readfs, nullptr, nullptr, &tm_tv);
        if( tm_ready <= 0 )
            return;

        if( !FD_ISSET(ar_cli_socket, &tm_readfs) )
            return;

        // BAN CHECK
        if( ar_server.get_policy().is_connection_banned(ar_ipaddr) ) {
            if( ar_server.get_flag().has(_FLAG_SOCKET_LOGGER) )
                ar_server.get_logger().write(level_t::Warn, ar_ipaddr + " is banned", GET_SOURCE);
            return;
        }

        // DATA PACKET & RECV
        DataPacket tm_datapack;
        Status tm_status = ar_server.recv(ar_cli_socket, tm_datapack);

        // RECV STATUS
        switch( tm_status.get_code() ) {
            case status::to_underlying(socket_code_t::socket_data_recv):            
                if( ar_server.get_flag().has(_FLAG_SOCKET_LOGGER) )
                    ar_server.get_logger().write(level_t::Warn, "Data received from " + ar_ipaddr, GET_SOURCE);
                break;

            case status::to_underlying(socket_code_t::packet_no_data):
                if( ar_server.get_flag().has(_FLAG_SOCKET_LOGGER) )
                    ar_server.get_logger().write(level_t::Warn, "Packet no data from " + ar_ipaddr, GET_SOURCE);
                break;

            default:
                if( ar_server.get_flag().has(_FLAG_SOCKET_LOGGER) )
                    ar_server.get_logger().write(level_t::Warn, "data couldn't receive from " + ar_ipaddr + " | code: " + std::to_string(tm_status.get_code()), GET_SOURCE);
                return;
        }

        // LOG THE DATA
        ar_server.get_logger().write(level_t::Text, "Password: " + tm_datapack.m_pwd, GET_SOURCE);
        ar_server.get_logger().write(level_t::Text, "Id: " + ar_client.m_ip + '_' + std::to_string(ar_client.m_user.m_same_user_count), GET_SOURCE);
        ar_server.get_logger().write(level_t::Text, "Username: " + tm_datapack.m_name, GET_SOURCE);
        ar_server.get_logger().write(level_t::Text, "Message: " + tm_datapack.m_msg, GET_SOURCE);

        // SEND
        tm_status = ar_server.send(ar_cli_socket, tm_datapack);

        // SEND STATUS
        switch( tm_status.get_code() )
        {
            case status::to_underlying(socket_code_t::socket_data_sent):
                if( ar_server.get_flag().has(_FLAG_SOCKET_LOGGER) )
                    ar_server.get_logger().write(level_t::Warn, "Data sent to " + ar_ipaddr, GET_SOURCE);
                break;

            default:
                if( ar_server.get_flag().has(_FLAG_SOCKET_LOGGER) )
                    ar_server.get_logger().write(level_t::Warn, "data couldn't send to " + ar_ipaddr + " | code: " + std::to_string(tm_status.get_code()), GET_SOURCE);
                return;
        }
    }
}
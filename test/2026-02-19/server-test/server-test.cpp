// Abdulkadir U. - 2026/02/19

/**
 * Server Test (Sunucu Testi)
 * 
 * Sunucu için gerekli olanları test ederek olası hataları
 * ve sorunları giderip, bir sunucunun olması gerektiği gibi
 * çalışmasını test edeceğiz. Gerektiği durumda performans
 * optimizasyonları da gözden geçirilecektir.
 * 
 * Sunucu denilen yapı farklı sürümler içerebilmektedir.
 * Bunu test etmek için ilk önce istemciyle olan sürüm kontrolü
 * yapılır, aksi halde veri alış-verişi kısmında olabilecek
 * çok hata bulunmakta. Program çökmeyebilir fakat veriler
 * bozuk olarak iletilmiş olabilir. Bunun dışında şifreli mesaj
 * gönderimini test edecek. Veri aldıktan sonra veriyi okuyup değerlendirme
 * ve bunun dışında çoklu çekirdek testi ile yapılacaktır.
 * 
 * Derleme:
 *  Bsd     :: g++ -I../../../include -std=c++17 -Wall -Werror -Wextra server-test.cpp -pthread -o bsd/server-test.bsd
 *  Linux   :: g++ -I../../../include -std=c++17 -Wall -Werror -Wextra server-test.cpp -o linux/server-test.linux
 *  Windows :: g++ -I../../../include -std=c++17 -Wall -Werror -Wextra server-test.cpp -o windows/server-test.exe -lws2_32
 * 
 * Çalıştırma:
 *  Bsd     :: ./bsd/server-test.bsd
 *  Linux   :: ./linux/server-test.linux
 *  Windows :: ./windows/server-test.exe
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
static constexpr Version ss_ver(0, 8, 0);
static constexpr Vch<16> ss_verhash("20260219|test|server|080", 20260219);

static const std::string ss_osname = utf::to_lower(current_os_name());
static const std::string ss_logname = "server-test-" + ss_osname;

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
    const bool ar_running,
    const socket_t ar_cli_socket,
    const std::string ar_ipaddr,
    std::unordered_map<socket_t, SocketCtx>& ar_clients,
    Socket<CipherT>& ar_server
) noexcept;

// main
int main(void)
{
    using CipherEnc = Xor;

    Logger<FileOut, ConsoleOut> vv_servlog("logs/" + ss_logname, "console-" + ss_logname);
    Crypto<CipherEnc> vv_cipher("test-server-cipher", utf::to_utf8(U"key-cipher-server@20260219"));
    Socket<CipherEnc> vv_server(vv_cipher, "logs/socket-" + ss_logname, "netsocket-server", "pwd@server!test", 2026, ipv_t::ipv4, _FLAG_SOCKET_LOGGER | _FLAG_SOCKET_LOG_TITLE);

    std::unordered_map<socket_t, SocketCtx> vv_clients;

    // SERVER INITIALIZER
    std::atomic<bool> vv_running = false;
    Status vv_status = server_initializer(vv_running, vv_server);
    if( !vv_status.is_ok() ) {
        vv_running.store(false);
        vv_servlog.write(level_t::Err, "Server Initialize Failed. Error Code: " + std::to_string(vv_status.get_code()), GET_SOURCE);
        
        return EXIT_FAILURE;
    }

    vv_servlog.write(level_t::Succ, "Server Succesfully Initalized", GET_SOURCE);

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

        // SAME IP CHECK
        {
            size_t tm_count_same_ip = 0;

            for( const auto& [tm_sock, tm_ctx] : ar_clients) {
                if( tm_ctx.m_ip == tm_ipaddr )
                    ++tm_count_same_ip;

                if( tm_count_same_ip > ar_server.get_policy().get_max_same_ip() ||
                    ar_clients.size() >= ar_server.get_policy().get_max_connection() )
                {
                    if( ar_server.get_flag().has(_FLAG_SOCKET_LOGGER) )
                        ar_server.get_logger().write(level_t::Warn, "Over total connection and same ip connection", GET_SOURCE);

                    close_socket(tm_cli_accpt);
                    continue;
                }

                ar_clients.emplace(tm_cli_accpt, SocketCtx{ tm_ipaddr, UserPacket{} });

                if( ar_server.get_flag().has(_FLAG_SOCKET_LOGGER) )
                    ar_server.get_logger().write(level_t::Info, tm_ipaddr + " added client list", GET_SOURCE);
            }
        }

        ss_tpool.enqueue([&, tm_cli_accpt, tm_ipaddr]{
            client_work<CipherT>(ar_running.load(std::memory_order_relaxed), tm_cli_accpt, tm_ipaddr, ar_clients, ar_server);

            if( ar_server.get_flag().has(_FLAG_SOCKET_LOGGER) )
                ar_server.get_logger().write(level_t::Info, tm_ipaddr + " enqueued | client work", GET_SOURCE);
        });
    }
}

/**
 * @brief Client Work
 * 
 * Sunucu da verilen istemci ile ilgili veri alma
 * ve veri gönderme işlerinin yönetimini sağlayacak
 * 
 * @param bool Running
 * @param socket_t Client Socket
 * @param string IP
 * @param unordered_map<socket_t, SocketCtx>& Clients
 * @param Socket<CipherT>& Server
 */
template<typename CipherT>
void client_work(
    const bool ar_running,
    const socket_t ar_cli_socket,
    const std::string ar_ipaddr,
    std::unordered_map<socket_t, SocketCtx>& ar_clients,
    Socket<CipherT>& ar_server
) noexcept
{
    auto cleanup = [&]{
        close_socket(ar_cli_socket);

        if( ar_server.get_flag().has(_FLAG_SOCKET_LOGGER) )
            ar_server.get_logger().write(level_t::Warn, "Socket closed", GET_SOURCE);

        ar_clients.erase(ar_cli_socket);
        return;
    }; 

    if( !ar_running )
        return;

    const auto tm_timeout = std::chrono::seconds(ar_server.get_timeout());
    const std::string tm_msg_timeout = " idle timeout over limit (" + std::to_string(ar_server.get_timeout()) + ")";

    // RUNNING
    fd_set tm_readfs;
    FD_ZERO(&tm_readfs);
    FD_SET(ar_cli_socket, &tm_readfs);

    timeval tm_tv {};
    tm_tv.tv_sec = tm_timeout.count();
    tm_tv.tv_usec = 0;

    // SELECT
    int tm_ready = select(ar_cli_socket + 1, &tm_readfs, nullptr, nullptr, &tm_tv);
    if( tm_ready <= 0 )
        cleanup();

    if( !FD_ISSET(ar_cli_socket, &tm_readfs) )
        cleanup();

    // BAN CHECK
    if( ar_server.get_policy().is_connection_banned(ar_ipaddr) ) {
        if( ar_server.get_flag().has(_FLAG_SOCKET_LOGGER) )
            ar_server.get_logger().write(level_t::Warn, ar_ipaddr + " is banned", GET_SOURCE);
        cleanup();
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
            cleanup();
    }

    // LOG THE DATA
    ar_server.get_logger().write(level_t::Text, "Password: " + tm_datapack.m_pwd, GET_SOURCE);
    ar_server.get_logger().write(level_t::Text, "Username: " + tm_datapack.m_name, GET_SOURCE);
    ar_server.get_logger().write(level_t::Text, "Message: " + tm_datapack.m_msg, GET_SOURCE);

    // SEND
    tm_status = ar_server.send(ar_cli_socket, tm_datapack);

    // SEND STATUS
    switch( tm_status.get_code() ) {
        case status::to_underlying(socket_code_t::socket_data_sent):
            if( ar_server.get_flag().has(_FLAG_SOCKET_LOGGER) )
                ar_server.get_logger().write(level_t::Warn, "Data sent to " + ar_ipaddr, GET_SOURCE);
            return;
        default:
            if( ar_server.get_flag().has(_FLAG_SOCKET_LOGGER) )
                ar_server.get_logger().write(level_t::Warn, "data couldn't send to " + ar_ipaddr + " | code: " + std::to_string(tm_status.get_code()), GET_SOURCE);
            cleanup();
    }
}
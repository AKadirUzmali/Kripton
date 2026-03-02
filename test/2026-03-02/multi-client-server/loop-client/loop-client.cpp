// Abdulkadir U. - 2026/03/02

/**
 * Loop Client (Döngü İstemci)
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
 *  Bsd     :: g++ -I../../../../include -std=c++17 -Wall -Werror -Wextra loop-client.cpp -pthread -o bsd/loop-client.bsd
 *  Linux   :: g++ -I../../../../include -std=c++17 -Wall -Werror -Wextra loop-client.cpp -o linux/loop-client.linux
 *  Windows :: g++ -I../../../../include -std=c++17 -Wall -Werror -Wextra loop-client.cpp -o windows/loop-client.exe -lws2_32
 * 
 * Çalıştırma:
 *  Bsd     :: ./bsd/loop-client.bsd <ip version> <ip address>
 *  Linux   :: ./linux/loop-client.linux <ip version> <ip address>
 *  Windows :: ./windows/loop-client.exe <ip version> <ip address>
 * 
 * Örnek:
 *  Bsd     :: ./bsd/loop-client.bsd v6 ::1
 *  Linux   :: ./linux/loop-client.linux v4 127.0.0.1
 *  Windows :: ./windows/loop-client.exe v4 192.168.1.108
 */

// Include
#include <atomic>
#include <mutex>
#include <algorithm>
#include <unordered_map>
#include <thread>

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
using namespace core::status;

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
    static constexpr int ss_inv_connect = SOCKET_ERROR;
#elif __OS_POSIX__
    static constexpr int ss_inv_connect = -1;
#endif

// Static
static constexpr Version ss_ver(0, 8, 4);
static constexpr Vch<16> ss_verhash("20260302|test|client|loop", 20260302);

static const std::string ss_osname = utf::to_lower(current_os_name());
static const std::string ss_logname = "client-loop-" + ss_osname;

// Function Define
template<typename CipherT>
Status client_session(
    Socket<CipherT>& ar_sock,
    const std::string& ar_ip
) noexcept;

/**
 * @brief main
 * 
 * @param ipv_t Ip Version
 * @param string Ip Address
 */
int main(int argc, char* argv[])
{
    if( argc < 3 )
        return EXIT_FAILURE;

    using CipherEnc = Xor;

    Logger<FileOut, ConsoleOut> vv_clilog("logs/" + ss_logname, "console-" + ss_logname);
    Crypto<CipherEnc> vv_cipher("loop-client-cipher", utf::to_utf8(U"key-cipher-server-loop@20260302"));

    // VALID IP VERSION?
    const std::string vv_ipv(argv[1]);
    if( vv_ipv.compare("v4") != 0 && vv_ipv.compare("v6") != 0 )
    {
        vv_clilog.write(level_t::Err, vv_ipv + " ip version is not valid, not v4/v6", GET_SOURCE);
        return EXIT_FAILURE;
    }
    const ipv_t vv_iptype = vv_ipv.compare("v4") == 0 ? ipv_t::ipv4 : ipv_t::ipv6;

    // VALID IP?
    const std::string vv_ip(argv[2]);
    if( vv_iptype == ipv_t::ipv4 && !is_valid_ipv4(vv_ip) ) {
        vv_clilog.write(level_t::Err, vv_ip + " ip address is not valid ipv4", GET_SOURCE);
        return EXIT_FAILURE;
    }
    else if( vv_iptype == ipv_t::ipv6 && !is_valid_ipv6(vv_ip) ) {
        vv_clilog.write(level_t::Err, vv_ip + " ip address is not valid ipv6", GET_SOURCE);
        return EXIT_FAILURE;
    }

    // SOCKET
    Socket<CipherEnc> vv_client(vv_cipher,
        "logs/socket-" + ss_logname,
        "netsocket-client",
        "pwd@server.loop!test",
        2026,
        vv_iptype,
        _FLAG_SOCKET_LOGGER | _FLAG_SOCKET_LOG_TITLE
    );

    // POLICY
    Status tm_status = vv_client.get_policy().set_username("CLooper");
    if( tm_status.is_ok() ) vv_clilog.write(level_t::Succ, "New Client Name: " + vv_client.get_policy().get_username(), GET_SOURCE);
    else vv_clilog.write(level_t::Err, "Client Name Not Changed: " + vv_client.get_policy().get_username(), GET_SOURCE);

    // CREATE
    vv_client.create();

    // CLIENT
    tm_status = client_session<CipherEnc>(vv_client, vv_ip);
    vv_clilog.write(tm_status.is_ok() ? level_t::Succ : level_t::Err, std::to_string(tm_status.get_code()), GET_SOURCE);

    // PRINT
    vv_clilog.print();

    return EXIT_SUCCESS;
}

/**
 * @brief Client Session
 */
template<typename CipherT>
Status client_session(
    Socket<CipherT>& ar_sock,
    const std::string& ar_ip
) noexcept
{
    if( !is_valid_ipv4(ar_ip) && !is_valid_ipv6(ar_ip) )
        return Status::err(domain_t::client, 20000);

    if( !is_valid_socket(ar_sock.get_socket()) )
        return Status::err(domain_t::client, 20001);

    sockaddr* tm_addr = nullptr;
    socklen_t tm_len = 0;

    if( ar_sock.get_ipv() == ipv_t::ipv6 )
    {
        static sockaddr_in6 tm_v6 {};
        tm_v6.sin6_family = tcp::ipv6::domain;
        tm_v6.sin6_port = htons(ar_sock.get_port());

        if( inet_pton(tcp::ipv6::domain, ar_ip.c_str(), &tm_v6.sin6_addr) != 1 )
            return Status::err(domain_t::client, 20002);

        tm_addr = reinterpret_cast<sockaddr*>(&tm_v6);
        tm_len = sizeof(tm_v6);
    }
    else
    {
        static sockaddr_in tm_v4 {};
        tm_v4.sin_family = tcp::ipv4::domain;
        tm_v4.sin_port = htons(ar_sock.get_port());

        if( inet_pton(tcp::ipv4::domain, ar_ip.c_str(), &tm_v4.sin_addr) != 1 )
            return Status::err(domain_t::client, 20002);

        tm_addr = reinterpret_cast<sockaddr*>(&tm_v4);
        tm_len = sizeof(tm_v4);
    }

    if( ::connect(ar_sock.get_socket(), tm_addr, tm_len) != 0 )
    {
        if( ar_sock.get_flag().has(_FLAG_SOCKET_LOGGER) )
            ar_sock.get_logger().write(level_t::Err, ar_ip + " connect failed", GET_SOURCE);

        #if __OS_WINDOWS__
            ar_sock.get_logger().write(level_t::Debug, "Windows Error Code: " + std::to_string(::WSAGetLastError()) ,GET_SOURCE);
        #endif

        return Status::err(domain_t::client, 20003);
    }

    if( ar_sock.get_flag().has(_FLAG_SOCKET_LOGGER) )
        ar_sock.get_logger().write(level_t::Succ, ar_ip + " Connected", GET_SOURCE);

    Status tm_handshake = handshake_send_verify(ar_sock.get_socket());
    if( !tm_handshake.is_ok() )
    {
        if( ar_sock.get_flag().has(_FLAG_SOCKET_LOGGER) )
            ar_sock.get_logger().write(level_t::Err, ar_ip + " handshake send failed | code: " + std::to_string(tm_handshake.get_code()), GET_SOURCE);

        return Status::err(domain_t::client, 20004);
    }

    tm_handshake = handshake_recv_verify(ar_sock.get_socket());
    if( !tm_handshake.is_ok() )
    {
        if( ar_sock.get_flag().has(_FLAG_SOCKET_LOGGER) )
            ar_sock.get_logger().write(level_t::Err, ar_ip + " handshake recv failed | code: " + std::to_string(tm_handshake.get_code()), GET_SOURCE);

        return Status::err(domain_t::client, 20010);
    }

    if( ar_sock.get_flag().has(_FLAG_SOCKET_LOGGER) )
        ar_sock.get_logger().write(level_t::Succ, ar_ip + " Handshake Verified", GET_SOURCE);

    // SEND MESSAGE
    DataPacket tm_packet;

    tm_packet.m_name = ar_sock.get_policy().get_username();
    tm_packet.m_pwd = ar_sock.get_policy().get_password();
    tm_packet.m_msg = utf::to_utf8(U"Hello Server, It's Client! 😀");

    // 10 SEC
    using clock = std::chrono::steady_clock;
    constexpr int vv_total_time = 10;
    auto tm_start = clock::now();

    for(int tm_count = 0; tm_count < vv_total_time; ++tm_count)
    {
        auto tm_tick_time = tm_start + std::chrono::seconds(tm_count);

        // SEND
        if( !ar_sock.send(ar_sock.get_socket(), tm_packet).is_ok() )
        {
            if( ar_sock.get_flag().has(_FLAG_SOCKET_LOGGER) )
                ar_sock.get_logger().write(level_t::Err, "Data couldn't send to " + get_ip(ar_sock.get_socket()), GET_SOURCE);

            return Status::err(domain_t::client, 20005);
        }

        if( ar_sock.get_flag().has(_FLAG_SOCKET_LOGGER) )
            ar_sock.get_logger().write(level_t::Succ, ar_ip + " Sent", GET_SOURCE);

        // RECV
        if( !ar_sock.recv(ar_sock.get_socket(), tm_packet).is_ok() )
        {
            if( ar_sock.get_flag().has(_FLAG_SOCKET_LOGGER) )
                ar_sock.get_logger().write(level_t::Err, "Data couldn't receive from " + get_ip(ar_sock.get_socket()), GET_SOURCE);

            return Status::err(domain_t::client, 20006);
        }

        // OUTPUT
        if( ar_sock.get_flag().has(_FLAG_SOCKET_LOGGER) )
        {
            ar_sock.get_logger().write(level_t::Text, "Password: " + tm_packet.m_pwd, GET_SOURCE);
            ar_sock.get_logger().write(level_t::Text, "Username: " + tm_packet.m_name, GET_SOURCE);
            ar_sock.get_logger().write(level_t::Text, "Message: " + tm_packet.m_msg, GET_SOURCE);
        }

        std::this_thread::sleep_until(tm_tick_time + std::chrono::seconds(1));
    }

    return Status::ok(domain_t::client, 20007);
}
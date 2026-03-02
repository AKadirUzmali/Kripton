// Abdulkadir U. - 2026/03/02

/**
 * Client Ipv6 (İstemci Ipv6)
 * 
 * İstemci ipv4 testini yapmıştık ve başarı ile sonuçlanmıştı.
 * Şimdi ise ipv6 testini yaparak bunu da öğrenmiş olacağız.
 * 
 * Kodlar ipv4 ile aynı olacak, sadece ipv6 için özelleştirilmesi
 * gereken kısımlar özelleştirilecek.
 * 
 * Derleme:
 *  Bsd     :: g++ -I../../../../include -std=c++17 -Wall -Werror -Wextra client-ipv6.cpp -pthread -o bsd/client-ipv6.bsd
 *  Linux   :: g++ -I../../../../include -std=c++17 -Wall -Werror -Wextra client-ipv6.cpp -o linux/client-ipv6.linux
 *  Windows :: g++ -I../../../../include -std=c++17 -Wall -Werror -Wextra client-ipv6.cpp -o windows/client-ipv6.exe -lws2_32
 * 
 * Çalıştırma:
 *  Bsd     :: ./bsd/client-ipv6.bsd <ipv6 address>
 *  Linux   :: ./linux/client-ipv6.linux <ipv6 address>
 *  Windows :: ./windows/client-ipv6.exe <ipv6 address>
 * 
 * Örnek:
 *  Bsd     :: ./bsd/client-ipv6.bsd ::1
 *  Linux   :: ./linux/client-ipv6.linux ::1
 *  Windows :: ./windows/client-ipv6.exe ::1
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
static constexpr Version ss_ver(0, 8, 1);
static constexpr Vch<16> ss_verhash("20260302|test|client|ipv6", 20260302);

static const std::string ss_osname = utf::to_lower(current_os_name());
static const std::string ss_logname = "client-ipv6-" + ss_osname;

// Function Define
template<typename CipherT>
Status client_session(
    Socket<CipherT>& ar_sock,
    const std::string& ar_ip
) noexcept;

// main
int main(int argc, char* argv[])
{
    if( argc < 2 )
        return EXIT_FAILURE;

    using CipherEnc = Xor;

    Logger<FileOut, ConsoleOut> vv_clilog("logs/" + ss_logname, "console-" + ss_logname);
    Crypto<CipherEnc> vv_cipher("ipv6-client-cipher", utf::to_utf8(U"key-cipher-server@20260302"));
    Socket<CipherEnc> vv_client(vv_cipher, "logs/socket-" + ss_logname, "netsocket-client", "pwd@server.ipv6!test", 2026, ipv_t::ipv6, _FLAG_SOCKET_LOGGER | _FLAG_SOCKET_LOG_TITLE);

    // VALID IP?
    const std::string vv_ip(argv[1]);
    if( !is_valid_ipv4(vv_ip) && !is_valid_ipv6(vv_ip) ) {
        vv_clilog.write(level_t::Err, vv_ip + " ip address is not valid, not ipv4/v6", GET_SOURCE);
    }

    // POLICY
    Status tm_status = vv_client.get_policy().set_username("Client-v6");
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

    if( !ar_sock.send(ar_sock.get_socket(), tm_packet).is_ok() )
    {
        if( ar_sock.get_flag().has(_FLAG_SOCKET_LOGGER) )
            ar_sock.get_logger().write(level_t::Err, "Data couldn't send to " + get_ip(ar_sock.get_socket()), GET_SOURCE);

        return Status::err(domain_t::client, 20005);
    }

    if( ar_sock.get_flag().has(_FLAG_SOCKET_LOGGER) )
        ar_sock.get_logger().write(level_t::Succ, ar_ip + " Sent", GET_SOURCE);

    if( !ar_sock.recv(ar_sock.get_socket(), tm_packet).is_ok() )
    {
        if( ar_sock.get_flag().has(_FLAG_SOCKET_LOGGER) )
            ar_sock.get_logger().write(level_t::Err, "Data couldn't receive from " + get_ip(ar_sock.get_socket()), GET_SOURCE);

        return Status::err(domain_t::client, 20006);
    }
    else
    {
        if( ar_sock.get_flag().has(_FLAG_SOCKET_LOGGER) )
        {
            ar_sock.get_logger().write(level_t::Text, "Password: " + tm_packet.m_pwd, GET_SOURCE);
            ar_sock.get_logger().write(level_t::Text, "Username: " + tm_packet.m_name, GET_SOURCE);
            ar_sock.get_logger().write(level_t::Text, "Message: " + tm_packet.m_msg, GET_SOURCE);
        }
    }

    return Status::ok(domain_t::client, 20007);
}
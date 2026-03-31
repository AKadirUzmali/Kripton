// Abdulkadir U. - 2026/03/29

/**
 * Client Fix (İstemci Hata Düzeltme)
 * 
 * Daha önceden geliştirilmiş ve sadece linux üzerinde test edilmiş
 * olan client-update deki Socket ve Client yapılarında hata düzeltme,
 * güvenlik ve performans iyileştirilmeli yapıldı. Client yapısının
 * kullanılabilirliğini test edeceğiz.
 * 
 * Derleme:
 *  Bsd     :: g++ -I../../../include -std=c++17 -Wall -Werror -Wextra client-fix.cpp -pthread -o bsd/client-fix.bsd
 *  Linux   :: g++ -I../../../include -std=c++17 -Wall -Werror -Wextra client-fix.cpp -o linux/client-fix.linux
 *  Windows :: g++ -I../../../include -std=c++17 -Wall -Werror -Wextra client-fix.cpp -o windows/client-fix.exe -lws2_32
 * 
 * Çalıştırma:
 *  Bsd     :: ./bsd/client-fix.bsd <ip version> <ip address> <username>
 *  Linux   :: ./linux/client-fix.linux <ip version> <ip address> <username>
 *  Windows :: ./windows/client-fix.exe <ip version> <ip address> <username>
 * 
 * Örnek:
 *  Bsd     :: ./bsd/client-fix.bsd v6 ::1 tester
 *  Linux   :: ./linux/client-fix.linux v4 127.0.0.1 tester
 *  Windows :: ./windows/client-fix.exe v4 192.168.1.108 tester
 */

// Define
#define __BUILD_DEBUG__
#define __SIGNAL_DETECT__ SigInterrupt | SigAbort | SigIllegal | SigSegFault | SigTerminate

// Include
#include <kits/corekit.hpp>
#include <kits/toolkit.hpp>

#include <dev/developer.hpp>

#include <pool/cipherpool.hpp>
#include <pool/threadpool.hpp>

#include <socket/socket.hpp>
#include <socket/client/client.hpp>

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

using namespace pool::cipherpool;
using namespace pool::threadpool;

using namespace netsocket;
using namespace netsocket::client;

// Static
static constexpr Version ss_ver(0, 8, 8);
static const std::string ss_osname = utf::to_lower(current_os_name());
static const std::string ss_logname = "client-fix-" + ss_osname;

// Function Define
void client_handler(Client& ar_client);

/**
 * @brief main
 * 
 * @param ipv_t Ip Version
 * @param string Ip Address
 * @param string Username
 */
int main(int argc, char* argv[])
{
    if( argc < 4 )
        return EXIT_FAILURE;

    using CipherEnc = Xor;

    Logger<FileOut, ConsoleOut> vv_clilog("logs/" + ss_logname, "console-" + ss_logname);
    CipherEnc vv_cipher("loop-client-cipher", utf::to_utf8(U"key-cipher-server@20260329"));

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
    if( vv_iptype == ipv_t::ipv4 && !Socket::is_valid_ipv4(vv_ip) ) {
        vv_clilog.write(level_t::Err, vv_ip + " ip address is not valid ipv4", GET_SOURCE);
        return EXIT_FAILURE;
    }
    else if( vv_iptype == ipv_t::ipv6 && !Socket::is_valid_ipv6(vv_ip) ) {
        vv_clilog.write(level_t::Err, vv_ip + " ip address is not valid ipv6", GET_SOURCE);
        return EXIT_FAILURE;
    }

    // USERNAME
    const std::string vv_username(argv[3]);

    // CLIENT 1
    Client vv_client_st(
        vv_cipher,
        "logs/socket-" + ss_logname,
        vv_username + "-1",
        "pwd@server!test",
        5070,
        vv_iptype,
        vv_ip,
        client_handler,
        _FLAG_SOCKET_LOGGER
    );

    // CLIENT 2
    Client vv_client_nd(
        vv_cipher,
        "logs/socket-" + ss_logname,
        vv_username + "-2",
        "wrong-password-test",
        vv_client_st.get_port(),
        vv_iptype,
        vv_ip,
        client_handler,
        _FLAG_SOCKET_LOGGER
    );

    // CLIENT 3
    Client vv_client_rd(
        vv_cipher,
        "logs/socket-" + ss_logname,
        vv_username + "-3",
        "pwd@server!test",
        6060,
        vv_iptype,
        vv_ip,
        client_handler,
        _FLAG_SOCKET_LOGGER
    );

    // STATUS VARIABLE
    Status vv_status;

    // HAS ERROR ? (CONVERT DECIMAL OUTPUT TO BINARY AND CHECK THE FLAGS BY BIT BY)
    vv_clilog.write(vv_client_st.has_error() ? level_t::Err : level_t::Succ, vv_client_st.get_policy().get_username(), "Client Has No Error, Flag: " + std::to_string(vv_client_st.get_status_flag().get()), GET_SOURCE);
    vv_clilog.write(vv_client_nd.has_error() ? level_t::Err : level_t::Succ, vv_client_nd.get_policy().get_username(), "Client Has No Error, Flag: " + std::to_string(vv_client_nd.get_status_flag().get()), GET_SOURCE);
    vv_clilog.write(vv_client_rd.has_error() ? level_t::Err : level_t::Succ, vv_client_rd.get_policy().get_username(), "Client Has No Error, Flag: " + std::to_string(vv_client_rd.get_status_flag().get()), GET_SOURCE);

    // CLIENT 1 RUN
    vv_status = vv_client_st.run();
    vv_clilog.write(
        vv_status.is_ok() ? level_t::Succ : level_t::Err,
        vv_client_st.get_policy().get_username(),
        "Client Started To Run, Code: " + std::to_string(vv_status.get_code()),
        GET_SOURCE
    );

    // CLIENT 2 RUN
    vv_status = vv_client_nd.run();
    vv_clilog.write(
        vv_status.is_ok() ? level_t::Succ : level_t::Err,
        vv_client_nd.get_policy().get_username(),
        "Client Started To Run, Code: " + std::to_string(vv_status.get_code()),
        GET_SOURCE
    );

    // CLIENT 3 RUN
    vv_status = vv_client_rd.run();
    vv_clilog.write(
        vv_status.is_ok() ? level_t::Succ : level_t::Err,
        vv_client_rd.get_policy().get_username(),
        "Client Started To Run, Code: " + std::to_string(vv_status.get_code()),
        GET_SOURCE
    );

    // WAIT FOR 15 SECONDS
    std::this_thread::sleep_for(std::chrono::seconds(15));

    // CLIENT 1 STOP
    vv_status = vv_client_st.stop();
    vv_clilog.write(
        vv_status.is_ok() ? level_t::Succ : level_t::Err,
        vv_client_st.get_policy().get_username(),
        "Client Stopped, Code: " + std::to_string(vv_status.get_code())
    );

    // CLIENT 2 STOP
    vv_status = vv_client_nd.stop();
    vv_clilog.write(
        vv_status.is_ok() ? level_t::Succ : level_t::Err,
        vv_client_nd.get_policy().get_username(),
        "Client Stopped, Code: " + std::to_string(vv_status.get_code())
    );

    // CLIENT 3 STOP
    vv_status = vv_client_rd.stop();
    vv_clilog.write(
        vv_status.is_ok() ? level_t::Succ : level_t::Err,
        vv_client_rd.get_policy().get_username(),
        "Client Stopped, Code: " + std::to_string(vv_status.get_code())
    );
}

/**
 * @brief Client Handler
 * 
 * @param Client& Client
 */
void client_handler(Client& ar_client)
{
    // SEND MESSAGE
    DataPacket tm_packet;

    // STATUS VARIABLE
    Status tm_status;

    // SERVER IP
    const std::string tm_server_ip = ar_client.get_server_ip();

    tm_packet.m_name = ar_client.get_policy().get_username();
    tm_packet.m_pwd = ar_client.get_policy().get_password();
    tm_packet.m_msg = "Hello Server, It's " + ar_client.get_policy().get_username() + " ! 😀";

    for(int tm_count = 0; tm_count < 10; ++tm_count)
    {
        // SEND
        tm_status = ar_client.send(ar_client.get_socket(), tm_packet);
        switch( tm_status.get_status() )
        {
            case status::status_t::ok:
                DEBUG_ONLY(ar_client.get_logger().write(level_t::Debug, "Data Sent To " + tm_server_ip, GET_SOURCE));
            break;

            case status::status_t::warn:
                DEBUG_ONLY(ar_client.get_logger().write(level_t::Debug, "Warn, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
            break;

            default:
                DEBUG_ONLY(ar_client.get_logger().write(level_t::Debug, "Data Couldn't Send To " + tm_server_ip + ", Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
            return;
        }

        // RECV
        tm_status = ar_client.recv(ar_client.get_socket(), tm_packet);
        switch( tm_status.get_status() )
        {
            case status::status_t::ok:
                DEBUG_ONLY(ar_client.get_logger().write(level_t::Debug, "Data Receive From " + tm_server_ip, GET_SOURCE));
            break;

            case status::status_t::warn:
                DEBUG_ONLY(ar_client.get_logger().write(level_t::Debug, "Warn, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
            break;

            default:
                DEBUG_ONLY(ar_client.get_logger().write(level_t::Debug, "Data Couldn't Receive From " + tm_server_ip + ", Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
            return;
        }

        // OUTPUT
        DEBUG_ONLY(ar_client.get_logger().write(level_t::Debug, ar_client.get_policy().get_username() ,"Password: " + tm_packet.m_pwd, GET_SOURCE));
        DEBUG_ONLY(ar_client.get_logger().write(level_t::Debug, ar_client.get_policy().get_username() ,"Username: " + tm_packet.m_name, GET_SOURCE));
        DEBUG_ONLY(ar_client.get_logger().write(level_t::Debug, ar_client.get_policy().get_username() ,"Message: " + tm_packet.m_msg, GET_SOURCE));

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
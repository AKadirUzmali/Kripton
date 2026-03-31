// Abdulkadir U. - 2026/03/29

/**
 * Server Fix (Sunucu Hata Düzeltme)
 * 
 * Daha önceden geliştirilmiş ve sadece linux üzerinde test edilmiş
 * olan server-update deki Socket ve Server yapılarında hata düzeltme,
 * güvenlik ve performans iyileştirilmeli yapıldı. Sunucu yapısının
 * kullanılabilirliğini test edeceğiz.
 * 
 * Derleme:
 *  Bsd     :: g++ -I../../../include -std=c++17 -Wall -Werror -Wextra server-fix.cpp -pthread -o bsd/server-fix.bsd
 *  Linux   :: g++ -I../../../include -std=c++17 -Wall -Werror -Wextra server-fix.cpp -o linux/server-fix.linux
 *  Windows :: g++ -I../../../include -std=c++17 -Wall -Werror -Wextra server-fix.cpp -o windows/server-fix.exe -lws2_32
 * 
 * Çalıştırma:
 *  Bsd     :: ./bsd/server-fix.bsd <ip version>
 *  Linux   :: ./linux/server-fix.linux <ip version>
 *  Windows :: ./windows/server-fix.exe <ip version>
 * 
 * Örnek:
 *  Bsd     :: ./bsd/server-fix.bsd v6
 *  Linux   :: ./linux/server-fix.linux v4
 *  Windows :: ./windows/server-fix.exe v4
 * 
 * Sonuç:
 *  %0.0 işlemci kullanımı ve yaklaşık olarak 400-500kb ram kullanımı ile
 *  performanslı bir şekilde çalışmıştır. Daha da geliştirilmekle birlikte
 *  genel olarak düzgün çalıştığı testpit edilmiştir. Ekstra veya diğer bazı
 *  özellikler daha test edilmemiş olabilir ya da farkında olmadan
 *  test edilmiş olabilir. Şuan için sunucudan beklediğimizi karşılamıştır.
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
#include <socket/server/server.hpp>

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
using namespace netsocket::server;

// Static
static constexpr Version ss_ver(0, 8, 8);
static const std::string ss_osname = utf::to_lower(current_os_name());
static const std::string ss_logname = "server-fix-" + ss_osname;

// Function Define
void server_handler(
    Server& ar_server,
    const socket_t& ar_socket,
    const SocketCtx& ar_info
);

/**
 * @brief main
 */
int main(int argc, char* argv[])
{
    if( argc < 2 )
        return EXIT_FAILURE;

    using CipherEnc = Xor;

    Logger<FileOut, ConsoleOut> vv_servlog("logs/debug-" + ss_logname, "console-" + ss_logname);
    CipherEnc vv_cipher("server-cipher", utf::to_utf8(U"key-cipher-server@20260329"));
    
    // VALID IP VERSION?
    const std::string vv_ipv(argv[1]);
    if( vv_ipv.compare("v4") != 0 && vv_ipv.compare("v6") != 0 )
    {
        vv_servlog.write(level_t::Err, vv_ipv + " Ip Version Is Not Valid, Not v4/v6", GET_SOURCE);
        return EXIT_FAILURE;
    }
    const ipv_t vv_iptype = vv_ipv.compare("v4") == 0 ? ipv_t::ipv4 : ipv_t::ipv6;
    
    // SERVER 1
    Server vv_server_st(
        vv_cipher,
        "logs/socket-" + ss_logname,
        "netserver1",
        "pwd@server!test",
        true,
        5070,
        vv_iptype,
        server_handler,
        policy::_DEF_CONNECTION,
        policy::_DEF_SAME_IP_COUNT,
        _FLAG_SOCKET_LOGGER
    );

    // SERVER 2
    Server vv_server_nd(
        vv_cipher,
        "logs/socket-" + ss_logname,
        "netserver2",
        "pwd@server!test",
        true,
        vv_server_st.get_port(),
        vv_iptype,
        server_handler,
        policy::_DEF_CONNECTION,
        policy::_DEF_SAME_IP_COUNT,
        _FLAG_SOCKET_LOGGER
    );

    // HAS ERROR ? (CONVERT DECIMAL OUTPUT TO BINARY AND CHECK THE FLAGS BY BIT BY)
    vv_servlog.write(vv_server_st.has_error() ? level_t::Err : level_t::Succ, vv_server_st.get_policy().get_username(), "Server Has No Error, Flag: " + std::to_string(vv_server_st.get_status_flag().get()), GET_SOURCE);
    vv_servlog.write(vv_server_nd.has_error() ? level_t::Err : level_t::Succ, vv_server_nd.get_policy().get_username(), "Server Has No Error, Flag: " + std::to_string(vv_server_nd.get_status_flag().get()), GET_SOURCE);

    // STATUS VARIABLE
    Status vv_status;

    // MAX CONNECTION, MAX SAME IP
    const policy::max_conn_t vv_max_conn = 4;
    const policy::max_conn_t vv_max_same_ip = 3;

    // FOR SERVER 1
    vv_status = vv_server_st.get_policy().set_max_connection(vv_max_conn);
    vv_servlog.write(
        vv_status.is_ok() ? level_t::Succ : level_t::Err,
        vv_server_st.get_policy().get_username(),
        "New Max Connection Limit Is: " + std::to_string(vv_server_st.get_policy().get_max_connection()),
        GET_SOURCE);

    vv_status = vv_server_st.get_policy().set_max_same_ip(vv_max_same_ip);
    vv_servlog.write(
        vv_status.is_ok() ? level_t::Succ : level_t::Err,
        vv_server_st.get_policy().get_username(),
        "New Max Same Ip Limit Is: " + std::to_string(vv_server_st.get_policy().get_max_same_ip()),
        GET_SOURCE);

    // FOR SERVER 2
    vv_status = vv_server_nd.get_policy().set_max_connection(vv_max_conn);
    vv_servlog.write(
        vv_status.is_ok() ? level_t::Succ : level_t::Err,
        vv_server_nd.get_policy().get_username(),
        "New Max Connection Limit Is: " + std::to_string(vv_server_nd.get_policy().get_max_connection()),
        GET_SOURCE);

    vv_status = vv_server_nd.get_policy().set_max_same_ip(vv_max_same_ip);
    vv_servlog.write(
        vv_status.is_ok() ? level_t::Succ : level_t::Err,
        vv_server_nd.get_policy().get_username(),
        "New Max Same Ip Limit Is: " + std::to_string(vv_server_nd.get_policy().get_max_same_ip()),
        GET_SOURCE);

    // PRINT
    vv_server_st.print();
    vv_server_nd.print();

    // SERVER 2 SET NEW PORT
    vv_status = vv_server_nd.set_port(6060);
    vv_servlog.write(
        vv_status.is_ok() ? level_t::Succ : level_t::Err,
        vv_server_nd.get_policy().get_username(),
        "Server Port Changed, Code: " + std::to_string(vv_status.get_code()),
        GET_SOURCE
    );

    // HAS ERROR ? (CONVERT DECIMAL OUTPUT TO BINARY AND CHECK THE FLAGS BY BIT BY)
    vv_servlog.write(vv_server_st.has_error() ? level_t::Err : level_t::Succ, vv_server_st.get_policy().get_username(), "Server Has No Error, Flag: " + std::to_string(vv_server_st.get_status_flag().get()), GET_SOURCE);
    vv_servlog.write(vv_server_nd.has_error() ? level_t::Err : level_t::Succ, vv_server_nd.get_policy().get_username(), "Server Has No Error, Flag: " + std::to_string(vv_server_nd.get_status_flag().get()), GET_SOURCE);

    // RUN SERVER 1
    vv_status = vv_server_st.run();
    vv_servlog.write(vv_status.is_ok() ? level_t::Succ : level_t::Err, vv_server_st.get_policy().get_username(), "Server Started To Run, Code: " + std::to_string(vv_status.get_code()));

    // RUN SERVER 2
    vv_status = vv_server_nd.run();
    vv_servlog.write(vv_status.is_ok() ? level_t::Succ : level_t::Err, vv_server_nd.get_policy().get_username(), "Server Started To Run, Code: " + std::to_string(vv_status.get_code()));

    // HAS ERROR ? (CONVERT DECIMAL OUTPUT TO BINARY AND CHECK THE FLAGS BY BIT BY)
    vv_servlog.write(vv_server_st.has_error() ? level_t::Err : level_t::Succ, vv_server_st.get_policy().get_username(), "Server Has No Error, Flag: " + std::to_string(vv_server_st.get_status_flag().get()), GET_SOURCE);
    vv_servlog.write(vv_server_nd.has_error() ? level_t::Err : level_t::Succ, vv_server_nd.get_policy().get_username(), "Server Has No Error, Flag: " + std::to_string(vv_server_nd.get_status_flag().get()), GET_SOURCE);

    // WAIT 5 SECONDS
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // PRINT AGAIN
    vv_server_st.print();
    vv_server_nd.print();

    // WAIT 15 SECONDS
    std::this_thread::sleep_for(std::chrono::seconds(15));

    // STOP SERVER 1
    vv_status = vv_server_st.stop();
    vv_servlog.write(vv_status.is_ok() ? level_t::Succ : level_t::Err, vv_server_st.get_policy().get_username(), "Server Stopped, Code: " + std::to_string(vv_status.get_code()));

    // STOP SERVER 2
    vv_status = vv_server_nd.stop();
    vv_servlog.write(vv_status.is_ok() ? level_t::Succ : level_t::Err, vv_server_nd.get_policy().get_username(), "Server Stopped, Code: " + std::to_string(vv_status.get_code()));
}

/**
 * @brief Server Handler
 * 
 * @param Server& Server
 * @param socket_t& Socket
 * @param SocketCtx& Socket Info
 */
void server_handler(
    Server& ar_server,
    const socket_t& ar_socket,
    const SocketCtx& ar_info
)
{
    // TIMEOUT
    const auto tm_timeout = std::chrono::seconds(ar_server.get_timeout());

    // RUNNING
    while( ar_server.is_running() )
    {
        // SET FD
        fd_set tm_readfs;
        FD_ZERO(&tm_readfs);
        FD_SET(ar_socket, &tm_readfs);

        // SET TIMEOUT
        timeval tm_tv {};
        tm_tv.tv_sec = tm_timeout.count();
        tm_tv.tv_usec = 0;

        // SELECT
        int tm_ready = ::select(ar_socket + 1, &tm_readfs, nullptr, nullptr, &tm_tv);
        if( tm_ready <= 0 )
            return;

        // BAN CHECK
        if( ar_server.get_policy().is_connection_banned(ar_socket) )
        {
            DEBUG_ONLY(ar_server.get_logger().write(level_t::Warn, ar_server.get_policy().get_username(), ar_info.m_ip + "/" + std::to_string(ar_socket) + " Banned Ip/Socket tried to connect", GET_SOURCE));
            break;
        }
        // ALLOW CHECK
        else if( !ar_server.get_policy().is_connection_allowed(ar_info.m_ip) )
        {
            DEBUG_ONLY(ar_server.get_logger().write(level_t::Warn, ar_server.get_policy().get_username(), ar_info.m_ip + " Ip Not Allowed", GET_SOURCE));
            break;
        }

        // DATA PACKET & RECV
        DataPacket tm_datapack;
        Status tm_status = ar_server.recv(ar_socket, tm_datapack);

        // RECV STATUS
        switch( tm_status.get_status() ) {
            case status::status_t::ok:
                DEBUG_ONLY(ar_server.get_logger().write(level_t::Succ, ar_server.get_policy().get_username(), "Received From " + ar_info.m_ip, GET_SOURCE));
            break;

            case status::status_t::warn:
                DEBUG_ONLY(ar_server.get_logger().write(level_t::Warn, ar_server.get_policy().get_username(), "Warning, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
            break;

            case status::status_t::err:
                switch( tm_status.get_code() )
                {
                    case status::to_underlying(server_code_t::client_entered_old_password):
                    case status::to_underlying(server_code_t::client_sent_wrong_password):
                        break;

                    default:
                        DEBUG_ONLY(ar_server.get_logger().write(level_t::Debug, ar_server.get_policy().get_username(), "Server Receive Error, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
                        return;
                }
            break;

            default:
                DEBUG_ONLY(ar_server.get_logger().write(level_t::Err, ar_server.get_policy().get_username(), "Data Couldn't Receive From " + ar_info.m_ip + ", Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
            return;
        }

        // LOG ALL
        DEBUG_ONLY(ar_server.get_logger().write(level_t::Debug, ar_server.get_policy().get_username() + "|Recv", "Id: " + std::to_string(ar_socket), GET_SOURCE));
        DEBUG_ONLY(ar_server.get_logger().write(level_t::Debug, ar_server.get_policy().get_username() + "|Recv", "Password: " + tm_datapack.m_pwd, GET_SOURCE));
        DEBUG_ONLY(ar_server.get_logger().write(level_t::Debug, ar_server.get_policy().get_username() + "|Recv", "Username: " + tm_datapack.m_name, GET_SOURCE));
        DEBUG_ONLY(ar_server.get_logger().write(level_t::Debug, ar_server.get_policy().get_username() + "|Recv", "Message: " + tm_datapack.m_msg, GET_SOURCE));

        // SEND
        tm_status = ar_server.send(ar_socket, tm_datapack);

        // SEND STATUS
        switch( tm_status.get_status() )
        {
            case status::status_t::ok:
                DEBUG_ONLY(ar_server.get_logger().write(level_t::Debug, ar_server.get_policy().get_username(), "Sent To " + ar_info.m_ip, GET_SOURCE));
            break;

            case status::status_t::warn:
                DEBUG_ONLY(ar_server.get_logger().write(level_t::Debug, ar_server.get_policy().get_username(), "Warning, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
            break;

            case status::status_t::err:
                switch( tm_status.get_code() )
                {
                    default:
                        DEBUG_ONLY(ar_server.get_logger().write(level_t::Debug, ar_server.get_policy().get_username(), "Server Send Error, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
                        return;
                }
            break;

            default:
                DEBUG_ONLY(ar_server.get_logger().write(level_t::Debug, ar_server.get_policy().get_username(), "Data Couldn't Send To " + ar_info.m_ip + ", Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
            return;
        }
    }
}
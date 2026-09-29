// Abdulkadir U. - 2026/09/29

/**
 * Server Chat (Sunucu Mesajlaşma)
 * 
 * Sunucu ve istemcinin kendi arasında mesajlaşabilmesini
 * başarmıştık (6 ay askerliğe gitmemden önce) ve artık
 * sunucu ile mesajlaşıldığından tüm istemcilerin haberi
 * olmasını sağlayacağız yani sunucuya a1 istemcisi mesaj
 * gönderdiğinde sadece a1 ile sunucu bilirken artık
 * sunucu kullanıcı adı ve mesajı diğer istemcilere, örnek;
 * b1, c2, d3, e1 gibi istemcilere gönderecek ve o istemcilerde
 * a1 adlı istemciden mesaj geldiğini anlayacak ve a1 istemcisi
 * sunucuyla irtibatını kesitğinde bunu sunucu yine bildirecek.
 * Amaç ana sınıf fonksiyonlarına dokunmak değil, test ile
 * sunucunun kapasitesini ölçme olacak. İlerleyen vakitlerde
 * bunlara ek olarak grafik arayüzü yapılabilip kullanılabilir
 * hale de getirtilebilir...
 * 
 * Derleme:
 *  Bsd     :: g++ -I../../../include -std=c++17 -Wall -Werror -Wextra server-chat.cpp -pthread -o bsd/server-chat.bsd
 *  Linux   :: g++ -I../../../include -std=c++17 -Wall -Werror -Wextra server-chat.cpp -o linux/server-chat.linux
 *  Windows :: g++ -I../../../include -std=c++17 -Wall -Werror -Wextra server-chat.cpp -o windows/server-chat.exe -lws2_32
 * 
 * Çalıştırma:
 *  Bsd     :: ./bsd/server-chat.bsd <ip version>
 *  Linux   :: ./linux/server-chat.linux <ip version>
 *  Windows :: ./windows/server-chat.exe <ip version>
 * 
 * Örnek:
 *  Bsd     :: ./bsd/server-chat.bsd v6
 *  Linux   :: ./linux/server-chat.linux v4
 *  Windows :: ./windows/server-chat.exe v4
 * 
 * Sonuç:
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
static constexpr Version ss_ver(0, 8, 9);
static const std::string ss_osname = utf::to_lower(current_os_name());
static const std::string ss_logname = "server-chat" + ss_osname;

// Function Define
void chat_handler(
    Server& ar_server,
    const socket_t& ar_socket
);

/**
 * @brief main
 */
int main(int argc, char* argv[])
{
    // ARG CHECK
    if( argc < 2 )
        return EXIT_FAILURE;

    // SELECTED CIPHER
    using CipherEnc = Xor;

    // LOGGER
    Logger<FileOut, ConsoleOut> vv_chatlog("logs/debug-" + ss_logname, "console-" + ss_logname);

    // CIPHER
    CipherEnc vv_cipher("chat-cipher", utf::to_utf8(U"key-cipher-chat@20260929!ğşüç"));

    // IS VALID IP VERSION?
    const std::string vv_ipv(argv[1]);
    if( vv_ipv.compare("v4") != 0 && vv_ipv.compare("v6") != 0 )
    {
        vv_chatlog.write(level_t::Err, vv_ipv + " Ip Version Is Not Valid, Not v4/v6", GET_SOURCE);
        return EXIT_FAILURE;
    }

    // IP TYPE VAR
    const ipv_t vv_iptype = vv_ipv.compare("v4") == 0 ? ipv_t::ipv4 : ipv_t::ipv6;

    // SERVER
    Server vv_chat(
        vv_cipher,
        "logs/socket-" + ss_logname,
        "chat-server",
        "pwq@chat-20260929",
        true,
        6565,
        vv_iptype,
        chat_handler,
        policy::_DEF_CONNECTION,
        policy::_DEF_SAME_IP_COUNT,
        _FLAG_SOCKET_LOGGER
    );

    // HAS ERROR ? (CONVERT DECIMAL OUTPUT TO BINARY AND CHECK THE FLAGS BY BIT BY)
    vv_chatlog.write(vv_chat.has_error() ? level_t::Err : level_t::Succ, vv_chat.get_policy().get_username(), "Server Flag: " + std::to_string(vv_chat.get_status_flag().get()), GET_SOURCE);

    // STATUS VAR
    Status vv_status;

    // MAX CONNECTION, MAX SAME IP
    const policy::max_conn_t vv_max_conn = 10;
    const policy::max_conn_t vv_max_same_ip = 7;

    // MAX CONN
    vv_status = vv_chat.get_policy().set_max_connection(vv_max_conn);
    vv_chatlog.write(
        vv_status.is_ok() ? level_t::Succ : level_t::Err,
        vv_chat.get_policy().get_username(),
        "New Max Connection Limit Is: " + std::to_string(vv_chat.get_policy().get_max_connection()),
        GET_SOURCE
    );

    // MAX SAME IP
    vv_status = vv_chat.get_policy().set_max_same_ip(vv_max_same_ip);
    vv_chatlog.write(
        vv_status.is_ok() ? level_t::Succ : level_t::Err,
        vv_chat.get_policy().get_username(),
        "New Max Same Ip Limit Is: " + std::to_string(vv_chat.get_policy().get_max_same_ip()),
        GET_SOURCE
    );

    // HAS ERROR ? (CONVERT DECIMAL OUTPUT TO BINARY AND CHECK THE FLAGS BY BIT BY)
    vv_chatlog.write(vv_chat.has_error() ? level_t::Err : level_t::Succ, vv_chat.get_policy().get_username(), "Server Flag: " + std::to_string(vv_chat.get_status_flag().get()), GET_SOURCE);

    // PRINT SERVER INFO
    vv_chat.print();

    // RUN SERVER
    vv_status = vv_chat.run();

    // IS SERVER RUNNING? LOG IT
    vv_chatlog.write(
        vv_status.is_ok() ? level_t::Succ: level_t::Err,
        vv_chat.get_policy().get_username(),
        "Server Run Code: " + std::to_string(vv_status.get_code()),
        GET_SOURCE
    );

    // WAIT FOR SERVER START
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // CHECK SERVER RUN
    if( !vv_status.is_ok() || !vv_chat.is_running() )
        vv_chatlog.write(level_t::Err, vv_chat.get_policy().get_username(), "Server Failed To Start Or Stopped Immediately!", GET_SOURCE);
    // WAIT FOR 20 SECONDS
    else
        std::this_thread::sleep_for(std::chrono::seconds(20));

    // STOP THE SERVER
    vv_status = vv_chat.stop();

    // SERVER STOPPED? LOG
    vv_chatlog.write(vv_status.is_ok() ? level_t::Succ : level_t::Err,
        vv_chat.get_policy().get_username(), "Server Stopped, Code: " + std::to_string(vv_status.get_code()),
        GET_SOURCE
    );

    // END
    return EXIT_SUCCESS;
}

/**
 * @brief Chat Handler
 * 
 * @param Server& Sunucu
 * @param socket_t& Soket
 * 
 * Kullanıcılar sunucuya bağlanıp sohbet edebilecek
 * fakat sunucu da kullanıcı hiç kalmadıktan sonra
 * sunucu kendisini durduracak
 */
void chat_handler(
    Server& ar_server,
    const socket_t& ar_socket
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

        // SELECT ERR
        if( tm_ready < 0 )
            return;
        // SELECT TIMEOUT
        else if( tm_ready == 0 )
            return;

        // Find Ip Address
        const std::string tm_ip = Socket::get_ip(ar_socket);

        // BAN CHECK
        if( ar_server.get_policy().is_connection_banned(ar_socket) )
        {
            // LOG IT
            ar_server.get_logger().write(
                level_t::Warn,
                ar_server.get_policy().get_username(),
                tm_ip + "/" + std::to_string(ar_socket) + " Banned Ip/Socket Tried To Connect Server",
                GET_SOURCE
            );
            break;
        }
        // NOT ALLOW CHECK
        else if( !ar_server.get_policy().is_connection_allowed(tm_ip) )
        {
            // LOG IT (DEBUG)
            DEBUG_ONLY(ar_server.get_logger().write(level_t::Warn,
                ar_server.get_policy().get_username(),
                tm_ip + " Ip Not Allowed",
                GET_SOURCE)
            );
            break;
        }

        // DATA PACKET
        DataPacket tm_datapack;

        // RECEIVE
        Status tm_status = ar_server.recv(ar_socket, tm_datapack);

        // RECV STATUS
        switch( tm_status.get_status() )
        {
            // OK
            case status::status_t::ok:
            {
                DEBUG_ONLY(ar_server.get_logger().write(level_t::Succ, ar_server.get_policy().get_username(), "Received From " + tm_ip, GET_SOURCE));
                
                // FIND CLIENT
                auto tm_cli = ar_server.get_clients().find(ar_socket);
                if( tm_cli != ar_server.get_clients().end() )
                {
                    // TEMP CLIENT
                    SocketCtx tm_storecli {};

                    tm_storecli.m_ip = tm_cli->second.m_ip;
                    tm_storecli.m_user.m_same_user_count = tm_cli->second.m_user.m_same_user_count;
                    tm_storecli.m_user.m_try_passwd = tm_cli->second.m_user.m_try_passwd;
                    tm_storecli.m_user.m_username = tm_datapack.m_name;

                    // UPDATE CLIENT DATA
                    ar_server.update_client(ar_socket, tm_storecli);
                }
            }
            break;

            // WARN
            case status::status_t::warn:
                DEBUG_ONLY(ar_server.get_logger().write(level_t::Warn, ar_server.get_policy().get_username(), "Warning, Code: " + std::to_string(tm_status.get_code()), GET_SOURCE));
            break;

            // ERROR
            case status::status_t::err:
                // ERROR CODE
                switch( tm_status.get_code() )
                {
                    // NOT RECV BECAUSE OF CLIENT
                    case status::to_underlying(socket_code_t::socket_not_recv_header):
                    // CLIENT CONNECTION CLOSED
                    case status::to_underlying(socket_code_t::recv_socket_close_header):
                        {
                            // FIND CLIENT
                            auto tm_cli = ar_server.get_clients().find(ar_socket);
                            if( tm_cli != ar_server.get_clients().end() )
                                tm_datapack.m_msg = "User (" + tm_cli->second.m_user.m_username + ") Disconnected";
                            else
                                tm_datapack.m_msg = "(" + std::to_string(ar_socket) + "/" + tm_ip + ") Disconnected";

                            // CLIENT MSG SENDER NAME
                            tm_datapack.m_name = "Server";

                            // LOG IT
                            ar_server.get_logger().write(level_t::Info, tm_datapack.m_msg, GET_SOURCE);

                            // SEND MSG TO ALL CLIENTS
                            for( const auto& [tm_socket, tm_ctx] : ar_server.get_clients() )
                            {
                                if( tm_socket != ar_socket )
                                    ar_server.send(tm_socket, tm_datapack);
                            }
                        }
                    return;

                    // OLD PWD
                    case status::to_underlying(server_code_t::client_entered_old_password):
                        DEBUG_ONLY(ar_server.get_logger().write(level_t::Err, ar_server.get_policy().get_username(), "Client Entered Old Password", GET_SOURCE));
                    break;

                    // WRONG PWD
                    case status::to_underlying(server_code_t::client_sent_wrong_password):
                        DEBUG_ONLY(ar_server.get_logger().write(level_t::Err, ar_server.get_policy().get_username(), "Client Sent Wrong Password", GET_SOURCE));
                    break;

                    // DEFAULT
                    default:
                        DEBUG_ONLY(ar_server.get_logger().write(level_t::Debug,
                            ar_server.get_policy().get_username(),
                            "Server Receive Error, Code: " + std::to_string(tm_status.get_code()),
                            GET_SOURCE)
                        );
                }
            return;

            // DATA RECEIVE ERROR
            default: return;
        }

        // SENT TO ALL CLIENTS
        for( const auto& [tm_socket, tm_ctx] : ar_server.get_clients() )
        {
            // SEND
            tm_status = ar_server.send(tm_socket, tm_datapack);

            // SEND STATUS
            switch( tm_status.get_status() )
            {
                // OK
                case status::status_t::ok:
                    DEBUG_ONLY(ar_server.get_logger().write(level_t::Debug,
                        ar_server.get_policy().get_username(),
                        "Sent To " + std::to_string(tm_socket), GET_SOURCE)
                    );
                break;

                // WARN
                case status::status_t::warn:
                    DEBUG_ONLY(ar_server.get_logger().write(level_t::Debug,
                        ar_server.get_policy().get_username(),
                        "Warning, Code: " + std::to_string(tm_status.get_code()),
                        GET_SOURCE)
                    );
                break;

                // ERR
                case status::status_t::err:
                    DEBUG_ONLY(ar_server.get_logger().write(level_t::Debug,
                        ar_server.get_policy().get_username(),
                        "Server Send Error To Socket " + std::to_string(tm_socket) + ", Code: " + std::to_string(tm_status.get_code()),
                        GET_SOURCE)
                    );
                continue;

                // DATA SENT ERROR
                default:
                    auto tm_txt = "Data Couldn't Send To " + std::to_string(tm_socket) + ", Code: " + std::to_string(tm_status.get_code());
                    DEBUG_ONLY(ar_server.get_logger().write(level_t::Debug,
                        ar_server.get_policy().get_username(),
                        tm_txt,
                        GET_SOURCE)
                    );
            }
        }
    }
}
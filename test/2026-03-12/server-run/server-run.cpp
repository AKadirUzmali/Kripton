// Abdulkadir U. - 2026/03/12

/**
 * Server Run (Sunucu Çalıştır)
 * 
 * Derleme:
 *  Bsd     :: g++ -I../../../include -std=c++17 -Wall -Werror -Wextra server-run.cpp -pthread -o bsd/server-run.bsd
 *  Linux   :: g++ -I../../../include -std=c++17 -Wall -Werror -Wextra server-run.cpp -o linux/server-run.linux
 *  Windows :: g++ -I../../../include -std=c++17 -Wall -Werror -Wextra server-run.cpp -o windows/server-run.exe -lws2_32
 * 
 * Çalıştırma:
 *  Bsd     :: ./bsd/server-run.bsd <ip version>
 *  Linux   :: ./linux/server-run.linux <ip version>
 *  Windows :: ./windows/server-run.exe <ip version>
 * 
 * Örnek:
 *  Bsd     :: ./bsd/server-run.bsd v6
 *  Linux   :: ./linux/server-run.linux v4
 *  Windows :: ./windows/server-run.exe v4
 * 
 * Sonuç:
 *  %0.0 işlemci kullanımı ve taklaşık olarak 300-350kb ram kullanımı ile
 *  performanslı bir şekilde çalışmıştır. Daha da geliştirilmekle birlikte
 *  genel olarak düzgün çalıştığı testpit edilmiştir. Ekstra veya diğer bazı
 *  özellikler daha test edilmemiş olabilir ya da farkında olmadan
 *  test edilmiş olabilir. Şuan için sunucudan beklediğimizi karşılamıştır.
 */

// Define
#define __BUILD_TYPE__ built_t::Debug
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
static constexpr Version ss_ver(0, 8, 5);
static constexpr Vch<16> ss_verhash("20260312|test|server-run", 20260312);

static const std::string ss_osname = utf::to_lower(current_os_name());
static const std::string ss_logname = "server-run-" + ss_osname;

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
    CipherEnc vv_cipher("server-cipher", utf::to_utf8(U"key-cipher-server@20260312"));
    
    // VALID IP VERSION?
    const std::string vv_ipv(argv[1]);
    if( vv_ipv.compare("v4") != 0 && vv_ipv.compare("v6") != 0 )
    {
        vv_servlog.write(level_t::Err, vv_ipv + " ip version is not valid, not v4/v6", GET_SOURCE);
        return EXIT_FAILURE;
    }
    const ipv_t vv_iptype = vv_ipv.compare("v4") == 0 ? ipv_t::ipv4 : ipv_t::ipv6;
    
    // SOCKET
    Server vv_server(
        vv_cipher,
        "logs/server-" + ss_logname,
        "netserver",
        "pwd@server!test",
        true,
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

    // PRINT
    vv_server.print();

    // RUN
    std::thread tm_run([&]{
        vv_server.run();
    });

    // INFO
    vv_servlog.write(level_t::Info, "Server Run Function Started", GET_SOURCE);

    // BAN IP
    const std::string tm_banip("192.168.1.108");

    // BAN IP FOR 3 SEC
    vv_servlog.write(vv_server.get_policy().set_ban(tm_banip).is_ok() ? level_t::Succ : level_t::Err, tm_banip + " banned from server", GET_SOURCE);

    // 3 SEC
    std::this_thread::sleep_for(std::chrono::seconds(3));

    // UNBAN
    vv_servlog.write(vv_server.get_policy().set_ban(tm_banip, false).is_ok() ? level_t::Succ : level_t::Err, tm_banip + " ban removed after 3 second", GET_SOURCE);

    // WAIT FOR 5 SEC
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // PRINT AGAIN
    vv_server.print();

    // WAIT FOR 15
    std::this_thread::sleep_for(std::chrono::seconds(15));

    // SERVER STOP
    vv_server.stop();

    // THREAD JOIN
    tm_run.join();
}
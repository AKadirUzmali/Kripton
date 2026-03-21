// Abdulkadir U. - 2026/03/22
/**
 * Server Update (Sunucu Güncelleme)
 * 
 * Test işlemi önceden yapılmış olan sunucu bir kaç güncelleme
 * alaraktan bir üst sürüme çıkarılmıştır. Bunun testlerinin
 * yapılması için 2026-03-12 tarihinde yapılmış olan sunucu
 * test kodu aynı şekilde alınmış olup üzerine değişiklik
 * yapılarak bu test dosyayı oluşturulmuştur
 * 
 * Derleme:
 *  Bsd     :: g++ -I../../../include -std=c++17 -Wall -Werror -Wextra server-update.cpp -pthread -o bsd/server-update.bsd
 *  Linux   :: g++ -I../../../include -std=c++17 -Wall -Werror -Wextra server-update.cpp -o linux/server-update.linux
 *  Windows :: g++ -I../../../include -std=c++17 -Wall -Werror -Wextra server-update.cpp -o windows/server-update.exe -lws2_32
 * 
 * Çalıştırma:
 *  Bsd     :: ./bsd/server-update.bsd <ip version>
 *  Linux   :: ./linux/server-update.linux <ip version>
 *  Windows :: ./windows/server-update.exe <ip version>
 * 
 * Örnek:
 *  Bsd     :: ./bsd/server-update.bsd v6
 *  Linux   :: ./linux/server-update.linux v4
 *  Windows :: ./windows/server-update.exe v4
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
static constexpr Version ss_ver(0, 8, 6);
static constexpr Vch<16> ss_verhash("20260321|test|server-update", 20260321);

static const std::string ss_osname = utf::to_lower(current_os_name());
static const std::string ss_logname = "server-update-" + ss_osname;

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

    Logger<FileOut, ConsoleOut> vv_servlog("logs/debug-" + ss_logname, "console-" + ss_logname);
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
        "logs/socket-" + ss_logname,
        "netserver",
        "pwd@server!test",
        true,
        2026,
        vv_iptype,
        _FLAG_SOCKET_LOGGER
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
    vv_status = vv_server.run();

    // INFO
    if( !vv_status.is_ok() )
    {
        vv_servlog.write(level_t::Err, "Server Run Failed | Code: " + std::to_string(vv_status.get_code()), GET_SOURCE);
        return EXIT_FAILURE;
    }
    vv_servlog.write(level_t::Info, "Server Started To Run", GET_SOURCE);

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
}
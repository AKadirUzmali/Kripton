// Abdulkadir U. - 2026/02/09

/**
 * Socket Compile (Soket Derle)
 * 
 * Soket yapısını derleyip sonrasında basitçe veri gönderme ve
 * veri alma testleri yaparak olası hataları başlangıçta düzelteceğiz.
 * Bu sayede ileriki safhaya yani istemci (client) ve sunucu (server)
 * kısmına geçildiğinde sistemin temeli kaynaklı hatalar olmayacak
 * 
 * Derleme:
 *  Bsd     :: g++ -I../../../include -std=c++17 -Wall -Werror -Wextra socket-compile-test.cpp -pthread -o bsd/socket-compile-test.bsd
 *  Linux   :: g++ -I../../../include -std=c++17 -Wall -Werror -Wextra socket-compile-test.cpp -o linux/socket-compile-test.linux
 *  Windows :: g++ -I../../../include -std=c++17 -Wall -Werror -Wextra socket-compile-test.cpp -o windows/socket-compile-test.exe
 */

// Include
#define __BUILD_TYPE__ build_t::Debug
#include <kits/corekit.hpp>
#include <kits/toolkit.hpp>
#include <kits/outputkit.hpp>
#include <kits/hashkit.hpp>

#include <dev/developer.hpp>

#include <pool/cryptopool.hpp>

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

using namespace netsocket;

// Static
static inline constexpr Version ss_ver(0, 7, 0);
static inline constexpr Vch<16> ss_verhash("20260209|test|socket-compile", 20260209); // 2026-02-09

static std::string ss_osname(utf::to_lower(std::string(current_os_name())));
static std::string ss_filename = "2026-02-16-algorithm-crypto-socket-" + ss_osname;

static Logger<FileOut, ConsoleOut> ss_testlog("logs/" + ss_filename, ss_filename + "-console");
static Crypto<Xor> ss_cipher("test-cipher-socket", tools::charset::utf::to_utf8(U"key-2026-cipher-socket-0209"));
static Socket<Xor> ss_socket(ss_cipher, "logs/socket-test-" + ss_osname, "netsocket-cipher", "pwd-test-0209", 9999, ipv_t::ipv4, _FLAG_SOCKET_LOGGER);

// main
int main(void)
{
    tools::console::enable_utf8_console();

    ss_testlog.write(level_t::Info, ss_verhash.c_str(), GET_SOURCE);
    ss_testlog.write(level_t::Debug, ss_buildtype.c_str(), GET_SOURCE);
    ss_testlog.write(level_t::Info, "Algorithm Name: " + ss_cipher.algorithm().get_name(), GET_SOURCE);
    ss_testlog.write(level_t::Info, "Algorithm Key: " + ss_cipher.algorithm().get_key(), GET_SOURCE);
    ss_testlog.write(level_t::Info, "Total Socket Count: " + std::to_string(ss_socket.get_total_socket()), GET_SOURCE);
    ss_testlog.write(level_t::Info, "Port: " + std::to_string(ss_socket.get_port()), GET_SOURCE);
    ss_testlog.write(level_t::Info, "Socket Username: " + ss_socket.get_policy().get_username(), GET_SOURCE);

    ss_socket.get_policy().set_password("new-pwd-2026-netsocket");

    ss_testlog.write(level_t::Info, "Socket Password: " + ss_socket.get_policy().get_password(), GET_SOURCE);
    ss_testlog.write(level_t::Info, "Socket Old Password: " + ss_socket.get_policy().get_old_password(), GET_SOURCE);

    ss_socket.print();
    ss_testlog.print();

    return EXIT_FAILURE;
}
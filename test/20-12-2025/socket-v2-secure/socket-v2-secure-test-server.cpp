// Abdulkadir U. - 26/11/2025

/**
 * Server Socket (Sunucu Soketi)
 * 
 * Sunucu için başlatma, port ayarlama, dinleme sınırı,
 * dinleme zamanlayıcısı (ms) vb. özellikleri test edip
 * kontrol etmemizi sağlayacak bir test
 */

// Include:
#include <Algorithm/AlgorithmPool.h>
#include <ThreadPool/ThreadPool.h>
#include <Socket/Server/Server.h>

#include <File/Logger/Logger.h>
#include <Tool/Utf/Utf.h>
#include <Test/Test.h>

// Using Namespace:
using namespace core::algorithmpool;
using namespace core::socket;

// Function: Net Handler
auto NetHandler(NetPacket& _netpacket) noexcept
{
    std::cout << _netpacket.getUsername() << ": " << _netpacket.getMessage() << "\n";
    return e_server::succ_set_running;
}

// main
int main(void)
{
    // Windows UTF-8
    #if defined __PLATFORM_DOS__
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif

    Logger logger(U"Server Socket", U"../logs/server_socket");
    ThreadPool tpool;

    const socket_port_t portaddr = 9876;

    Server<Xor> testserver(
        Xor(U"xor-test-key-123"),
        tpool,
        NetHandler,
        portaddr,
        false, true,
        U"Server Test 22/12/2025",
        utf::to_utf32("../logs/linux-server-test-22122025-" + std::to_string(portaddr) + "-log")
    );

    LOG_EXPECT(logger, testserver.getPolicy().setPassword(U"Password@123!-_üçşğ"), e_accesspolicy::succ_set_password, U"Server Password Setted");
    LOG_MSG(logger, testserver.getPolicy().getPassword(), test::e_status::information, true);

    LOG_EXPECT(logger, testserver.hasError(), false, U"Server has no error");
    LOG_MSG(logger, U"Printing Server Socket informations...", test::e_status::information, true);

    testserver.print();

    LOG_EXPECT(logger, testserver.getPolicy().ban("127.0.0.1"), e_accesspolicy::succ_ip_addr_banned, U"Ip address banned");
    LOG_EXPECT(logger, testserver.getPolicy().canAllow("127.0.0.1"), e_accesspolicy::err_not_allow_ip, U"Ip address not allowing");

    std::this_thread::sleep_for(std::chrono::seconds(2));
    LOG_EXPECT(logger, testserver.getPolicy().unban("127.0.0.1"), e_accesspolicy::succ_ip_addr_ban_removed, U"Ip address ban removed");

    LOG_MSG(logger, U"Running Server Socket for 10 seconds...", test::e_status::information, true);
    auto fut_run = testserver.run();

    std::this_thread::sleep_for(std::chrono::seconds(10));
    auto fut_stop = testserver.stop();

    LOG_EXPECT(logger, fut_run.get(), e_server::succ_server_run, U"Server runned");
    LOG_EXPECT(logger, fut_stop, e_server::succ_server_close, U"Server closed");
}
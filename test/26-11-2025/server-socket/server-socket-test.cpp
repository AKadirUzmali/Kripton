// Abdulkadir U. - 26/11/2025

/**
 * Server Socket (Sunucu Soketi)
 * 
 * Sunucu için başlatma, port ayarlama, dinleme sınırı,
 * dinleme zamanlayıcısı (ms) vb. özellikleri test edip
 * kontrol etmemizi sağlayacak bir test
 */

// Include:
#include <Test/Test.h>
#include <File/Logger/Logger.h>
#include <Tool/Utf/Utf.h>

#include <Algorithm/AlgorithmPool.h>
#include <ThreadPool/ThreadPool.h>
#include <Socket/Server/Server.h>

// Using Namespace:
using namespace core::algorithmpool;
using namespace core::socket;

// main
int main(void)
{
    Logger logger(U"Server Socket", U"../logs/server_socket");
    ThreadPool tpool;

    auto handle = [&](const std::u32string& message = U"Client Message")
    {
        std::cout << utf::to_utf8(U"[Client] " + message) << "\n";
        return e_server::succ_set_running;
    };

    Server<Xor> testserver(Xor(U"xor-test-key-123"), tpool, handle, 8080);

    logger.log(testserver.hasError(), false, U"Server has no error");

    logger.log(utf::to_utf32(test::text_info + " ") + U"Printing Server Socket informations...");
    test::message(test::e_status::information, "Printing Server Socket informations...");
    logger.log(testserver.print(), e_socket::succ_socket_print, U"Server Socket informations printed");

    logger.log(testserver.getPolicy().ban("127.0.0.1"), e_accesspolicy::succ_ip_addr_banned, U"Ip address banned");
    logger.log(testserver.getPolicy().canAllow("127.0.0.1"), e_accesspolicy::err_not_allow_ip, U"Ip address not allowing");

    std::this_thread::sleep_for(std::chrono::seconds(2));
    logger.log(testserver.getPolicy().unban("127.0.0.1"), e_accesspolicy::succ_ip_addr_ban_removed, U"Ip address ban removed");

    logger.log(utf::to_utf32(test::text_info + " ") + U"Running Server Socket for 10 seconds...");
    auto fut_run = testserver.run();
    std::this_thread::sleep_for(std::chrono::seconds(10));
    auto fut_stop = testserver.stop();

    logger.log(fut_run.get(), e_server::succ_server_run, U"Server runned");
    logger.log(fut_stop.get(), e_server::succ_server_close, U"Server closed");
}
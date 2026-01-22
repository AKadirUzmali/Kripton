// Abdulkadir U. - 20/12/2025

/**
 * Client Test (İstemci Test)
 * 
 * Basit bir istemci oluşturup sunucuya mesaj
 * gidip gitmediğini test edeceğiz ama
 * sunucuya ait şifre ve kullanıcı adı ile veri
 * göndereceğiz ve bunu da sunucu kısmında kontrol
 * edeceğiz
 * 
 * NOT: Hata var şuan da, verinin boyutu düzgün olmadığından
 * veriyi düzgünce gönderemiyor ve sunucu tarafı da veriyi
 * işleyemediği için sunucu tarafında da problem oluşuyor.
 * Sunucu kapandığını söylüyor ama hala beklemede kalıyor ve
 * program sonlanmıyor. Sistem hatası vermesine neden oluyor.
 */

// Include:
#include <Global.h>
#include <Tool/Utf/Utf.h>
#include <Algorithm/AlgorithmPool.h>
#include <Socket/Socket.h>

#include <iostream>
#include <vector>
#include <array>
#include <thread>

// Using Namespace:
using namespace tool;
using namespace core::algorithmpool;
using namespace core::virbase;

// main
int main(void)
{
    // Windows UTF-8 & Winsock API
    #if defined __PLATFORM_DOS__
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);

        constexpr int inv_connect = SOCKET_ERROR;
    #elif defined __PLATFORM_POSIX__
        constexpr int inv_connect = -1;
    #endif

    const std::u32string logpath = utf::to_utf32("logs/" + utf::to_lower(platform::name()) + "/");
    const socket_port_t portaddr = 9876;

    using CryptT = Xor;
    CryptT crypto(U"cxor-test-key-123c");

    Socket<CryptT> client(
        crypto,
        U"Client-tester",
        portaddr,
        true,
        U"Socket Client Test",
        logpath + utf::to_utf32(utf::to_lower(platform::name()) + "-client-test-24122025-" + std::to_string(portaddr) + "-log")
    );

    e_socket cli_status = glo::to_status<e_socket>(client.create());
    if( cli_status != e_socket::succ_socket_create ) {
        LOG_MSG(client.getLogger(), utf::to_utf32("Failed To Create Socket | Code: " + std::to_string(static_cast<size_t>(cli_status))), test::e_status::error, true);
        return perror("socket"), 1;
    }

    LOG_MSG(client.getLogger(), U"Client Algorithm Key: " + client.getAlgorithm().getKey(), test::e_status::information, true);
    LOG_EXPECT(client.getLogger(), client.getAlgorithm().setKey(U"xor-test-key-123"), e_algorithm::succ_set_key, U"Client Algorithm Key Changed");
    LOG_MSG(client.getLogger(), U"Client Algorithm Key After Set: " + client.getAlgorithm().getKey(), test::e_status::information, true);

    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(client.getPort());
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if( ::connect(client.getSocket(), (sockaddr*)&addr, sizeof(addr)) == inv_connect )
    {
        #if defined __PLATFORM_DOS__
            int err = WSAGetLastError();
            std::cerr << "connect error: " << err << std::endl;
        #endif

        LOG_MSG(client.getLogger(), U"Failed To Connect Server | Code: ", test::e_status::error, true);
        return perror("connect"), 2;
    }

    datapacket_t net_datapacket {
        .pwd = U"Password@123!-_üçşğ_test",
        .name = U"Client_Test_Name",
        .msg = U"Hello Server 😁, It's Client! 😌"
    };

    cli_status = glo::to_status<e_socket>(client.send(client.getSocket(), net_datapacket));
    if( cli_status != e_socket::succ_socket_send )
        LOG_MSG(client.getLogger(), utf::to_utf32("Failed to send data to server | Code: " + std::to_string(static_cast<size_t>(cli_status))), test::e_status::error, true);

    cli_status = glo::to_status<e_socket>(client.receive(client.getSocket(), net_datapacket));
    if( cli_status != e_socket::succ_socket_recv )
        LOG_MSG(client.getLogger(), utf::to_utf32("Failed to receive data from server | Code: " + std::to_string(static_cast<size_t>(cli_status))), test::e_status::error, true);

    std::cout << "[CLIENT] Password: " << utf::to_utf8(net_datapacket.pwd) <<
        "\nUsername: " << utf::to_utf8(net_datapacket.name) <<
        "\nMessage: " << utf::to_utf8(net_datapacket.msg) << "\n\n";

    net_datapacket.pwd = U"Password@123!-_üçşğ";
    net_datapacket.name = U"Client_Test_Name 2";
    net_datapacket.msg = U"Hello Server 😁, It's Client 2! 😌";
    
    cli_status = glo::to_status<e_socket>(client.send(client.getSocket(), net_datapacket));
    if( cli_status != e_socket::succ_socket_send )
        LOG_MSG(client.getLogger(), utf::to_utf32("Failed to send data to server | Code: " + std::to_string(static_cast<size_t>(cli_status))), test::e_status::error, true);

    cli_status = glo::to_status<e_socket>(client.receive(client.getSocket(), net_datapacket));
    if( cli_status != e_socket::succ_socket_recv )
        LOG_MSG(client.getLogger(), utf::to_utf32("Failed to receive data from server | Code: " + std::to_string(static_cast<size_t>(cli_status))), test::e_status::error, true);

    net_datapacket.pwd = U"aPassword@123!-_üçşğb";
    net_datapacket.name = U"Client_Test_Name 3";
    net_datapacket.msg = U"Hello Server 😁, It's Client 3! 😌";
    
    cli_status = glo::to_status<e_socket>(client.send(client.getSocket(), net_datapacket));
    if( cli_status != e_socket::succ_socket_send )
        LOG_MSG(client.getLogger(), utf::to_utf32("Failed to send data to server | Code: " + std::to_string(static_cast<size_t>(cli_status))), test::e_status::error, true);

    cli_status = glo::to_status<e_socket>(client.receive(client.getSocket(), net_datapacket));
    if( cli_status != e_socket::succ_socket_recv )
        LOG_MSG(client.getLogger(), utf::to_utf32("Failed to receive data from server | Code: " + std::to_string(static_cast<size_t>(cli_status))), test::e_status::error, true);

    if( cli_status == e_socket::succ_socket_recv ) {
        LOG_MSG(client.getLogger(),
            U"[CLIENT RECV] Password: " + net_datapacket.pwd
            + U"\nUsername: " + net_datapacket.name
            + U"\nMessage: " + net_datapacket.msg,
            test::e_status::information,
            false
        );
    }
    else {
        LOG_MSG(client.getLogger(),
            U"Client Data Recive Failed",
            test::e_status::error,
            false
        );
    }

    std::this_thread::sleep_for(std::chrono::seconds(5));
}
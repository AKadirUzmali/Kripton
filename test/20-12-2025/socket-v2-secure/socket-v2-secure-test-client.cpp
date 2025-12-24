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

        WSADATA wsadata {};
        WSAStartup(MAKEWORD(2, 2), &wsadata);

        constexpr int inv_connect = SOCKET_ERROR;
    #elif defined __PLATFORM_POSIX__
        constexpr int inv_connect = -1;
    #endif

    Xor testxor(U"xor-test-key-123");
    const socket_port_t portaddr = 9876;

    Socket<Xor> client(
        std::forward<Xor>(testxor),
        U"Client-tester",
        portaddr,
        true,
        U"Socket Client Test",
        utf::to_utf32("../logs/linux-client-test-24122025-" + std::to_string(portaddr) + "-log")
    );

    if( client.create() != e_socket::succ_socket_create )
        return perror("socket"), 1;

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

        return perror("connect"), 2;
    }

    datapacket_t net_datapacket {
        .pwd = U"Password@123!-_üçşğ",
        .name = U"Client_Test_Name",
        .msg = U"Hello Server 😁, It's Client! 😌"
    };

    e_socket cli_status = glo::to_status<e_socket>(client.send(client.getSocket(), net_datapacket));
    if( cli_status != e_socket::succ_socket_send )
        return perror("send"), 3;

    std::cout << "[CLIENT] Password: " << utf::to_utf8(net_datapacket.pwd) <<
        "\nUsername: " << utf::to_utf8(net_datapacket.name) <<
        "\nMessage: " << utf::to_utf8(net_datapacket.msg) << "\n\n";

    cli_status = glo::to_status<e_socket>(client.receive(client.getSocket(), net_datapacket));
    if( cli_status != e_socket::succ_socket_recv )
        return perror("recv"), 4;

    std::cout << "[CLIENT] Password: " << utf::to_utf8(net_datapacket.pwd) <<
        "\nUsername: " << utf::to_utf8(net_datapacket.name) <<
        "\nMessage: " << utf::to_utf8(net_datapacket.msg) << "\n\n";

    #if defined __PLATFORM_DOS__
        WSACleanup();
    #endif
}
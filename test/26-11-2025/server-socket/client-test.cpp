// Abdulkadir U. - 26/11/2025

/**
 * Client Test (İstemci Test)
 * 
 * Basit bir istemci oluşturup sunucuya mesaj
 * gidip gitmediğini test edeceğiz
 */

// Include:
#include <Platform/Platform.h>
#include <Tool/Utf/Utf.h>
#include <Algorithm/AlgorithmPool.h>
#include <Socket/Socket.h>

#include <iostream>
#include <vector>

// Using Namespace:
using namespace tool;
using namespace core::algorithmpool;

// main
int main(void)
{
    // Windows UTF-8 & Winsock API
    #if defined __PLATFORM_DOS__
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);

        WSADATA wsadata {};
        WSAStartup(MAKEWORD(2, 2), &wsadata);
    #endif

    Xor testxor(U"xor-test-key-123");

    socket_t sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if( sock == invalid_socket )
        return perror("socket"), 1;

    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9876);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if( ::connect(sock, (sockaddr*)&addr, sizeof(addr)) == invalid_connect )
    {
        #if defined __PLATFORM_DOS__
            int err = WSAGetLastError();
            std::cerr << "connect error: " << err << std::endl;
        #endif

        return perror("connect"), 2;
    }

    std::u32string msg = U"Hello Server 😁, It's Client! 😌";
    std::string rawmsg = utf::to_utf8(msg);

    // std::u32string msg =U"😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a 1 😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a 2 😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a 3 😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a 4 😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a 5 😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a 6 😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a 7 😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a 8 😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a 9 😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a 10 😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a 11 😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a 12 😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a 13 😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a 14 😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a 15 😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a 16 😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a 17 😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a 18 😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a 19 😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a😑😒😓😔😕abcdef😑😒😓😔😕a 20";
    testxor.encrypt(msg);

    std::string msgstr = tool::utf::to_utf8(msg);
    ::send(sock, msgstr.data(), static_cast<int>(msgstr.size()), 0);

    std::vector<char> buffer(1024);

    int recv_bytes = ::recv(sock, buffer.data(), static_cast<int>(buffer.size()), 0);
    if( recv_bytes == 0 )
    {
        std::cout << "[RECV] Connection closed by remote host\n";
        close_socket(sock);
        return 3;
    }
    else if( recv_bytes < 0 )
    {
        std::cout << "[RECV] Receive data error\n";
        close_socket(sock);
        return 4;
    }
    std::cout << "[RECV] Got " << recv_bytes << " bytes\n";

    std::u32string reply = tool::utf::to_utf32(std::string(buffer.data(), static_cast<size_t>(recv_bytes)));
    testxor.decrypt(reply);

    std::cout << tool::utf::to_utf8(reply) << std::endl;
    close_socket(sock);

    #if defined __PLATFORM_DOS__
        WSACleanup();
    #endif
}
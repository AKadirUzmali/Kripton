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
#include <Platform/Platform.h>
#include <Tool/Utf/Utf.h>
#include <Algorithm/AlgorithmPool.h>
#include <Socket/Socket.h>

#include <iostream>
#include <vector>
#include <array>

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

        constexpr int inv_connect = SOCKET_ERROR;
    #elif defined __PLATFORM_POSIX__
        constexpr int inv_connect = -1;
    #endif

    Xor testxor(U"xor-test-key-123");

    socket_t sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if( sock == inv_socket )
        return perror("socket"), 1;

    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9876);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if( ::connect(sock, (sockaddr*)&addr, sizeof(addr)) == inv_connect )
    {
        #if defined __PLATFORM_DOS__
            int err = WSAGetLastError();
            std::cerr << "connect error: " << err << std::endl;
        #endif

        return perror("connect"), 2;
    }

    const std::u32string pwd = U"Password@123!-_üçşğ";
    const std::u32string nick = U"Client_Test_Name";
    const std::u32string msg = U"Hello Server 😁, It's Client! 😌";

    NetPacket netpack(pwd, nick, msg);

    std::u32string u32pack;
    netpack.copy(u32pack);
    testxor.encrypt(u32pack);
    netpack.pack(u32pack);

    ::send(sock, netpack.get().data(), static_cast<int>(netpack.get().size()), 0);

    std::array<char, SIZE_SOCKET_TOTAL> buffer {};

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

    const std::u32string u32conv = reply;
    netpack.pack(u32conv);

    std::cout << "Password: " << netpack.getPassword() << "\n"
        << "Username: " << netpack.getUsername() << "\n"
        << "Message: " << netpack.getMessage() << "\n";

    close_socket(sock);

    #if defined __PLATFORM_DOS__
        WSACleanup();
    #endif
}
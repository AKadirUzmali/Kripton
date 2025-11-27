// Abdulkadir U. - 26/11/2025

/**
 * Client Test (İstemci Test)
 * 
 * Basit bir istemci oluşturup sunucuya mesaj
 * gidip gitmediğini test edeceğiz
 */

// Include:
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>

#include <Tool/Utf/Utf.h>
#include <Algorithm/AlgorithmPool.h>

// Using Namespace:
using namespace tool;
using namespace core::algorithmpool;

// main
int main(void)
{
    Xor testxor(U"xor-test-key-123");

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if( sock < 0 )
        return perror("socket"), 1;

    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if( connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0 )
        return perror("connect"), 2;

    std::u32string msg = U"Hello Server 😁, It's Client! 😌";
    testxor.encrypt(msg);

    send(sock, msg.data(), msg.size() * sizeof(char32_t), 0);

    std::vector<char32_t> buffer(1024);

    int r = recv(sock, buffer.data(), buffer.size() * sizeof(char32_t), 0);
    if( r <= 0 )
        return perror("recv"), close(sock), 3;

    buffer.resize(r / sizeof(char32_t));
    
    std::u32string reply(buffer.begin(), buffer.end());
    testxor.decrypt(reply);

    std::cout << tool::utf::to_utf8(reply) << std::endl;
    close(sock);
}
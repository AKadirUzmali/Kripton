// Abdulkadir U. - 13/12/2025

/**
 * Windows Server (WinSocket)
 * 
 * Windows için sunucu yapısı oluşturup bunu test ediyoruz
 */

// Include:
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <array>
#include <chrono>
#include <thread>
#include <csignal>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <windows.h>

// Static:
static WSAData wsadata;
static SOCKET servsock;

// Signal Cleaner:
void signal_cleaner(int _sig) {
    if( servsock != INVALID_SOCKET )
        closesocket(servsock);

    WSACleanup();

    std::cout << "[ERROR] Signal Code: " << std::to_string(_sig) << std::endl;
    std::exit(EXIT_FAILURE);
}

// main
int main(void)
{
    std::signal(SIGSEGV, signal_cleaner);
    std::signal(SIGABRT, signal_cleaner);
    std::signal(SIGFPE,  signal_cleaner);
    std::signal(SIGILL,  signal_cleaner);
    std::signal(SIGTERM, signal_cleaner);
    std::signal(SIGINT,  signal_cleaner);

    int wsaerr = WSAStartup(MAKEWORD(2,2), &wsadata);
    if( wsaerr != 0 ) {
        std::cout << "[SERVER] Winsock dll not found" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "[SERVER] Winsock dll found" << std::endl;

    servsock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if( servsock == INVALID_SOCKET ) {
        std::cout << "[SERVER] Winsock create error, code: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return EXIT_FAILURE;
    }
    std::cout << "[SERVER] Winsock created successfully" << std::endl;

    sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(36366);

    if( ::bind(servsock, reinterpret_cast<sockaddr*>(&servaddr), sizeof(servaddr)) == SOCKET_ERROR ) {
        std::cout << "[SERVER] Winsock bind error, code: " << WSAGetLastError() << std::endl;
        closesocket(servsock);
        WSACleanup();
        return EXIT_FAILURE;
    }
    std::cout << "[SERVER] Winsock bind successfull" << std::endl;

    if( ::listen(servsock, 5) == SOCKET_ERROR ) {
        std::cout << "[SERVER] Winsock listen error, code: " << WSAGetLastError() << std::endl;
        closesocket(servsock);
        WSACleanup();
        return EXIT_FAILURE;
    }
    std::cout << "[SERVER] Winsock listen successfull" << std::endl;

    SOCKET accptsock = ::accept(servsock, nullptr, nullptr);
    if( accptsock == INVALID_SOCKET ) {
        std::cout << "[SERVER] Winsock accept error, code: " << WSAGetLastError() << std::endl;
        closesocket(servsock);
        WSACleanup();
        return EXIT_FAILURE;
    }
    std::cout << "[SERVER] Winsock accept successfull" << std::endl;

    while( true ) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        std::array<char, 128> buffer;

        std::cout << "Enter the message: ";
        std::cin.getline(buffer.data(), buffer.size());

        int sendbyte = ::send(accptsock, buffer.data(), buffer.size(), 0);
        if( sendbyte == SOCKET_ERROR ) {
            std::cout << "[SERVER] Winsock send error, code: " << WSAGetLastError() << std::endl;
            closesocket(servsock);
            WSACleanup();
            return EXIT_FAILURE;
        }
        std::cout << "[SERVER] Winsock send successfull, send bytes: " << sendbyte << std::endl;

        int recvbyte = ::recv(accptsock, buffer.data(), buffer.size(), 0);
        if( recvbyte == 0 ) {
            std::cout << "[SERVER] Winsock socket closed" << std::endl;
            closesocket(accptsock);
            break;
        }
        else if( recvbyte < 0 ) {
            std::cout << "[SERVER] Winsock recv error, code: " << WSAGetLastError() << std::endl;
            closesocket(accptsock);
            break;
        }
        std::cout << "[SERVER] Winsock receive successfull, received data: " << buffer.data() << std::endl;
    }

    WSACleanup();
    return EXIT_SUCCESS;
}
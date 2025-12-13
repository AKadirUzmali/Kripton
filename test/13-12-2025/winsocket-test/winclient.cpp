// Abdulkadir U. - 13/12/2025

/**
 * Windows Client (WinSocket)
 * 
 * Windows için istemci yapısı oluşturup bunu test ediyoruz
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
static SOCKET clisock;

// Signal Cleaner:
void signal_cleaner(int _sig) {
    if( clisock != INVALID_SOCKET )
        closesocket(clisock);

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
        std::cout << "[CLIENT] Winsock dll not found" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "[CLIENT] Winsock dll found" << std::endl;

    clisock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if( clisock == INVALID_SOCKET ) {
        std::cout << "[CLIENT] Winsock create error, code: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return EXIT_FAILURE;
    }
    std::cout << "[CLIENT] Winsock created successfully" << std::endl;

    sockaddr_in cliaddr;
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    cliaddr.sin_port = htons(36366);

    if( ::connect(clisock, reinterpret_cast<sockaddr*>(&cliaddr), sizeof(cliaddr)) == SOCKET_ERROR ) {
        std::cout << "[CLIENT] Winsock connect error, code: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return EXIT_FAILURE;
    }
    std::cout << "[CLIENT] Winsock connect successfully" << std::endl;

    std::array<char, 128> buffer;

    std::cout << "Enter the message: ";
    std::cin.getline(buffer.data(), buffer.size());

    int sendbyte = ::send(clisock, buffer.data(), buffer.size(), 0);
    if( sendbyte == SOCKET_ERROR ) {
        std::cout << "[SERVER] Winsock send error, code: " << WSAGetLastError() << std::endl;
        closesocket(clisock);
        WSACleanup();
        return EXIT_FAILURE;
    }
    std::cout << "[SERVER] Winsock send successfull, send bytes: " << sendbyte << std::endl;

    int recvbyte = ::recv(clisock, buffer.data(), buffer.size(), 0);
    if( recvbyte == 0 ) {
        std::cout << "[SERVER] Winsock socket closed" << std::endl;
        closesocket(clisock);
        WSACleanup();
        return EXIT_FAILURE;
    }
    else if( recvbyte < 0 ) {
        std::cout << "[SERVER] Winsock recv error, code: " << WSAGetLastError() << std::endl;
        closesocket(clisock);
        WSACleanup();
        return EXIT_FAILURE;
    }
    std::cout << "[SERVER] Winsock receive successfull, received data: " << buffer.data() << std::endl;

    return EXIT_SUCCESS;
}
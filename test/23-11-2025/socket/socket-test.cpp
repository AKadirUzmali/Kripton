// Abdulkadir U. - 23/11/2025

/**
 * Socket Test (Soket Testi)
 * 
 * Soket ile sunucu oluşturmak ya da istemci ile
 * sunucuya bağlanmayı sağlayacak sistemi tasarlamadan
 * önce soket yapısının düzgün çalışıp çalışmadığını test etmeliyiz.
 */

// Include:
#include <Test/Test.h>
#include <File/Logger/Logger.h>
#include <Tool/Utf/Utf.h>

#include <Algorithm/AlgorithmPool.h>
#include <Socket/Socket.h>

// Using Namespace:
using namespace core;
using namespace virbase;
using namespace tool;
using namespace algorithmpool;

// Using:
using CryptType = Xor;
using SockType = socket::Tcp;

// Logger
static Logger _logger(U"Socket Test", U"socket_test");

// main
int main(void)
{
    // nesneyi oluştursun
    auto sockobj = Socket<CryptType, SockType>(CryptType{ U"socket-test-crypt-key-23112025" });

    // soket de hata olup olmadığını kontrol etsin
    _logger.log(sockobj.hasError(), false, U"Socket has no error");
    _logger.log(sockobj.isTcp(), true, U"Socket type is Tcp");
    _logger.log(sockobj.create(), e_socket::succ_socket_create, U"Socket created");
    _logger.log(sockobj.getSocket() != invalid_socket, true, U"Socket is valid");
    _logger.log(sockobj.getPort(), invalid_port, U"Socket port is not valid (No Connection | Just Base Class)");

    // bilgi çıktısı
    _logger.log(utf::to_utf32(test::text_info + " ") + U"Printing Socket informations...");
    test::message(test::e_status::information, "Printing Socket informations...");

    _logger.log(sockobj.print(), e_socket::succ_socket_print, U"Socket informations printed");

    // soket türünü değiştirsin, udp yapsın
    _logger.log(sockobj.setSocketType(e_socket_type::udp), e_socket::succ_socket_set_type_udp, U"Socket type changed to Udp from Tcp");
    _logger.log(sockobj.getSocketType(), e_socket_type::udp, U"Verified that the socket type is Udp");

    // bilgi çıktısı
    _logger.log(utf::to_utf32(test::text_info + " ") + U"Printing Socket informations...");
    test::message(test::e_status::information, "Printing Socket informations...");

    _logger.log(sockobj.print(), e_socket::succ_socket_print, U"Socket informations printed");

    // soket kapatma işlemi ve kontrolü
    _logger.log(sockobj.close(), e_socket::succ_socket_close, U"Socket closed");
    _logger.log(sockobj.isClose(), true, U"Verified the socket closed");

    // soket temizleme işlemi ve kontrolü
    _logger.log(sockobj.clear(), e_socket::succ_socket_clear, U"Socket cleared");
    _logger.log(sockobj.isFree(), true, U"Socket is free");

    // bilgi çıktısı
    _logger.log(utf::to_utf32(test::text_info + " ") + U"Printing Socket informations...");
    test::message(test::e_status::information, "Printing Socket informations...");

    _logger.log(sockobj.print(), e_socket::succ_socket_print, U"Socket informations printed");

    // test sonu
    test::message(test::e_status::information, "Test is ending...");
}
// Abdulkadir U. - 16/11/2025

/**
 * Logger Test (Kayıt Edici Testi)
 * 
 * Kayıt edici sistemin işe yarayıp yaramadığını
 * test ederek sonrasında daha rahat işlemler için
 * kullanılabilirliğini sağlaamayı denemek.
 */

// Include:
#include <Test/Test.h>
#include <File/Logger/Logger.h>
#include <Tool/Utf/Utf.h>

// Using Namespace:
using namespace core;
using namespace subcore;
using namespace tool;

// main
int main(void)
{
    // Logger Oluşturma
    Logger logger(U"LoggerTest", U"logger_test_" + utf::to_utf32(utf::to_lower(platform::name())));

    // sınıfın doğru oluşturulduğunu ve adların aynı olduğunu kontrol et
    test::exit_eq(logger.getName(), U"LoggerTest", "Logger name is equals");
    logger.log(utf::to_utf32(test::text_pass + ' ' + "Logger name is equals"));

    // hata olup olmadığını kontrol et
    test::exit_eq(logger.hasError(), false, "Logger has no error");
    logger.log(utf::to_utf32(test::text_pass + ' ' + "Logger has no error"));

    // sonlandır
    test::message(test::e_status::warning, "Logger test is ending...");
}
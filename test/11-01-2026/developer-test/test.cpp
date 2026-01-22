// Abdulkadir U. - 11/01/2026
// Abdulkadir U. - 12/01/2026

/**
 * Herhangi bir işlem yapmadan sadece
 * ana header dosyasınını içe aktarıp
 * derleyip test edeceğiz, hata olup olmamasını
 * kontrol etmiş olacağız
 */

// Include:
#define __DEVEL__
#include <developer/Developer.h>

// Using Namespace:
using namespace dev::level;
using namespace dev::log;
using namespace dev::output::file;
using namespace dev::output::console;

// Static:
static std::string filename = "11-01-2026-developer-" + tools::utf::to_lower(std::string(tools::os::current_os_name()));
static Logger<FileOut, ConsoleOut> testlog("logs/" + filename, filename + "-console");

/**
 * @brief Even or Odd (Tek ya da Çift)
 * 
 * Çift mi tek mi olduğunu ölçen basit bir fonksiyon
 * 
 * @return bool
 */
static bool isEven(const int value) noexcept
{
    TRACE_SCOPE(testlog, "is_even");
    LOG(testlog, "is_even çalıştırıldı");
    EXPECT_MSG(testlog, value % 2 == 0, "Sayi bir çift sayi");

    return value % 2 == 0;
}

// main
int main(void)
{
    tools::console::enable_utf8_console();

    // TRACE_FUNC();
    LOG(testlog, "Developer test sistemi başlatıldı");

    int a = 23;
    int b = 21;

    if( isEven(a) ) LOG_MSG(testlog, Level::Info, "A sayısı bir çift sayıdır");
    else LOG_MSG(testlog, Level::Warn, "A sayısı bir çift sayı değildir");

    ASSERT_MSG(testlog, !isEven(b), "B sayısı bir tek sayıdır");

    LOG(testlog, "Test sonuçları çıktısı veriliyor...");
    testlog.print();

    // ASSERT_MSG(isEven(b), "B sayisi bir cift sayidir");

    return 0;
}
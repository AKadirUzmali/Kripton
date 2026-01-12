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
#include <kits/ToolKit.h>

/**
 * @brief Even or Odd (Tek ya da Çift)
 * 
 * Çift mi tek mi olduğunu ölçen basit bir fonksiyon
 * 
 * @return bool
 */
static bool isEven(const int value) noexcept
{
    TRACE_SCOPE("is_even");
    LOG("is_even çalıştırıldı");
    EXPECT_MSG(value % 2 == 0, "Sayi bir çift sayi");

    return value % 2 == 0;
}

// main
int main(void)
{
    tools::console::enable_utf8_console();

    TRACE_FUNC();
    LOG("Developer test sistemi başlatıldı");

    int a = 23;
    int b = 21;

    if( isEven(a) ) LOG_MSG(dev::level::Level::Info, "A sayısı bir çift sayıdır");
    else LOG_MSG(dev::level::Level::Warn, "A sayısı bir çift sayı değildir");

    ASSERT_MSG(!isEven(b), "B sayısı bir tek sayıdır");

    LOG("Test sonuçları çıktısı veriliyor...");
    dev::log::Logger::print();

    // ASSERT_MSG(isEven(b), "B sayisi bir cift sayidir");

    return 0;
}
// Abdulkadir U. - 28/10/2025

/**
 * Xor Algorithm
 * 
 * Algoritma sınıfının yapısını ve bu sınıfı
 * sanal olarak kullanan Xor şifreleme yönteminin
 * doğruluğunu test edeceğiz
 */

// Include:
#include <exception>
#include <sstream>

#include <Test/Test.h>
#include <Platform/Platform.h>

#include <Algorithm/Xor/Xor.h>
using namespace core::crypt::algorithm;

// main
int main(void)
{
    // platform kontrolü
    test::exit_eq(platform::is_platform(), true, "Operating System Supporting");

    // Xor nesnesi
    Xor algo_xor(U"Test");

    // nesnenin adını getirsin ve kontrol etsin
    test::exit_eq(algo_xor.getName(), Xor::NAME, "Algorithm Name Matches");

    // nesne anahtarını kontrol etsin
    test::exit_eq(Xor::isValidKey( algo_xor.getKey() ), true, "Algorithm Key Is Valid");

    // veriyi şifrelemeden önce orijinalini yedekte tutuyoruz
    std::u32string tmp_text = U"Hello from FreeBSD, Testing Xor Algorithm";
    std::u32string tmp_copytext = tmp_text;

    // veriyi şifrelesin
    algo_xor.encrypt(tmp_copytext);

    // eğer veri şifrelenmiş ise orijinal metinle uyuşmamalı
    test::exit_eq(tmp_copytext != tmp_text, true, "Text Is Encrypted");

    // verinin şifreli ve orijinal halini çıktı versin
    test::info_msg(test::to_visible(tmp_text));
    test::info_msg(test::to_visible(tmp_copytext));

    // şifrelenmiş veriyi çözsün ve uyumluluğu kontrol etsin
    algo_xor.decrypt(tmp_copytext);
    test::exit_eq(tmp_copytext, tmp_text, "Text Is Decrypted");

    // test sonlandırılıyor
    test::warn_msg("Test Is Ending");

    return 0;
}
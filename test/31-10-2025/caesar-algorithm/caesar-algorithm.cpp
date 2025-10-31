// Abdulkadir U. - 31/10/2025

/**
 * Caesar Algorithm
 * 
 * Algoritma sınıfının yapısını ve bu sınıfı
 * sanal olarak kullanan Caesar şifreleme yönteminin
 * doğruluğunu test edeceğiz
 */

// Include:
#include <exception>
#include <sstream>

#include <Test/Test.h>
#include <Platform/Platform.h>

#include <Algorithm/Caesar/Caesar.h>
using namespace core::crypt::algorithm;

// main
int main(void)
{
    // platform kontrolü
    test::exit_eq(platform::is_platform(), true, "Operating System Supporting");

    // Caesar nesnesi
    Caesar algo_caesar(U"");

    // nesnenin adını getirsin ve kontrol etsin
    test::exit_eq(algo_caesar.getName(), Caesar::NAME, "Algorithm Name Matches");

    // nesne anahtarını kontrol etsin
    bool status_key = test::expect_eq(Caesar::checkValidKey( algo_caesar.getKey() ), true, "Algorithm Key Is Valid");

    // eski anahtarı depolasın
    std::u32string tmp_old_key = algo_caesar.getKey();

    // eğer şifreleme anahtarı hatası varsa doğru gidiyoruz
    if( !status_key )
        algo_caesar.setKey(U"new-caesar-valid-key");

    // nesne anahtarı değişti mi kontrol etsin
    test::exit_eq(algo_caesar.getKey() != tmp_old_key, true, "Algorithm Key Changed");

    // nesne anahtarını kontrol etsin
    test::exit_eq(Caesar::checkValidKey( algo_caesar.getKey() ), true, "Algorithm Key Is Valid");

    // veriyi şifrelemeden önce orijinalini yedekte tutuyoruz
    std::u32string tmp_text = U"Hello from FreeBSD, Testing Caesar Algorithm";
    std::u32string tmp_copytext = tmp_text;

    // veriyi şifrelesin
    algo_caesar.encrypt(tmp_copytext);

    // eğer veri şifrelenmiş ise orijinal metinle uyuşmamalı
    test::exit_eq(tmp_copytext != tmp_text, true, "Text Is Encrypted");

    // verinin şifreli ve orijinal halini çıktı versin
    test::message(test::e_status::information, test::to_visible(tmp_text));
    test::message(test::e_status::information, test::to_visible(tmp_copytext));

    // şifrelenmiş veriyi çözsün ve uyumluluğu kontrol etsin
    algo_caesar.decrypt(tmp_copytext);
    test::exit_eq(tmp_copytext, tmp_text, "Text Is Decrypted");

    // algoritmanın ismini değiştirme
    std::string tmp_oldname(algo_caesar.getName());
    algo_caesar.setName("New Caesar Algo");

    test::expect_eq(tmp_oldname != algo_caesar.getName(), true, "Algorithm Name Changed");

    std::stringstream ss;
    ss << "Algorithm New Name Is: \"" << algo_caesar.getName() << "\""; 
    test::message(test::e_status::information, ss.str());
    ss.clear();

    // algoritmanın anahtarını değiştirme
    std::u32string tmp_oldkey(algo_caesar.getKey());
    algo_caesar.setKey(U"NebawwCabfawrKeyüği.çşq32ü");

    test::exit_eq(tmp_oldkey != algo_caesar.getKey(), true, "Algorithm Key Changed");
    test::message(test::e_status::information, test::to_visible(algo_caesar.getKey()));

    // veriyi şifrelesin
    tmp_copytext = tmp_text;
    algo_caesar.encrypt(tmp_copytext);

    // eğer veri şifrelenmiş ise orijinal metinle uyuşmamalı
    test::exit_eq(tmp_copytext != tmp_text, true, "Text Is Encrypted With New Key");

    // verinin yeni şifreli halini ve orijinal halini çıktı versin
    test::message(test::e_status::information, test::to_visible(tmp_text));
    test::message(test::e_status::information, test::to_visible(tmp_copytext));

    // yeni şifrelenmiş veriyi çözsün ve uyumluluğu kontrol etsin
    algo_caesar.decrypt(tmp_copytext);
    test::exit_eq(tmp_copytext, tmp_text, "Text Is Decrypted With New Key");

    // test sonlandırılıyor
    test::message(test::e_status::warning, "Test Is Ending");

    return 0;
}
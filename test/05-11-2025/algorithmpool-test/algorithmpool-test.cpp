// Abdulkadir U. - 05/11/2025

/**
 * Algorithm Pool Test
 * 
 * Algoritma havuzunda bulunan birden fazla
 * miktarda olan şifrelme yöntemlerini kullanmayı
 * basitçe deneyeceğiz
 */

// Include:
#include <sstream>

#include <Test/Test.h>
#include <Algorithm/AlgorithmPool.h>
namespace algopool = core::algorithmpool;

// main
int main(void)
{
    // algoritmaları oluştursun
    algopool::Xor crypto_xor(U"Aboov");
    algopool::Caesar crypto_caesar(crypto_xor.getKey());

    // hata varsa bildirsin ama sonlandırmasın
    // hata olmasına rağmen nasıl çalıştığını test
    // edeceğiz
    test::expect_eq(crypto_xor.hasError(), false, "Xor is normal");
    test::expect_eq(crypto_caesar.hasError(), false, "Caesar is normal");

    // şifreleme için sabit bir metin
    const std::u32string metin_ilk(U"abcdef");
    const std::u32string metin_iki(U"123456");

    // şifrelemeden önce orijinal metini ve anahtarları çıktı versin
    {
        std::stringstream ss;
        ss << "Xor key is: " << test::to_visible(crypto_xor.getKey()) << " | ";
        ss << "Xor original text is: " << test::to_visible(metin_ilk);
        test::message(test::e_status::information, ss.str());
    }

    {
        std::stringstream ss;
        ss << "Caesar key is: " << test::to_visible(crypto_caesar.getKey()) << " | ";
        ss << "Caesar original text is: " << test::to_visible(metin_iki);
        test::message(test::e_status::information, ss.str());
    }

    // bir test metini oluştursun
    std::u32string testmetin_xor(U"");
    std::u32string testmetin_caesar(testmetin_xor);

    // bu metini şifrelemeyi denesin
    test::expect_eq(crypto_xor.encrypt(testmetin_xor), true, "Xor encrypted empty text");
    test::expect_eq(crypto_caesar.encrypt(testmetin_caesar), true, "Caesar encrypted empty text");

    // metini doldursun
    testmetin_xor.assign(metin_ilk);
    testmetin_caesar.assign(metin_iki);

    // şifrelemeyi denesin
    bool result_xor = crypto_xor.encrypt(testmetin_xor) && testmetin_xor != metin_ilk;
    bool result_caesar = crypto_caesar.encrypt(testmetin_caesar) && testmetin_caesar != metin_iki;

    test::expect_eq(result_xor, true, "Xor encrypted text");
    test::expect_eq(result_caesar, true, "Caesar encrypted text");

    // metinlerin çıktısını versin
    if( result_xor ) {
        std::stringstream ss;
        ss << "Xor encrypted text is: " << test::to_visible(testmetin_xor);
        test::message(test::e_status::information, ss.str());
    }

    if( result_caesar ) {
        std::stringstream ss;
        ss << "Caesar encrypted text is: " << test::to_visible(testmetin_caesar);
        test::message(test::e_status::information, ss.str());
    }

    // metinleri çözümleyip eski halleri ile kıyaslasın
    result_xor = crypto_xor.decrypt(testmetin_xor) && testmetin_xor == metin_ilk;
    result_caesar = crypto_caesar.decrypt(testmetin_caesar) && testmetin_caesar == metin_iki;

    test::expect_eq(result_xor, true, "Text decrypted with Xor");
    test::expect_eq(result_caesar, true, "Text decrypted with Caesar");

    // metinlerin çıktısını versin
    if( result_xor ) {
        std::stringstream ss;
        ss << "Xor decrypted text is: " << test::to_visible(testmetin_xor);
        test::message(test::e_status::information, ss.str());
    }

    if( result_caesar ) {
        std::stringstream ss;
        ss << "Caesar decrypted text is: " << test::to_visible(testmetin_caesar);
        test::message(test::e_status::information, ss.str());
    }

    // temizlendiklerini kontrol etsin
    test::expect_eq(crypto_xor.clear(), true, "Xor cleared");
    test::expect_eq(crypto_caesar.clear(), true, "Caesar cleared");

    // test sonu
    test::message(test::e_status::warning, "Test is ending");

    return EXIT_SUCCESS;
}
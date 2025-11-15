// Abdulkadir U. - 14/11/2025

/**
 * File Test (Dosya Testi)
 * 
 * Dosya açma ve veri yazma işlemlerini yapmamızı
 * sağlayacak olan temel File sınıfını test edeceğiz.
 * Herhangi bir sorun içermesi durumunda hataları düzeltip
 * tekrar test edeceğiz. Genel kullanım amacı olarak:
 * Test sonuçlarını kayıt etme, gelecekte kullanabilmek
 * için veri kaydetme imkanı sağlamak, dosya işlemleri
 * yapmayı sağlamak bulunmaktadır.
 */

// Include:
#include <Platform/Platform.h>
#include <Test/Test.h>
#include <File/File.h>

#include <sstream>

using namespace core;
using namespace file;

// main
int main(void)
{
    // işletim sistemi uyumluluğu testi
    test::exit_eq(platform::is_platform(), true, "Operating System Supporting");

    // dosyayı açsın
    File testfile(U"test.txt", e_io::read_write);

    // dosya da hata olup olmadığını test etsin
    test::exit_eq(testfile.hasError(), false, "File Has No Error");

    // diğer yan fonksiyonları test etsin
    test::exit_eq(testfile.hasFile(), true, "Has File");
    test::exit_eq(testfile.hasPath(), true, "Has Path");

    test::expect_eq(testfile.isOpen(), true, "Is Open");
    test::expect_eq(testfile.isClose(), false, "Is Close");

    test::message(test::e_status::information, testfile.isAdd() ? "Add Mode" : "Not Add Mode");
    test::message(test::e_status::information, testfile.isRead() ? "Read Mode" : "Not Read Mode");
    test::message(test::e_status::information, testfile.isWrite() ? "Write Mode" : "Not Write Mode");

    std::stringstream tmp__ss;
    tmp__ss << "File Path Is: " << file::to_fs_path(testfile.getPath());
    test::message(test::e_status::information, tmp__ss.str());
    tmp__ss.clear();

    // kounumu dosyanın sonuna ayarlasın
    test::expect_eq(testfile.position(), e_file::succ_set_position, "File Position Setted To End Of File");

    // dosyadan yazıp okumak için sabit bir yazı belirlensin
    const std::u32string tmp__data(U"Merhaba Dünya 🌍");

    // veriyi dosyaya yazsın
    test::expect_eq(testfile.write(tmp__data), e_file::succ_write, "Data Writed To File");

    // eski konuma geri alsın
    std::streamoff tmp__offset = static_cast<std::streamoff>(tmp__data.length() * sizeof(char32_t));
    test::expect_eq(testfile.position(-tmp__offset), e_file::succ_set_position, "File Position Setted To Old Line");

    // veriyi dosyadan geri okusun
    std::u32string tmp__output;
    test::expect_eq(testfile.read(tmp__output), e_file::succ_read, "Data Readed From File");

    // okunan verileri karşılaştırsın
    test::expect_eq(tmp__data == tmp__output, true, "Writed And Readed Datas Are Equaling");

    // dosyayı kapatsın
    test::expect_eq(testfile.close(), e_file::succ_closed, "File Closed");

    // dosya da hata olup olmadığını kontrol etsin
    test::expect_eq(testfile.hasError(), false, "File Has No Error");

    // dosyayı temizlesin
    test::expect_eq(testfile.clear(), e_file::succ_clear, "File Successfully Cleared");

    // test bitiyor mesajı
    test::message(test::e_status::warning, "Test Is Ending...");

    // dosyaya ait bilgi çıktısı
    testfile.print();

    return 0;
}
// Abdulkadir U. - 16/11/2025
#pragma once

/**
 * Logger (Kaydedici)
 * 
 * Test işlemleri sonuçlarını görmek için ziyaretçilerin
 * oluşturduğumuz çalıştırılabilir dosyaları çalıştırmasına
 * gerek yok çünkü kötü amaçlar doğrultusunda işlemler
 * başkaları tarafından yapılabilir. Bunun aksine yeni
 * kayıt sistemi ile test işleminin yapıldığı işletim sistemi,
 * saat bilgisi, test numarası, ve yapılan işlemlerin sonuçları
 * belirlenen bir isimle oluşturulmuş dosyaya kaydedilir.
 * Bu sayede herkes işlemlerin sonucunda oluşan durumları
 * görüp değerlendirebilir.
 */

// Include:
#include <File/File.h>
#include <Tool/Utf/Utf.h>
#include <Platform/Platform.h>
#include <Test/Test.h>

#include <string>
#include <sstream>
#include <iomanip>

// Using Namespace:
using namespace core;
using namespace tool;

// Namespace: Sub Core
namespace subcore
{
    // Namespace: Logger
    namespace logger
    {
        // Enum Class: e_log
        enum class e_log : int
        {
            err_log_not_opened = 1000,
            err_not_set_key,
            err_name_empty,
            err_name_length_zero,
            err_set_name,
            err_path_empty,
            err_path_length_zero,
            err_set_path,
            err_log_not_ended,

            succ_logged = 2000,
            succ_set_key,
            succ_set_name,
            succ_set_path,
            succ_end,

            warn_log_already_exists = 3000
        };
    }

    // Using Namespace:
    using namespace logger;
    
    // Class: Logger
    class Logger : virtual public File
    {
        private:
            std::string key;
            std::u32string name;

            e_log setKey() noexcept;
            e_log setName(const std::u32string&) noexcept;

            void start() noexcept;
            void finish() noexcept;

        public:
            explicit Logger(
                const std::u32string& = U"log",
                const std::u32string& = U"output"
            ) noexcept;

            ~Logger() noexcept;

            const inline std::string& getKey() const noexcept;
            const inline std::u32string& getName() const noexcept;

            template<typename _Ltype, typename _Rtype>
            e_log log(_Ltype, _Rtype, const std::u32string&) noexcept;

            virtual e_log log(const std::u32string&) noexcept;
            virtual e_log end() noexcept;
    };
}

// Using Namespace:
using namespace subcore;
using namespace logger;

/**
 * @brief [Public] Constructor
 * 
 * Sınıfın oluşturucu. Varsayılan olarak
 * "output.log" dosyasını kullanır ama
 * istenirse farklı bir dosya adı da
 * verilebilir
 * 
 * @param u32string Logfilepath
 */
Logger::Logger(
    const std::u32string& _logname,
    const std::u32string& _logpath
) noexcept
: File(_logpath + utf::to_utf32(utf::to_lower("_" + platform::name() + ".log")), file::e_io::write)
{
    this->setName(_logname);
    this->setKey();
    this->start();
}

/**
 * @brief [Public] Destructor
 * 
 * Sınıfın yıkıcısı. Dosyayı kapatır
 * ve gerekli temizlik işlemlerini yapar
 */
Logger::~Logger() noexcept
{
    this->end();
}

/**
 * @brief [Private] Set Key
 * 
 * Log kayıt işlemimiz için özel bir anahtar
 * üretmemiz lazım ki gelecekte bu kayıtlara
 * daha çabuk ulaşabilelim.
 * 
 * İlk önce işletim sistemini tespit edip kısa
 * halde yazıyoruz ve o sistem için verilmiş örnek
 * kodu yazıyoruz.
 * 
 * @return e_logger
 */
e_log Logger::setKey() noexcept
{
    std::u32string lowered = this->name;
    utf::to_lower(lowered);

    for( auto& c32 : lowered )
        if( c32 == U' ')
            c32 = U'-';

    std::ostringstream oss;
    
    switch( platform::current() )
    {
        // Unix (BSD)
        case platform::e_os::Unix: oss << "unx"; break;
        // Linux
        case platform::e_os::Linux: oss << "lnx"; break;
        // Windows
        case platform::e_os::Windows: oss << "win"; break;
        // Unknown
        default: oss << "unk"; break;
    }

    oss << "-" << platform::current_date();
    oss << "-" << platform::current_time();
    oss << "-" << utf::to_utf8(lowered);

    this->key = oss.str();
    return !this->key.empty() && this->key == oss.str() ?
        e_log::succ_set_key : e_log::err_not_set_key;
}

/**
 * @brief [Private] Set Name
 * 
 * Log dosyasının ismini ayarlamak için
 * 
 * @param u32string name
 * @return e_logger
 */
e_log Logger::setName(const std::u32string& _name) noexcept
{
    if( _name.empty() ) return e_log::err_name_empty;
    if( _name.length() < 1 ) return e_log::err_name_length_zero;

    this->name = _name;
    return this->name == _name ?
        e_log::succ_set_name : e_log::err_set_name;
}

/**
 * @brief [Private] Start
 * 
 * Log kaydını başlatmak için
 * basit bir fonksiyon
 */
void Logger::start() noexcept
{
    if( !this->isOpen() && this->open() != file::e_file::succ_opened )
        return;

    this->write(U"Name: " + this->name + U"\n");
    this->write(utf::to_utf32("Os: " + platform::name() + "\n"));
    this->write(utf::to_utf32("Date: " + platform::current_date() + "\n"));
    this->write(utf::to_utf32("Key: " + this->key + "\n"));

    this->write(U"\n[LOGGER START]\n");
}

/**
 * @brief [Private] Finish
 * 
 * Log kaydını sonlandırmak için
 * basit bir fonksiyon
 */
void Logger::finish() noexcept
{
    if( !this->isOpen() && this->open() != file::e_file::succ_opened )
        return;

    this->write(U"[LOGGER END]\n");
}

/**
 * @brief [Public] Get Key
 * 
 * Log kaydının anahtarını almak için
 * 
 * @return string&
 */
const inline std::string& Logger::getKey() const noexcept
{
    return this->key;
}

/**
 * @brief [Public] Get Name
 * 
 * Log kaydının ismini almak için
 * 
 * @return u32string&
 */
const inline std::u32string& Logger::getName() const noexcept
{
    return this->name;
}

/**
 * @brief [Public] Log
 * 
 * Log dosyayı oluşturulduktan sonra
 * verilen kayıt işlemlerini dosyaya kaydetmemizi
 * sağlayacak olan kayıt fonksiyonu
 * 
 * @param string logtext
 * @return e_logger
 */
e_log Logger::log(const std::u32string& _logtext) noexcept
{
    if( !this->isOpen() )
        return e_log::err_log_not_opened;

    std::u32string msg_buffer = _logtext;

    this->write(utf::to_utf32(platform::current_time()) + U" ==> " + msg_buffer + U"\n");
    return e_log::succ_logged;
}

/**
 * @brief [Public] Log
 * 
 * Log dosyayı oluşturulduktan sonra
 * verilen kayıt işlemlerini dosyaya kaydetmemizi
 * sağlayacak olan kayıt fonksiyonu ve kayıt
 * öncesinden bilgilendirme amaçlı test çıktısını
 * verecektir.
 * 
 * @tparam _Ltype First
 * @tparam _Rtype Second
 * @param string Log Text
 * @return e_logger
 */
template<typename _Ltype, typename _Rtype>
e_log Logger::log(_Ltype _first, _Rtype _second, const std::u32string& _logtext) noexcept
{
    bool status = test::expect_eq(_first, _second, utf::to_utf8(_logtext));
    return this->log(utf::to_utf32(status ? test::text_pass : test::text_fail) + U" " + _logtext);
}

/**
 * @brief [Public] End
 * 
 * Log kaydını sonlandırmak için
 * ve kayıt işlemine ait tarihi yazıp bitirmek
 * için varolan kayıt sonlandırma
 * 
 * @return e_logger
 */
e_log Logger::end() noexcept
{
    if( !this->isOpen() && this->open() != file::e_file::succ_opened )
        return e_log::err_log_not_opened;
    
    this->finish();
    this->close();

    return !this->isOpen() ?
        e_log::succ_end : e_log::err_log_not_ended;
}
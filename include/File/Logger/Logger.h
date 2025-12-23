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
 * 
 * ====================================================================================================
 * Güncelleme: 19/12/2025
 * 
 * Kaç adet test yapıldı ve kaç tanesi başarı diye test çıktısı ve
 * kaç tanesi normal mesaj diye çıktı verebiliriz ve bu sayede ek bir bilgi
 * sağlanmış olur
 * 
 * ====================================================================================================
 */

// Include:
#include <File/File.h>
#include <Tool/Utf/Utf.h>
#include <Platform/Platform.h>
#include <Test/Test.h>
#include <Handler/Crash/CrashBase.h>

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <atomic>
#include <mutex>
#include <array>

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
            err = 1000,
            err_log_not_opened,
            err_not_set_key,
            err_name_empty,
            err_name_length_zero,
            err_set_name,
            err_path_empty,
            err_path_length_zero,
            err_set_path,
            err_log_not_ended,
            err_log_add_pass,
            err_log_add_fail,
            err_log_add_other,

            succ = 2000,
            succ_logged,
            succ_set_key,
            succ_set_name,
            succ_set_path,
            succ_end,

            warn = 3000,
            warn_log_already_exists
        };

        // Static Constexpr
        static inline constexpr size_t code_pass = 0;
        static inline constexpr size_t code_fail = 1;
        static inline constexpr size_t code_other = 2;

        // Function: Short Path
        [[maybe_unused]]
        static std::u32string short_path(const char* _path) noexcept
        {
            std::u32string filepath = utf::to_utf32(_path);

            const auto pos = filepath.find_last_of(U"/\\");
            if( pos == std::u32string::npos )
                return filepath;

            return filepath.substr(pos + 1);
        }

        // Struct: Source Info
        typedef struct srcinfo_t
        {
            std::u32string file;
            const int line;
            const char* func;
        } srcinfo_t;

        // Define Functions:
        #define LOG_MSG(logger, msg, status, console) \
            (logger).log( \
                msg, \
                srcinfo_t{ short_path(__FILE__), __LINE__, __func__ }, \
                status, \
                console \
            )

        #define LOG_EXPECT(logger, first, second, msg) \
            (logger).log( \
                first, \
                second, \
                msg, \
                srcinfo_t{ short_path(__FILE__), __LINE__, __func__ } \
            )
    }

    // Using Namespace:
    using namespace logger;
    using namespace handler;
    
    // Class: Logger
    class Logger : virtual public File, virtual public CrashBase
    {
        private:
            std::string key;
            std::u32string name;

            std::array<std::atomic<size_t>, 3> tests {{
                ATOMIC_VAR_INIT(0),
                ATOMIC_VAR_INIT(0),
                ATOMIC_VAR_INIT(0)
            }};

            e_log setKey() noexcept;
            e_log setName(const std::u32string&) noexcept;

            void start() noexcept;
            void finish() noexcept;

            bool inc(test::e_status) noexcept;

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

            template<typename _Ltype, typename _Rtype>
            e_log log(
                _Ltype, _Rtype,
                const std::u32string&,
                const srcinfo_t&
            ) noexcept;

            virtual e_log log(
                const std::u32string&,
                const srcinfo_t&,
                const test::e_status = test::e_status::information,
                const bool = true
            ) noexcept;

            virtual e_log end() noexcept;

            virtual void print_test_result() noexcept;

            void onCrash() noexcept override;
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
 * @brief [Private] Increase
 * 
 * Belirlenen türdeki duruma göre
 * yapılan test değerini arttıracak
 * 
 * @param e_status Status
 * @return bool
 */
bool Logger::inc(test::e_status _status) noexcept
{
    if( !this->isOpen() )
        return false;

    switch( _status ) {
        case test::e_status::success:
            this->tests.at(logger::code_pass)++;
            break;
        case test::e_status::error:
            this->tests.at(logger::code_fail)++;
            break;
        case test::e_status::information:
        default:
            this->tests.at(logger::code_other)++;
    }

    return true;
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
    if( !this->isOpen() )
        return e_log::err_log_not_opened;

    bool status = test::expect_eq(_first, _second, utf::to_utf8(_logtext));
    this->write(utf::to_utf32(platform::current_time()) + U" ==> " + (status ? U"[ PASS ] " : U"[ FAIL ] ") + _logtext + U"\n");

    return e_log::succ_logged;
}

/**
 * @brief [Public] Log
 * 
 * Log dosyayı oluşturulduktan sonra
 * verilen kayıt işlemlerini dosyaya kaydetmemizi
 * sağlayacak olan kayıt fonksiyonu ve kayıt
 * öncesinden bilgilendirme amaçlı test çıktısını
 * verecektir fakat ek olarak hangi dosya, satır
 * ve fonksiyon bilgisin de tutabilecek
 * 
 * @tparam _Ltype First
 * @tparam _Rtype Second
 * @param string Log Text
 * @param srcinfo_t Log Source
 * 
 * @return e_logger
 */
template<typename _Ltype, typename _Rtype>
e_log Logger::log(
    _Ltype _first,
    _Rtype _second,
    const std::u32string& _logtext,
    const srcinfo_t& _logsrc
) noexcept
{
    const std::u32string& msg =
        U"[ FILE: " + _logsrc.file +
        utf::to_utf32(
            " | LINE: " + std::to_string(_logsrc.line) +
            " | FUNC: " + std::string(_logsrc.func) + 
            " ] "
        ) + _logtext;

    return this->log(_first, _second, msg);
}

/**
 * @brief [Public] Log
 * 
 * Log dosyayı oluşturulduktan sonra
 * verilen kayıt işlemlerini dosyaya kaydetmemizi
 * sağlayacak olan kayıt fonksiyonu fakat ek olarak
 * hangi dosya, satır ve fonksiyon bilgisin de
 * tutabilecek
 * 
 * @param string logtext
 * @param srcinfo_t Log Source
 * @param e_status Log Type
 * @param bool Console Output
 * 
 * @return e_logger
 */
e_log Logger::log(
    const std::u32string& _logtext,
    const srcinfo_t& _logsrc,
    const test::e_status _logtype,
    const bool _console
) noexcept
{
    if( !this->inc(_logtype) )
        return e_log::err_log_not_opened;

    std::string logtypestr;

    switch( _logtype ) {
        case test::e_status::success: logtypestr = "PASS"; break;
        case test::e_status::error: logtypestr = "FAIL"; break;
        case test::e_status::information: logtypestr = "INFO"; break;
        default: logtypestr = "TEXT";
    }

    const std::u32string& msg =
        U"[ FILE: " + _logsrc.file +
        utf::to_utf32(
            " | LINE: " + std::to_string(_logsrc.line) +
            " | FUNC: " + std::string(_logsrc.func) + 
            " ] "
        ) + _logtext;

    this->write(utf::to_utf32(platform::current_time() + " ==> [ " + logtypestr  + " ] ") + msg + U"\n");

    if( _console )
        test::message(_logtype, utf::to_utf8(_logtext));

    return e_log::succ_logged;
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

/**
 * @brief [Public] Print Test Result
 * 
 * Test sonuçlarının çıktısını vererek kullanıcıyı
 * daha iyi bir şekilde bilgilendirmeyi amaçlamak
 */
void Logger::print_test_result() noexcept
{
    test::reset_color();
    std::cout << "\n==================== LOGGER ====================\n";
    std::cout << std::setw(20) << std::left << "Total Test: " << this->tests.at(logger::code_pass).load() + this->tests.at(logger::code_fail).load() << "\n";

    test::set_color(test::color_pass);
    std::cout << std::setw(20) << std::left << "Pass: " << this->tests.at(logger::code_pass).load() << "\n";

    test::set_color(test::color_fail);
    std::cout << std::setw(20) << std::left << "Fail: " << this->tests.at(logger::code_fail).load() << "\n";

    test::set_color(test::color_info);
    std::cout << std::setw(20) << std::left << "Other: " << this->tests.at(logger::code_other).load() << "\n";

    test::reset_color();
    std::cout << "\n================================================\n";
}

/**
 * @brief [Public] On Crash
 * 
 * Çökme durumunda log kaydına
 * çökme bilgisini yazmak için
 */
void Logger::onCrash() noexcept
{
    LOG_MSG((*this),
        utf::to_utf32("[LOGGER:CRASH HANDLER] [ CODE: " + std::to_string(getSignal()) + " ] Application crashed unexpectedly"),
        test::e_status::error,
        false
    );
    
    this->finish();
}
// Abdulkadir U. - 09/11/2025
#pragma once

/**
 * File (Dosya)
 * 
 * Dosya açıp veri yazmak, okumak veya eklemek
 * gibi işlemleri yapmamızı sağlayacak sınıf yapısı.
 * Gerektiğinde hata olup olmadığını kontrol ederek
 * de olası sorunların oluşmasını engelleyeceğiz.
 */

// Include:
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <codecvt>
#include <locale>
#include <mutex>

// Namespace: Core
namespace core
{
    // Namespace: File
    namespace file
    {
        // Limit:
        static inline constexpr size_t MAX_READ_LETTER = 1024;
        static inline constexpr size_t MAX_WRITE_LETTER = 1024;

        // Enum Clas: IO Code
        enum class e_io : int
        {
            read = static_cast<int>(std::ios::in),
            write = static_cast<int>(std::ios::out),
            add = static_cast<int>(std::ios::app),
            trunc = static_cast<int>(std::ios::trunc),
            binary = static_cast<int>(std::ios::binary),
            read_write = static_cast<int>(std::ios::in | std::ios::out),
            read_write_bin = static_cast<int>(std::ios::in | std::ios::out | std::ios::binary),
            add_bin = static_cast<int>(std::ios::app | std::ios::binary)
        };

        /**
         * @brief [Operator] |
         * 
         * e_io enum sınıfı için bit türünde |
         * işlemi yapabilmek
         * 
         * @param e_io Lhs
         * @param e_io Rhs
         * @return e_io
         */
        inline constexpr e_io operator|(e_io _lhs, e_io _rhs) noexcept {
            return static_cast<e_io>(
                static_cast<int>(_lhs) |
                static_cast<int>(_rhs)
            );
        }

        /**
         * @brief [Operator] &
         * 
         * e_io enum sınıfı için bit türünde &
         * işlemi yapabilmek
         * 
         * @param e_io Lhs
         * @param e_io Rhs
         * @return e_io
         */
        inline constexpr e_io operator&(e_io _lhs, e_io _rhs) noexcept {
            return static_cast<e_io>(
                static_cast<int>(_lhs) &
                static_cast<int>(_rhs)
            );
        }

        /**
         * @brief [Operator] |
         * 
         * e_io ile std::ios::openmode operator |
         * kullanımını sağlamak
         * 
         * @param e_io Lhs
         * @param std::ios::openmode Rhs
         * @return std::ios::openmode
         */
        inline constexpr std::ios::openmode operator|(e_io _lhs, std::ios::openmode _rhs) noexcept {
            return static_cast<std::ios::openmode>(static_cast<int>(_lhs)) | _rhs;
        }

        /**
         * @brief [Operator] &
         * 
         * e_io ile std::ios::openmode operator &
         * kullanımını sağlamak
         * 
         * @param e_io Lhs
         * @param std::ios::openmode Rhs
         * @return std::ios::openmode
         */
        inline constexpr std::ios::openmode operator&(e_io _lhs, std::ios::openmode _rhs) noexcept {
            return static_cast<std::ios::openmode>(static_cast<int>(_lhs)) & _rhs;
        }

        // Enum Class: File Code
        enum class e_file : size_t
        {
            unknown = 0x0,
            error   = 0x1,
            warning = 0x2,
            success = 0x3,

            err_not_opened = 1000,
            err_not_closed,
            err_no_status,
            err_has_error,
            err_no_file,
            err_no_path,
            err_not_read,
            err_not_read_mode,
            err_not_write,
            err_not_write_mode,
            err_not_add,
            err_not_clear,
            err_not_remove,
            err_not_newfile,
            err_set_filepath,
            err_invalid_position,
            err_invalid_position_over,
            err_invalid_position_under,
            err_set_position,
            err_set_openmode,
            err_already_open,
            err_close_file_for_setpath,
            err_close_file_for_setmode,
            err_read,
            err_write,
            err_reset,
            err_undo,
            err_print,

            succ_opened = 2000,
            succ_closed,
            succ_no_error,
            succ_status_good,
            succ_has_file,
            succ_has_path,
            succ_on_read,
            succ_on_write,
            succ_on_add,
            succ_clear,
            succ_remove,
            succ_is_newfile,
            succ_set_filepath,
            succ_valid_position,
            succ_set_position,
            succ_set_openmode,
            succ_read,
            succ_write,
            succ_reset,
            succ_print,

            warn_still_open = 3000,
            warn_already_close,
        };
    }

    // Class: File
    class File
    {
        private:
            static std::string to_utf8(const std::u32string& _text) noexcept;
            static std::u32string to_utf32(const std::string& _text) noexcept;

        private:
            std::u32string path;
            std::fstream file;
            file::e_io mode;

            mutable std::mutex mtx;

            e_file setPath(const std::u32string&) noexcept;
            e_file setMode(const e_io) noexcept;

        public:
            explicit File(const std::u32string&, const e_io = e_io::read_write_bin) noexcept;
            ~File() noexcept;

            virtual inline bool hasError() const noexcept;
            virtual inline bool hasFile() const noexcept;
            virtual inline bool hasPath() const noexcept;

            virtual inline bool isOpen() const noexcept;
            virtual inline bool isClose() const noexcept;
            virtual inline bool isRead() const noexcept;
            virtual inline bool isWrite() const noexcept;
            virtual inline bool isAdd() const noexcept;

            virtual inline const std::u32string& getPath() const noexcept;
            virtual inline const std::fstream& getFile() const noexcept;
            virtual inline const e_io& getMode() const noexcept;

            virtual e_file open() noexcept;
            virtual e_file close() noexcept;
            virtual e_file clear() noexcept;

            virtual e_file write(const std::u32string&) noexcept;
            virtual e_file read(std::u32string&) noexcept;

            virtual e_file position(const std::streampos = std::ios::end) noexcept;
            virtual e_file reset() noexcept;
            virtual e_file undo() noexcept;

            virtual e_file print() noexcept;
    };
}

// Core::File
using namespace core;
using namespace file;

/**
 * @brief [Static Private] To UTF-8
 * 
 * UTF-32 metini UTF-8 metine çevirme işlemi
 * 
 * @param u32string& Text
 * @return string
 */
std::string File::to_utf8(const std::u32string& _text) noexcept
{
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> tmp__conv;
    return tmp__conv.to_bytes(_text);
}

/**
 * @brief [Static Private] To UTF-32
 * 
 * UTF-8 metini UTF-32 metine çevirme işlemi
 * 
 * @param string& Text
 * @return u32string
 */
std::u32string File::to_utf32(const std::string& _text) noexcept
{
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> tmp__conv;
    return tmp__conv.from_bytes(_text);
}

/**
 * @brief [Public] Constructor
 * 
 * Sınıfın oluşturucu. Gerekli dosyanın yolunu
 * bulmayı dener ve eğer dosya adı yoksa yenisini
 * oluşturur
 * 
 * @param u32string& Filepath
 * @param ios::openmode Mode
 */
File::File
(
    const std::u32string& _filepath,
    const e_io _mode
)
{
    this->setPath(_filepath);
    this->setMode(_mode);
}

/**
 * @brief [Public] Destructor
 * 
 * Sınıfı yıkıcısı. Dosyayı kapatır, dosya yolunu
 * temizler ve sanki hiç varolmamış gibi hale getirir.
 */
File::~File() noexcept
{
    this->close();
}

/**
 * @brief [Public] Has Error
 * 
 * Dosya işleyicisinde herhangi bir hata varsa eğer
 * gelecekteki işlemleri de etkileyeceği için
 * hatayı şimdiden kontrol ediyoruz
 * 
 * @return bool
 */
bool File::hasError() const noexcept
{
    return this->file.fail() || this->file.bad();
}

/**
 * @brief [Public] Has File
 * 
 * Dosya işleyici eğer gerçekten varolan bir dosyaın
 * olup olmadığını ki buna yeni oluşturulmuş dosya da
 * dahil olacak şekilde kontrol eder
 * 
 * @return bool
 */
bool File::hasFile() const noexcept
{
    return !this->path.empty() && std::filesystem::exists(to_utf8(this->path));
}

/**
 * @brief [Public] Has Path
 * 
 * Dosya yolunun var olup olmadığını kontrol eder sadece
 * 
 * @return bool
 */
bool File::hasPath() const noexcept
{
    return !this->path.empty();
}

/**
 * @brief [Public] Is Open
 * 
 * Bir dosya var mı ve bu dosya açık mı diye
 * kontrol sağlar ve olası güvenlik sorunları engellenir
 * 
 * @return bool
 */
bool File::isOpen() const noexcept
{
    return this->hasFile() && this->file.is_open();
}

/**
 * @brief [Public] Is Close
 * 
 * Bir dosya var mı ve bu dosya kapalı mı diye
 * kontrol sağlar ve olası güvenlik sorunları engellenir
 * 
 * @return bool
 */
bool File::isClose() const noexcept
{
    return this->hasFile() && !this->isOpen();
}

/**
 * @brief [Public] Is Read
 * 
 * Dosya varsa ve açma modu okuma ise
 * başarıyla cevap döner, aksi halde hatalı döner
 * 
 * @return bool
 */
bool File::isRead() const noexcept
{
    return this->hasFile() && (static_cast<std::ios::openmode>(static_cast<int>(this->mode)) & std::ios::in);
}

/**
 * @brief [Public] Is Write
 * 
 * Dosya varsa ve açma modu yazma ise
 * başarıyla cevap döner, aksi halde hatalı döner
 * 
 * @return bool
 */
bool File::isWrite() const noexcept
{
    return this->hasFile() && (static_cast<std::ios::openmode>(static_cast<int>(this->mode)) & std::ios::out);
}

/**
 * @brief [Public] Is Add
 * 
 * Dosya varsa ve açma modu sonuna ekleme ise
 * başarıyla cevap döner, aksi halde hatalı döner
 * 
 * @return bool
 */
bool File::isAdd() const noexcept
{
    return this->hasFile() && (static_cast<std::ios::openmode>(static_cast<int>(this->mode)) & std::ios::app);
}

/**
 * @brief [Public] Get Path
 * 
 * Dosya yoluna ait bilgiyi getirir
 * 
 * @return u32string&
 */
const std::u32string& File::getPath() const noexcept
{
    return this->path;
}

/**
 * @brief [Public] Get File
 * 
 * Dosyanın kendisine ait olan veri yapısını getirir
 * 
 * @return fstream&
 */
const std::fstream& File::getFile() const noexcept
{
    return this->file;
}

/**
 * @brief [Public] Get Open Mode
 * 
 * Dosyanın açık olmasını ve var olduğu
 * açma modunu getirir
 * 
 * @return openmode
 */
const e_io& File::getMode() const noexcept
{
    return this->mode;
}

/**
 * @brief [Private] Set Path
 * 
 * Dosya yolunu güvenli ve kontrollü şekilde değiştirmeyi
 * sağlar ve bu süre zarfında dosyanın kendisi kapalı olmaldır
 * 
 * @param u32string& File Path
 * @return e_file
 */
e_file File::setPath(const std::u32string& _filepath) noexcept
{
    std::lock_guard<std::mutex> tmp__lock(this->mtx);

    if( _filepath.empty() || _filepath.length() < 1 )
        return e_file::err_no_path;

    if( this->isOpen() )
        return e_file::err_close_file_for_setpath;

    this->path.assign(_filepath);
    return this->path == _filepath ? e_file::succ_set_filepath : e_file::err_set_filepath;
}

/**
 * @brief [Private] Set Mode
 * 
 * Dosya yolunu güvenli ve kontrollü şekilde kontrol edip
 * eğer dosya kapalıysa modunu değiştirebilir ama aksi halde
 * hatalı ve belirlenemeyen sonuçlar üretebilir
 * 
 * @param openmode Openmode
 * @return e_file
 */
e_file File::setMode(const e_io _openmode) noexcept
{
    std::lock_guard<std::mutex> tmp__lock(this->mtx);

    if( this->isOpen() )
        return e_file::err_close_file_for_setmode;

    this->mode = _openmode;
    return (this->mode & _openmode) == _openmode ?
        e_file::succ_set_openmode : e_file::err_set_openmode;
}

/**
 * @brief [Public] Open
 * 
 * Dosyayı istediğimiz mod da açmamızı sağlayacak yapıdır
 * 
 * @return e_file
 */
e_file File::open() noexcept
{
    std::lock_guard<std::mutex> tmp__lock(this->mtx);

    if( this->path.empty() )
        return e_file::err_no_path;

    if( this->isOpen() )
        return e_file::err_already_open;

    this->file.open(to_utf8(this->getPath()), static_cast<std::ios::openmode>(this->mode));
    return this->isOpen() ? e_file::succ_opened : e_file::err_not_opened;
}

/**
 * @brief [Public] Close
 * 
 * Dosyayı kapatıp kapatmadığını kontrol ediyoruz
 * 
 * @return e_file
 */
e_file File::close() noexcept
{
    std::lock_guard<std::mutex> tmp__lock(this->mtx);

    if( !this->isOpen() )
        return e_file::warn_already_close;
        
    this->file.close();
    return this->isOpen() ? e_file::err_not_closed : e_file::succ_closed;
}

/**
 * @brief [Public] Clear
 * 
 * Dosyanın içeriğini temizleyip boş bırakmak için
 * 
 * @return e_file
 */
e_file File::clear() noexcept
{
    std::scoped_lock tmp__lock(this->mtx);

    if( !this->isOpen() )
        return e_file::err_not_opened;

    e_file tmp__status = this->close();
    
    if( tmp__status != e_file::succ_closed )
        return e_file::err_not_closed;

    std::ofstream tmp__trunc(to_utf8(this->getPath()), std::ios::out | std::ios::trunc | std::ios::binary);

    if( !tmp__trunc )
        return e_file::err_not_write_mode;

    tmp__trunc.close();
    tmp__status = this->open();

    if( tmp__status != e_file::succ_opened )
        return e_file::err_not_opened;

    return this->isOpen() ? e_file::succ_clear : e_file::err_not_clear;
}

/**
 * @brief [Public] Write
 * 
 * Dosyaya veri yazmak için vardır ve veri tür
 * dönüşümü yaparak yapar bunları
 * 
 * @param u32string Input
 * @return e_file
 */
e_file File::write(const std::u32string& _input) noexcept
{
    std::scoped_lock tmp__lock(this->mtx);

    if( !this->isOpen() )
        return e_file::err_not_opened;

    for( auto c32 : _input )
        this->file.write(reinterpret_cast<const char*>(&c32), sizeof(char32_t));

    if( !this->file.good() )
        return e_file::err_not_write;

    this->file.flush();
    return e_file::succ_write;
}

/**
 * @brief [Public] Read
 * 
 * Dosyaya bakıp okumak için vardır ve veri tür
 * dönüşümü yaparak yapar bunları
 * 
 * @param u32string Input
 * @return e_file
 */
e_file File::read(std::u32string& _output) noexcept
{
    std::scoped_lock tmp__lock(this->mtx);

    if( !this->isOpen() )
        return e_file::err_not_opened;

    if( this->file.eof() ) {
        this->file.clear();
        this->file.seekg(0, std::ios::beg);
    }

    std::u32string tmp__buffer;
    char32_t tmp__c32;

    for( size_t counter = 0;
        counter < file::MAX_READ_LETTER &&
        this->file.read(reinterpret_cast<char*>(&tmp__c32), sizeof(char32_t));
        ++counter )
            tmp__buffer.push_back(tmp__c32);

    _output = std::move(tmp__buffer);
    return e_file::succ_read;
}

/**
 * @brief [Public] Position
 * 
 * Dosyadaki belirli ama dosya boyutunu pozitif ya da
 * negatif yönde aşmayacak şekilde verebilirim
 * 
 * @param streampos Position
 * @return e_file
 */
e_file File::position(const std::streampos _position) noexcept
{
    std::lock_guard<std::mutex> tmp__lock(this->mtx);

    if( !this->isOpen() )
        return e_file::err_no_file;

    this->file.seekg(0, std::ios::end);
    auto tmp__size = this->file.tellg();
    
    if( tmp__size < 0 || _position < 0 ) return e_file::err_invalid_position_under;
    if( _position > tmp__size ) return e_file::err_invalid_position_over;

    this->file.seekg(_position, std::ios::beg);
    this->file.seekp(_position, std::ios::beg);

    return this->file.good() ? e_file::succ_set_position : e_file::err_set_position;
}

/**
 * @brief [Public] Reset
 * 
 * Şuan ki işlemleri dondurup kendini sıfırlamaya çalışır
 * ve yeniden başlatır çalışmaya
 * 
 * @return e_file
 */
e_file File::reset() noexcept
{
    std::lock_guard<std::mutex> tmp__lock(this->mtx);

    e_file tmp__status = this->isOpen() ? this->close() : e_file::warn_already_close;

    switch( tmp__status )
    {
        case e_file::succ_closed:
        case e_file::warn_already_close:
            break;
        default:
            return e_file::err_not_closed;
    }

    tmp__status = this->open();
    return tmp__status == e_file::succ_opened ? e_file::succ_reset : e_file::err_reset;
}

/**
 * @brief [Public] Undo
 * 
 * Dosyadaki işlemi bir önceki işleme doğru
 * geri sarma işlemi fakat şuan için ihtiyacımız yok
 * gelecekte yeni sınıflar türetilmek istenirse
 * fonksiyonel hale getirilebilir
 * 
 * @return e_file
 */
e_file File::undo() noexcept
{
    return e_file::err_undo;
}

/**
 * @brief [Public] Print
 * 
 * Dosya hakkında basit ve bilgilendirici çıktılar
 * vererek kullanıcıyı da bilgilendirebiliriz
 * 
 * @return e_file
 */
e_file File::print() noexcept
{
    std::cout << "\n===== FILE =====\n";
    std::cout << "Path: " << (this->getPath().empty() ? "(none)" : to_utf8(this->getPath())) << "\n";
    std::cout << "Is Open: " << (this->isOpen() ? "yes" : "no") << "\n";
    std::cout << "Has Error: " << (this->hasError() ? "yes" : "no") << "\n";
    std::cout << "Read: " << (this->isRead() ? "yes" : "no") << "\n";
    std::cout << "Write: " << (this->isWrite() ? "yes" : "no") << "\n";
    std::cout << "Append: " << (this->isAdd() ? "yes" : "no") << "\n";

    return e_file::succ_print;
}
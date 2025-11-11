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
#include <fstream>
#include <string>
#include <filesystem>
#include <codecvt>
#include <locale>
#include <cstdint>

// Namespace: Core
namespace core
{
    // Namespace: File
    namespace file
    {
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
            err_not_write,
            err_not_add,
            err_not_clear,
            err_not_newfile,
            err_set_filepath,
            err_invalid_position,
            err_set_position,
            err_set_openmode,
            err_already_open,
            err_close_file_for_setpath,
            err_close_file_for_setmode,
            err_read,
            err_write,
            err_reset,
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
            std::string path;
            std::fstream file;
            std::ios::openmode mode = std::ios::in | std::ios::out;

            e_file setPath(const std::string&) noexcept;
            e_file setMode(const std::ios::openmode) noexcept;

        public:
            explicit File(const std::string&, const std::ios::openmode) noexcept;
            ~File() noexcept;

            virtual inline bool hasError() const noexcept;
            virtual inline bool hasFile() const noexcept;
            virtual inline bool hasPath() const noexcept;

            virtual inline bool isOpen() const noexcept;
            virtual inline bool isClose() const noexcept;
            virtual inline bool isRead() const noexcept;
            virtual inline bool isWrite() const noexcept;
            virtual inline bool isAdd() const noexcept;

            virtual inline const std::string& getPath() const noexcept;
            virtual inline const std::fstream& getFile() const noexcept;
            virtual inline const std::ios::openmode& getMode() const noexcept;

            virtual e_file open() noexcept;
            virtual e_file close() noexcept;
            virtual e_file clear() noexcept;

            virtual e_file write(const std::u32string&) noexcept;
            virtual e_file read(std::u32string&) noexcept;

            virtual e_file position(const std::streampos = std::ios::end) noexcept;
            virtual e_file reset() noexcept;
            virtual e_file undo() noexcept = 0;

            virtual e_file print() noexcept;
    };
}

// Core::File
using namespace core;
using namespace file;

/**
 * @brief [Public] Constructor
 * 
 * Sınıfın oluşturucu. Gerekli dosyanın yolunu
 * bulmayı dener ve eğer dosya adı yoksa yenisini
 * oluşturur
 * 
 * @param string& Filepath
 * @param ios::openmode Mode
 */
File::File
(
    const std::string& _filepath,
    const std::ios::openmode _mode = std::ios::in | std::ios::out
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
    this->clear();
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
    return !this->path.empty() && std::filesystem::exists(this->path);
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
    return this->hasFile() && this->mode & std::ios::in;
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
    return this->hasFile() && this->mode & std::ios::out;
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
    return this->hasFile() && this->mode & std::ios::app;
}

/**
 * @brief [Public] Get Path
 * 
 * Dosya yoluna ait bilgiyi getirir
 * 
 * @return string&
 */
const std::string& File::getPath() const noexcept
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
const std::ios::openmode& File::getMode() const noexcept
{
    return this->mode;
}

/**
 * @brief [Private] Set Path
 * 
 * Dosya yolunu güvenli ve kontrollü şekilde değiştirmeyi
 * sağlar ve bu süre zarfında dosyanın kendisi kapalı olmaldır
 * 
 * @return e_file
 */
e_file File::setPath(const std::string& _filepath) noexcept
{
    if( _filepath.empty() || _filepath.length() < 1 )
        return e_file::err_no_path;

    if( !this->isOpen() )
        return e_file::err_close_file_for_setpath;

    this->path.assign(_filepath);
    return this->path.compare(_filepath) == 0 ? e_file::succ_set_filepath : e_file::err_set_filepath;
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
e_file File::setMode(const std::ios::openmode _openmode) noexcept
{
    if( this->isOpen() )
        return e_file::err_close_file_for_setmode;

    this->mode = _openmode;
    return this->mode == _openmode ? e_file::succ_set_openmode : e_file::err_set_openmode;
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
    if( this->path.empty() )
        return e_file::err_no_path;

    if( this->isOpen() )
        return e_file::err_already_open;

    this->file.open(this->path, this->mode);
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
    if( !this->isOpen() )
        return e_file::warn_already_close;

    return this->isOpen() ? e_file::err_not_closed : e_file::succ_closed;
}

/**
 * @brief [Public] Open
 * 
 * Dosyayı temizlemesi yani; dosya adı,
 * dosya ve dosya yolu temizlenmiş oluyor
 * 
 * @return e_file
 */
e_file File::clear() noexcept
{
    if( this->isOpen() )
    {
        this->file.close();

        if( this->isOpen() )
            return e_file::err_not_closed;
    }

    this->file.clear();
    this->path.clear();

    return e_file::succ_clear;
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
    if( !this->isOpen() )
        return e_file::err_no_file;

    try {
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> tmp__conv;
        std::string tmp__utf8 = tmp__conv.to_bytes(_input);
        this->file << tmp__utf8;
    } catch(...) {
        return e_file::err_write;
    }

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
    if( !this->isOpen() )
        return e_file::err_no_file;

    try {
        std::string tmp__buf((std::istreambuf_iterator<char>(this->file)), std::istreambuf_iterator<char>());
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> tmp__conv;
        _output = tmp__conv.from_bytes(tmp__buf);
    } catch(...) {
        return e_file::err_read;
    }

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
    if( !this->isOpen() )
        return e_file::err_no_file;

    this->file.seekg(0, std::ios::end);
    auto tmp__end = this->file.tellg();

    if( _position < 0 || _position > tmp__end )
        return e_file::err_invalid_position;

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
    if( !this->isOpen() )
        return e_file::err_no_file;

    if( this->close() != e_file::succ_closed )
        return e_file::err_not_closed;

    if( this->open() != e_file::succ_opened )
        return e_file::err_not_opened;

    return this->isOpen() ? e_file::succ_reset : e_file::err_reset;
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
    std::cout << "Path: " << (this->path.empty() ? "(none)" : this->path) << "\n";
    std::cout << "Is Open: " << (this->isOpen() ? "yes" : "no") << "\n";
    std::cout << "Has Error: " << (this->hasError() ? "yes" : "no") << "\n";
    std::cout << "Read: " << (this->isRead() ? "yes" : "no") << "\n";
    std::cout << "Write: " << (this->isWrite() ? "yes" : "no") << "\n";
    std::cout << "Add: " << (this->isAdd() ? "yes" : "no") << "\n";

    return e_file::succ_print;
}
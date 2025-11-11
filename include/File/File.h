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
#include <vector>
#include <filesystem>

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
            err_set_location,
            err_set_openmode,
            err_already_open,
            err_close_file_for_setpath,
            err_close_file_for_setmode,
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
            succ_set_location,
            succ_set_openmode,
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
            virtual inline bool hasFile()  const noexcept;
            virtual inline bool hasPath()  const noexcept;

            virtual inline bool isOpen()     const noexcept;
            virtual inline bool isClose()     const noexcept;
            virtual inline bool isRead()       const noexcept;
            virtual inline bool isWrite()      const noexcept;
            virtual inline bool isAdd()        const noexcept;

            virtual inline const std::string& getPath() const noexcept;
            virtual inline const std::fstream& getFile() const noexcept;

            virtual e_file open()     noexcept;
            virtual e_file close()    noexcept;
            virtual e_file clear()    noexcept;

            virtual e_file write(const std::u32string&) noexcept;
            virtual e_file read(std::u32string&) noexcept;

            virtual e_file location(const size_t = 0) noexcept;
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

bool File::hasError() const noexcept
{
    return this->file.fail() || this->file.bad();
}

bool File::hasFile() const noexcept
{
    return !this->path.empty() && std::filesystem::exists(this->path);
}

bool File::hasPath() const noexcept
{
    return !this->path.empty();
}

bool File::isOpen() const noexcept
{
    return this->hasFile() && this->file.is_open();
}

bool File::isClose() const noexcept
{
    return this->hasFile() && !this->file.is_open();
}

bool File::isRead() const noexcept
{
    return this->hasFile() && this->mode & std::ios::in;
}

bool File::isWrite() const noexcept
{
    return this->hasFile() && this->mode & std::ios::out;
}

bool File::isAdd() const noexcept
{
    return this->hasFile() && this->mode & std::ios::app;
}

const std::string& File::getPath() const noexcept
{
    return this->path;
}

const std::fstream& File::getFile() const noexcept
{
    return this->file;
}

e_file File::setPath(const std::string& _filepath) noexcept
{
    if( _filepath.empty() || _filepath.length() < 1 )
        return e_file::err_no_path;

    if( !this->file.is_open() )
        return e_file::err_close_file_for_setpath;

    this->path.assign(_filepath);
    return this->path.compare(_filepath) == 0 ? e_file::succ_set_filepath : e_file::err_set_filepath;
}

e_file File::setMode(const std::ios::openmode _openmode) noexcept
{
    if( this->file.is_open() )
        return e_file::err_close_file_for_setmode;

    this->mode = _openmode;
    return this->mode == _openmode ? e_file::succ_set_openmode : e_file::err_set_openmode;
}

e_file File::open() noexcept
{
    if( this->path.empty() )
        return e_file::err_no_path;

    if( this->file.is_open() )
        return e_file::err_already_open;

    this->file.open(this->path, this->mode);
    return this->file.is_open() ? e_file::succ_opened : e_file::err_not_opened;
}

e_file File::close() noexcept
{
    if( !this->file.is_open() )
        return e_file::warn_already_close;

    return this->file.is_open() ? e_file::err_not_closed : e_file::succ_closed;
}

e_file File::clear() noexcept
{
    if( this->file.is_open() )
        this->file.close();

    if( this->file.is_open() )
        return e_file::err_not_closed;

    this->file.clear();
    this->path.assign("");

    return e_file::succ_clear;
}

e_file File::write(const std::u32string& _input) noexcept
{
    return e_file::err_not_write;
}

e_file File::read(std::u32string& _output) noexcept
{
    return e_file::err_not_read;
}

e_file File::location(const size_t _location) noexcept
{
    return e_file::err_set_location;
}

e_file File::reset() noexcept
{
    return e_file::err_reset;
}

e_file File::print() noexcept
{
    return e_file::succ_print;
}
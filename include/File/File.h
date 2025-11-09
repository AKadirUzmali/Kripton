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
#include <iomanip>
#include <fstream>
#include <mutex>
#include <chrono>
#include <string>
#include <sstream>
#include <vector>
#include <filesystem>

#include <Flag/Flag.h>

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
            err_set_filename,
            err_set_location,
            err_reset,

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
            succ_set_filename,
            succ_set_location,
            succ_reset,

            warn_still_open = 3000
        };
    }

    // Class: File
    class File
    {
        private:
            std::u32string filename;
            std::ofstream path;
            std::mutex mtx;

            Flag flag;

            e_file setFilename(const std::u32string&) noexcept;

        public:
            explicit File(const std::u32string&) noexcept;
            ~File() noexcept;

            virtual bool hasError() const noexcept;
            virtual bool hasFile()  const noexcept;
            virtual bool hasPath()  const noexcept;

            virtual bool isOpen()     const noexcept;
            virtual bool isClose()     const noexcept;
            virtual bool isRead()       const noexcept;
            virtual bool isWrite()      const noexcept;
            virtual bool isAdd()        const noexcept;
            virtual bool isNewFile()    const noexcept;

            virtual const std::u32string& getFilename() const noexcept;

            virtual e_file open()     noexcept;
            virtual e_file close()    noexcept;
            virtual e_file status()   noexcept;
            virtual e_file clear()    noexcept;

            virtual e_file write(const std::u32string&) noexcept;
            virtual e_file read(std::u32string&) noexcept;

            virtual e_file location(const size_t = 0) noexcept;
            virtual e_file reset() noexcept;
            virtual e_file undo() noexcept = 0;
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
 * @param u32string& Filename
 */
File::File(const std::u32string& _filename)
{
    this->setFilename(_filename);
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
    return true;
}

bool File::hasFile() const noexcept
{
    return false;
}

bool File::hasPath() const noexcept
{
    return false;
}

bool File::isOpen() const noexcept
{
    return false;
}

bool File::isClose() const noexcept
{
    return false;
}

bool File::isRead() const noexcept
{
    return false;
}

bool File::isWrite() const noexcept
{
    return false;
}

bool File::isAdd() const noexcept
{
    return false;
}

bool File::isNewFile() const noexcept
{
    return false;
}

const std::u32string& File::getFilename() const noexcept
{
    return this->filename;
}

e_file File::setFilename(const std::u32string& _filename) noexcept
{
    return e_file::err_set_filename;
}

e_file File::open() noexcept
{
    return e_file::err_not_opened;
}

e_file File::close() noexcept
{
    return e_file::err_not_closed;
}

e_file File::status() noexcept
{
    return e_file::err_no_status;
}

e_file File::clear() noexcept
{
    return e_file::err_not_clear;
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
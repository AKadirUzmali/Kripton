// Abdulkadir U. - 15/01/2026
#pragma once

/**
 * File Output
 * 
 * Oluşan durum hakkında bir başlık ve metin ile
 * insanın okuyabileceği basit bir çıktı vermeyi
 * yöntemini kullanıyoruz ve belirtilen dosyadan
 * bunu okuyarak ne olduğunu daha rahat anlayabileceğiz
 */

// Include:
#include <iostream>
#include <iomanip>
#include <string>
#include <mutex>
#include <fstream>

#include <developer/log/Output.h>
#include <kits/Toolkit.h>

// Namespace:
namespace dev::output::file
{
    // Using Namespace:
    using namespace dev::level;

    // Class:
    class FileOut : public virtual Output
    {
        private:
            inline static std::mutex mtx {};
            std::ofstream file;

        private:
            void init_info() noexcept;

        public:
            explicit FileOut(std::string_view outname);

            virtual void write(
                Level lvl,
                std::string_view msg
            ) noexcept override;

            virtual void write(
                std::string_view title,
                std::string_view msg
            ) noexcept override;

            virtual void write(
                std::string_view msg
            ) noexcept override;
    };

    /**
     * @brief FileOut
     * 
     * Belirtilen isim ile çıktı vericiyi
     * oluşturmasını sağlıyoruz
     * 
     * @param string_view Name
     */
    inline FileOut::FileOut
    (
        std::string_view outname
    ) :Output(outname), file(std::string(outname) + ".log", std::ios::app)
    {
        this->init_info();
    }

    /**
     * @brief Init Info
     * 
     * Dosya işlemi başladığı gibi bilgilendirmeyi dosyaya
     * kayıt edecek işlem
     */
    void FileOut::init_info() noexcept
    {
        constexpr size_t title_size = 20;

        std::scoped_lock lock(mtx);

        this->file << "================================================================================================================\n";
        this->file << std::setw(title_size) << std::left << "Path" << ": " << this->getName().data() << '\n';
        this->file << std::setw(title_size) << std::left << "Platform" << ": " << tools::os::current_pfm_name() << '\n';
        this->file << std::setw(title_size) << std::left << "Operating System" << ": " << tools::os::current_os_name() << '\n';
        this->file << std::setw(title_size) << std::left << "Timestamp" << ": " << tools::time::current_timestamp() << '\n';
        this->file << "================================================================================================================\n";

        this->file.flush();
    }

    /**
     * @brief Write
     * 
     * Duruma göre dosyaya çıktı verecek ve bu sayede
     * ne olduğunun anlaşılması daha rahat olacak
     * 
     * @note Durumda çıktı verilecek çünkü ne olduğunu anlamalıyız
     * @note Çıktı dosyaya kaydedilir
     * 
     * @param Level Status
     * @param string_view Message
     */
    void FileOut::write(
        Level lvl,
        std::string_view msg
    ) noexcept
    {
        std::scoped_lock lock(mtx);
        if( !this->file.is_open() )
            return;

        this->file << Output::getTitle(level::to_string(lvl).data()) << ' ' << msg << '\n';
        this->file.flush();
    }

    /**
     * @brief Write
     * 
     * Başlık ile dosyaya çıktı verecek ve bu sayede
     * ne olduğunun anlaşılması daha rahat olacak
     * 
     * @note Dosya olduğu için renk ayarlaması yok
     * 
     * @param string_view Title
     * @param string_view Message
     */
    void FileOut::write(
        std::string_view title,
        std::string_view msg
    ) noexcept
    {
        std::scoped_lock lock(mtx);
        if( !this->file.is_open() )
            return;

        this->file << Output::getTitle(title) << ' ' << msg << '\n';
        this->file.flush();
    }

    /**
     * @brief Write
     * 
     * Sadece mesaj ile dosyaya çıktı verecek ve bu sayede
     * ne olduğunun anlaşılması daha rahat olacak
     * 
     * @note Dosyaya veri yazılacağı için renk işlemi yok
     * @note Başlık da belirtilmediği için başlık kısmı hiç yok
     * 
     * @param string_view Message
     */
    void FileOut::write(
        std::string_view msg
    ) noexcept
    {
        std::scoped_lock lock(mtx);
        if( !this->file.is_open() )
            return;

        this->file << msg << '\n';
        this->file.flush();
    }
}
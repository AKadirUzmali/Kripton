// Abdulkadir U. - 15/01/2026
// Abdulkadir U. - 21/01/2026
#pragma once

/**
 * File Output (Dosya Çıktı)
 * 
 * Oluşan durum hakkında bir başlık ve metin ile
 * insanın okuyabileceği basit bir çıktı vermeyi
 * yöntemini kullanıyoruz ve belirtilen dosyadan
 * bunu okuyarak ne olduğunu daha rahat anlayabileceğiz
 */

// Include
#include <iostream>
#include <iomanip>
#include <string>
#include <mutex>
#include <fstream>

#include <dev/log/output.hpp>
#include <core/platform.hpp>

// Namespace
namespace dev::output::file
{
    // Using Namespace
    using namespace dev::level;

    // Class
    class FileOut : public virtual Output
    {
        private:
            static inline std::mutex m_mtx {};

        private:
            std::ofstream m_file;

        private:
            void init_info() noexcept;

        public:
            explicit FileOut(std::string_view ar_outname);

            virtual void write(
                level_t ar_lvl,
                std::string_view ar_msg
            ) noexcept override;

            virtual void write(
                std::string_view ar_title,
                std::string_view ar_msg
            ) noexcept override;

            virtual void write(
                std::string_view ar_msg
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
        std::string_view ar_outname
    ) :Output(ar_outname), m_file(std::string(ar_outname) + ".log", std::ios::app)
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
        constexpr size_t tm_title_size = 20;

        std::scoped_lock lock(m_mtx);

        this->m_file << "================================================================================================================\n";
        this->m_file << std::setw(tm_title_size) << std::left << "Path" << ": " << this->getName() << '\n';
        this->m_file << std::setw(tm_title_size) << std::left << "Platform" << ": " << core::platform::current_pfm_name() << '\n';
        this->m_file << std::setw(tm_title_size) << std::left << "Operating System" << ": " << core::platform::current_os_name() << '\n';
        this->m_file << std::setw(tm_title_size) << std::left << "Timestamp" << ": " << tools::time::current_timestamp() << '\n';
        this->m_file << "================================================================================================================\n";

        this->m_file.flush();
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
     * @param level_t Status
     * @param string_view Message
     */
    void FileOut::write(
        level_t ar_lvl,
        std::string_view ar_msg
    ) noexcept
    {
        std::scoped_lock lock(m_mtx);
        if( !this->m_file.is_open() )
            return;

        this->m_file << Output::getTitle(to_string(ar_lvl)) << ' ' << ar_msg << '\n';
        this->m_file.flush();
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
        std::string_view ar_title,
        std::string_view ar_msg
    ) noexcept
    {
        std::scoped_lock lock(m_mtx);
        if( !this->m_file.is_open() )
            return;

        this->m_file << Output::getTitle(ar_title) << ' ' << ar_msg << '\n';
        this->m_file.flush();
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
        std::string_view ar_msg
    ) noexcept
    {
        std::scoped_lock lock(m_mtx);
        if( !this->m_file.is_open() )
            return;

        this->m_file << ar_msg << '\n';
        this->m_file.flush();
    }
}
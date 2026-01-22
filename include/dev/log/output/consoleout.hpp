// Abdulkadir U. - 15/01/2026
// Abdulkadir U. - 21/01/2026
#pragma once

/**
 * Console Output (Konsol Çıktı)
 * 
 * Oluşan durum hakkında bir başlık ve metin ile
 * insanın okuyabileceği basit bir çıktı vermeyi
 * yöntemini kullanıyoruz ve konsol ekranından
 * bunu okuyarak ne olduğunu daha rahat anlayabileceğiz
 */

// Include:
#include <iostream>
#include <iomanip>
#include <string>
#include <mutex>

#include <dev/log/output.hpp>
#include <kits/toolkit.hpp>

// Namespace
namespace dev::output::console
{
    // Using Namespace
    using namespace dev::level;
    using namespace tools::console;

    // Class
    class ConsoleOut : public virtual Output
    {
        private:
            inline static std::mutex m_mtx {};

        private:
            void init_info() noexcept;

        public:
            explicit ConsoleOut(std::string_view ar_outname);

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
     * @brief ConsoleOut
     * 
     * Belirtilen isim ile çıktı vericiyi
     * oluşturmasını sağlıyoruz
     * 
     * @param string_view Name
     */
    ConsoleOut::ConsoleOut
    (
        std::string_view ar_outname
    ) :Output(ar_outname)
    {
        this->init_info();
    }

    /**
     * @brief Init Info
     * 
     * Konsol işlemi başladığı gibi bilgilendirmeyi dosyaya
     * kayıt edecek işlem
     */
    void ConsoleOut::init_info() noexcept
    {
        constexpr size_t tm_title_size = 20;

        std::scoped_lock lock(m_mtx);

        color::reset_color();

        std::cout << "================================================================================================================\n";
        std::cout << std::setw(tm_title_size) << std::left << "Name" << ": " << this->getName() << '\n';
        std::cout << std::setw(tm_title_size) << std::left << "Platform" << ": " << core::platform::current_pfm_name() << '\n';
        std::cout << std::setw(tm_title_size) << std::left << "Operating System" << ": " << core::platform::current_os_name() << '\n';
        std::cout << std::setw(tm_title_size) << std::left << "Timestamp" << ": " << tools::time::current_timestamp() << '\n';
        std::cout << "================================================================================================================\n";
    }

    /**
     * @brief Write
     * 
     * Duruma göre çıktı verecek ve bu sayede
     * ne olduğunun anlaşılması daha rahat olacak
     * 
     * @note Durumda çıktı verilecek çünkü ne olduğunu anlamalıyız
     * 
     * @param level_t Status
     * @param string_view Message
     */
    void ConsoleOut::write(
        level_t ar_lvl,
        std::string_view ar_msg
    ) noexcept
    {
        std::scoped_lock lock(m_mtx);

        color::set_color(static_cast<color::color_t>(ar_lvl));
        std::cout << Output::getTitle(to_string(ar_lvl)) << ' ' << ar_msg << '\n';
        color::reset_color();
    }

    /**
     * @brief Write
     * 
     * Başlık ile çıktı verecek ve bu sayede
     * ne olduğunun anlaşılması daha rahat olacak
     * 
     * @note Durum belirtilmediği için renk ayarlaması yok
     * 
     * @param string_view Title
     * @param string_view Message
     */
    void ConsoleOut::write(
        std::string_view ar_title,
        std::string_view ar_msg
    ) noexcept
    {
        std::scoped_lock lock(m_mtx);
        std::cout << Output::getTitle(ar_title) << ' ' << ar_msg << '\n';
    }

    /**
     * @brief Write
     * 
     * Sadece mesaj ile çıktı verecek ve bu sayede
     * ne olduğunun anlaşılması daha rahat olacak
     * 
     * @note Durum belirtilmediği için renk ayarlaması yok
     * @note Başlık da belirtilmediği için başlık kısmı hiç yok
     * 
     * @param string_view Message
     */
    void ConsoleOut::write(
        std::string_view ar_msg
    ) noexcept
    {
        std::scoped_lock lock(m_mtx);
        std::cout << ar_msg << '\n';
    }
}
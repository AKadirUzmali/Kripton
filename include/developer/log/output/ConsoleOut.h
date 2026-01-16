// Abdulkadir U. - 15/01/2026
#pragma once

/**
 * Console Output
 * 
 * Oluşan durum hakkında bir başlık ve metin ile
 * insanın okuyabileceği basit bir çıktı vermeyi
 * yöntemini kullanıyoruz ve konsol ekranından
 * bunu okuyarak ne olduğunu daha rahat anlayabileceğiz
 */

// Include:
#include <iostream>
#include <string>
#include <mutex>

#include <developer/log/Output.h>

// Namespace:
namespace dev::output::console
{
    // Using Namespace:
    using namespace dev::level;

    // Class:
    class ConsoleOut : public virtual Output
    {
        private:
            inline static std::mutex mtx {};

        private:
            void init_info() noexcept;

        public:
            explicit ConsoleOut(std::string_view outname);

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
     * @brief ConsoleOut
     * 
     * Belirtilen isim ile çıktı vericiyi
     * oluşturmasını sağlıyoruz
     * 
     * @param string_view Name
     */
    ConsoleOut::ConsoleOut
    (
        std::string_view outname
    ) :Output(outname)
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
        constexpr size_t title_size = 20;

        std::scoped_lock lock(mtx);

        tools::console::reset_color();

        std::cout << "================================================================================================================\n";
        std::cout << std::setw(title_size) << std::left << "Name" << ": " << this->getName().data() << '\n';
        std::cout << std::setw(title_size) << std::left << "Platform" << ": " << tools::os::current_pfm_name() << '\n';
        std::cout << std::setw(title_size) << std::left << "Operating System" << ": " << tools::os::current_os_name() << '\n';
        std::cout << std::setw(title_size) << std::left << "Timestamp" << ": " << tools::time::current_timestamp() << '\n';
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
     * @param Level Status
     * @param string_view Message
     */
    void ConsoleOut::write(
        Level lvl,
        std::string_view msg
    ) noexcept
    {
        std::scoped_lock lock(mtx);

        tools::console::set_color(tools::console::get_color(to_index(lvl)));
        std::cout << Output::getTitle(level::to_string(lvl).data()) << ' ' << msg << '\n';
        tools::console::reset_color();
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
        std::string_view title,
        std::string_view msg
    ) noexcept
    {
        std::scoped_lock lock(mtx);
        std::cout << Output::getTitle(title) << ' ' << msg << '\n';
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
        std::string_view msg
    ) noexcept
    {
        std::scoped_lock lock(mtx);
        std::cout << msg << '\n';
    }
}
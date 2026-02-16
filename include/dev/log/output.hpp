// Abdulkadir U. - 12/01/2026
// Abdulkadir U. - 21/01/2026
#pragma once

/**
 * Output (Çıktı)
 * 
 * Oluşan durum hakkında bir başlık ve metin ile
 * insanın okuyabileceği basit bir çıktı vermeyi
 * yöntemini kullanıyoruz ve konsol ekranından
 * bunu okuyarak ne olduğunu daha rahat anlayabileceğiz
 */

// Include:
#include <string_view>

#include <dev/log/levels.hpp>
#include <kits/toolkit.hpp>

// Namespace:
namespace dev::output
{
    // Using Namespace
    using namespace dev::level;

    // Class:
    class Output
    {
        public:
            static inline std::string getTitle(
                std::string_view ar_title
            ) noexcept;

        private:
            std::string m_name;

        public:
            explicit Output(std::string_view ar_outname);
            virtual ~Output() = default;

        public:
            virtual void write(
                level_t ar_lvl,
                std::string_view ar_msg
            ) noexcept = 0;

            virtual void write(
                std::string_view ar_title,
                std::string_view ar_msg
            ) noexcept = 0;

            virtual void write(
                std::string_view ar_msg
            ) noexcept = 0;

            virtual void print() noexcept = 0;

            inline const char* getName() const noexcept;
    };

    /**
     * @brief Output
     * 
     * Belirtilen isim ile çıktı vericiyi
     * oluşturmasını sağlıyoruz
     * 
     * @param string_view Name
     */
    Output::Output(
        std::string_view ar_outname
    )
    : m_name(ar_outname)
    {}

    /**
     * @brief Get Title
     * 
     * Verilen başlığı belirtilen standarta göre
     * geri döndürecek
     * 
     * @note Output: [title]
     * 
     * @param string_view Title
     * @return string
     */
    std::string Output::getTitle(
        std::string_view ar_title
    ) noexcept
    {
        std::string tm_out;
        
        tm_out.reserve(ar_title.size() + 3);
        tm_out.push_back('[');
        tm_out.append(ar_title);
        tm_out.push_back(']');

        return tm_out;
    }

    /**
     * @brief Get Name
     * 
     * Çıktı yöntemi için verilmiş isimi getirsin
     * ve bu isim istenirse konsol ekranı için ya da
     * dosya adı veya diğer şeyler içinde kullanılabilir
     * 
     * @return const char*
     */
    const char* Output::getName() const noexcept
    {
        return this->m_name.data();
    }
}
// Abdulkadir U. - 12/01/2026
#pragma once

/**
 * Output
 * 
 * Oluşan durum hakkında bir başlık ve metin ile
 * insanın okuyabileceği basit bir çıktı vermeyi
 * yöntemini kullanıyoruz ve konsol ekranından
 * bunu okuyarak ne olduğunu daha rahat anlayabileceğiz
 */

// Include:
#include <string_view>

#include <kits/ToolKit.h>
#include <developer/log/Levels.h>

// Namespace:
namespace dev::output
{
    // Class:
    class Output
    {
        public:
            static inline std::string getTitle(
                std::string_view title
            ) noexcept;

        private:
            std::string_view name;

        public:
            explicit Output(std::string_view outname);

        public:
            virtual void write(
                dev::level::Level lvl,
                std::string_view msg
            ) noexcept = 0;

            virtual void write(
                std::string_view title,
                std::string_view msg
            ) noexcept = 0;

            virtual void write(
                std::string_view msg
            ) noexcept = 0;

            inline constexpr std::string_view getName() const noexcept;
    };

    /**
     * @brief Output
     * 
     * Belirtilen isim ile çıktı vericiyi
     * oluşturmasını sağlıyoruz
     * 
     * @param string_view Name
     */
    Output::Output
    (
        std::string_view outname
    ) :name(outname)
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
    std::string Output::getTitle
    (
        std::string_view title
    ) noexcept
    {
        std::string out;
        
        out.reserve(title.size() + 3);
        out.push_back('[');
        out.append(title);
        out.push_back(']');

        return out;
    }

    /**
     * @brief Get Name
     * 
     * Çıktı yöntemi için verilmiş isimi getirsin
     * ve bu isim istenirse konsol ekranı için ya da
     * dosya adı veya diğer şeyler içinde kullanılabilir
     * 
     * @return const string_view
     */
    constexpr std::string_view Output::getName() const noexcept
    {
        return this->name;
    }
}
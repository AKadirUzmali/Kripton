// Abdulkadir U. - 12/01/2026
// Abdulkadir U. - 22/01/2026
#pragma once

/**
 * Logger (Kayıt Edici)
 * 
 * Yapılanları basit bir şekilde kayıt tutmamızı sağlayacak
 * bu sayede daha temiz ve modern bir test sistemi ile
 * kayıt yapmış olacağız
 */

// Include
#include <string>
#include <string_view>
#include <vector>
#include <memory>

#include <dev/core/source.hpp>
#include <dev/log/levels.hpp>
#include <dev/log/output.hpp>

#include <kits/outputkit.hpp>
#include <kits/toolkit.hpp>

// Namespace
namespace dev::log
{
    // Using Namespace
    using namespace dev::output;
    using namespace dev::level;
    using namespace dev::source;

    // Class
    template<class... OutputC>
    class Logger final
    {
        private:
            std::vector<std::unique_ptr<Output>> m_outputs;
            std::atomic<std::size_t> m_tests[ss_size_tests] = { 0, 0, 0, 0, 0, 0 };

        private:
            template<class T>
            void add_output(std::string_view ar_name) {
                this->m_outputs.emplace_back(std::make_unique<T>(ar_name));
            }

            template<class T, class... ResT>
            void create_outputs(const std::vector<std::string_view>& ar_names, size_t ar_index = 0)
            {
                std::string_view tm_name = ar_index < ar_names.size() ?
                    ar_names[ar_index] : ar_names.back();

                this->add_output<T>(tm_name);

                if constexpr (sizeof...(ResT) > 0)
                    this->create_outputs<ResT...>(ar_names, ar_index + 1);
            }

        public:
            template<typename... NameT>
            explicit Logger(std::string_view ar_firstname, NameT&&... ar_rest);

            void write(const level_t ar_lvl, const std::string_view ar_msg, const Source ar_src) noexcept;
            void print() noexcept;
    };

    /**
     * @brief Logger
     * 
     * Belirtilen tür ve isimler ile kayıt nesneleri
     * oluşturarak daha rahat ve okunabilir ve geliştirilebilir
     * bir kayıt sistemi oluşturur
     * 
     * @param string_view Firstname
     * @tparam NameT&&... Rest
     */
    template<class... OutputC>
    template<typename... NameT>
    Logger<OutputC...>::Logger(
        std::string_view ar_firstname,
        NameT&&... ar_rest
    )
    {
        static_assert(sizeof...(OutputC) > 0 );

        std::vector<std::string_view> tm_names = { ar_firstname, std::forward<NameT>(ar_rest)... };
        this->create_outputs<OutputC...>(tm_names);
    }

    /**
     * @brief Write
     * 
     * Dosyaya verilen türe göre çıktı vermeyi sağlar
     * 
     * @param level_t Level
     * @param string_view Message
     * @param Source Src
     */
    template<class... OutputC>
    void Logger<OutputC...>::write(
        const level_t ar_lvl,
        const std::string_view ar_msg,
        const Source ar_src
    ) noexcept
    {
        std::string tm_text;
        tm_text.reserve(256);

        tm_text.push_back('[');
        tm_text.append(tools::time::current_timestamp());
        tm_text.push_back('|');
        tm_text.append(ar_src.m_file);
        tm_text.push_back(':');
        tm_text.append(ar_src.m_func);
        tm_text.push_back(':');
        tm_text.append(std::to_string(ar_src.m_line));
        tm_text.append("] ");
        tm_text.append(ar_msg);

        for(auto& tm_out : this->m_outputs)
            tm_out->write(ar_lvl, tm_text);

        ++this->m_tests[get_valid_index(to_index(ar_lvl))];
    }

    /**
     * @brief Print
     * 
     * Dosya hakkında bilgi çıktısı verecek
     */
    template<class... OutputC>
    void Logger<OutputC...>::print() noexcept
    {
        std::string tm_text;
        tm_text.reserve(64);

        for(auto& tm_out : this->m_outputs)
            tm_out->print();

        for(size_t tm_count = 0; tm_count < ss_size_tests; ++tm_count) {
            const auto tm_idx = get_valid_index(tm_count);

            tm_text.clear();

            tm_text.append(level::to_string(tm_count));
            tm_text.append(": ");
            tm_text.append(std::to_string(this->m_tests[get_valid_index(tm_idx)].load(std::memory_order_relaxed)));

            for(auto& tm_out : this->m_outputs)
                tm_out->write("Logger", tm_text);
        }
    }
}
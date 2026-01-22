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
#include <type_traits>
#include <tuple>

#include <dev/core/source.hpp>
#include <dev/config/config.hpp>
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

    /**
     * @brief Output isimlerini indeks bazlı olarak seçer
     *
     * Logger birden fazla Output (FileOut, ConsoleOut, vb.) içerebilir.
     * Her Output için bir isim (string_view) verilmesi mümkündür fakat:
     *
     *  - Tek isim verilebilir
     *  - Output sayısından az isim verilebilir
     *  - Output sayısından fazla isim verilebilir
     *
     * Bu fonksiyon, Output indeksine (I) göre hangi ismin kullanılacağını
     * compile-time (if constexpr) olarak belirler.
     *
     * Seçim kuralları:
     *
     *  1) I == 0 ise:
     *     - Her zaman `first` kullanılır.
     *
     *  2) I > 0 ve yeterli sayıda ek isim varsa:
     *     - `rest` tuple içinden (I - 1). eleman alınır.
     *
     *  3) I > 0 fakat yeterli isim yoksa:
     *     - `rest` içindeki son isim tekrar kullanılır.
     *
     * Bu sayede:
     *  - Fazla isim verilmesi sorun olmaz
     *  - Az isim verilmesi sorun olmaz
     *  - Runtime kontrol veya branch oluşmaz
     *
     * @tparam I
     * Output’un indeksidir (0 → ilk Output, 1 → ikinci Output, ...)
     *
     * @tparam Names
     * Ek isimlerin türleri (genellikle std::string_view)
     *
     * @param first
     * İlk Output için kullanılacak isim
     *
     * @param rest
     * Diğer Output’lar için verilen ek isimleri içeren tuple
     * 
     * @note Fonksiyonun kullanım amacı ve ihtiyacı Abdulkadir U. tarafından belirlenmiştir
     * @note Fonksiyonun tasarımı ChatGPT tarafından yapılmıştır
     *
     * @return std::string_view
     * Seçilen Output ismi
     */
    template<std::size_t I, typename... Names>
    static std::string_view pick_name(
        std::string_view ar_first,
        const std::tuple<Names...>& ar_rest
    ) noexcept
    {
        if constexpr (I == 0)
            return ar_first;
        else if constexpr (I - 1 < sizeof...(Names))
            return std::get<I - 1>(ar_rest);
        else
            return std::get<sizeof...(Names) - 1>(ar_rest);
    }

    // Class:
    template<class... Outs>
    class Logger final
    {
        static_assert((std::is_base_of<Output, Outs>::value && ...), "All Logger Outputs Must Derive From Output");

        private:
            std::tuple<Outs...> m_outs;

        private:
            template<std::size_t... I, typename... Names>
            static std::tuple<Outs...> make_outs(
                std::index_sequence<I...>,
                std::string_view ar_first,
                const std::tuple<Names...>& ar_rest
            )
            {
                return std::tuple<Outs...>{
                    Outs(pick_name<I>(ar_first, ar_rest))...
                };
            }

        public:
            template<typename... Names>
            explicit Logger(std::string_view ar_firstname, Names... ar_rest);

            void write(
                level_t ar_lvl,
                std::string_view ar_msg,
                const Source ar_src
            ) noexcept;

            void print() noexcept;
    };

    /**
     * @brief Logger
     * 
     * Belirtilen isim ile çıktı verici sınıfı
     * kullanarak oluşturmasını sağlıyoruz
     * 
     * @param string_view Name
     */
    template<class... Outs>
    template<typename... Names>
    Logger<Outs...>::Logger
    (
        std::string_view ar_firstname,
        Names... ar_rest
    ) : m_outs(this->make_outs(std::index_sequence_for<Outs...>{}, ar_firstname, std::tuple<Names...>{ar_rest...}))
    {}

    /**
     * @brief Write
     * 
     * Normal çıktı işleminin aynısı fakat
     * bunu daha okunabilir yapmayı amaçlamak
     * 
     * @param level_t Status
     * @param string_view Message
     * @param Source Src
     */
    template<class... Outs>
    void Logger<Outs...>::write(
        level_t ar_lvl,
        std::string_view ar_msg,
        const Source ar_src
    ) noexcept
    {
        if constexpr (dev::config::logger) {
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

            std::apply([&](auto&... out) {
                (out.write(ar_lvl, std::string_view{tm_text}), ...);
            }, this->m_outs);

            ++ss_tests[get_valid_index(to_index(ar_lvl))];
        }
    }

    /**
     * @brief Print
     * 
     * Yapılan testlerin sonucunu bildirmek
     * amaçlı çıktı vermesini sağlamak
     */
    template<class... Outs>
    void Logger<Outs...>::print() noexcept
    {
        if constexpr (dev::config::logger) {
            std::string tm_result;
            tm_result.reserve(64);

            for(size_t tm_count = 0; tm_count < ss_size_tests; ++tm_count)
            {
                const auto tm_idx = get_valid_index(tm_count);
                if( !tm_count )
                    break;
                
                tm_result.clear();

                tm_result.append(level::to_string(tm_count));
                tm_result.append(": ");
                tm_result.append(std::to_string(ss_tests[get_valid_index(tm_idx)].load()));

                std::apply([&](auto&... out){
                    (out.write("Logger", std::string_view{tm_result}), ...);
                }, this->m_outs);
            }
        }
    }
}
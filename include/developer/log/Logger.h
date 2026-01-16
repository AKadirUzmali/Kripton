// Abdulkadir U. - 12/01/2026
#pragma once

/**
 * Logger
 * 
 * Yapılanları basit bir şekilde kayıt tutmamızı sağlayacak
 * bu sayede daha temiz ve modern bir test sistemi ile
 * kayıt yapmış olacağız
 */

// Include:
#include <string>
#include <type_traits>
#include <tuple>

#include <developer/core/Source.h>
#include <developer/config/Config.h>
#include <developer/log/Levels.h>

#include <kits/OutputKit.h>
#include <kits/ToolKit.h>

// Namespace:
namespace dev::log
{
    // Using Namespace:
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
        std::string_view first,
        const std::tuple<Names...>& rest
    ) noexcept
    {
        if constexpr (I == 0)
            return first;
        else if constexpr (I - 1 < sizeof...(Names))
            return std::get<I - 1>(rest);
        else
            return std::get<sizeof...(Names) - 1>(rest);
    }

    // Class:
    template<class... Outs>
    class Logger final
    {
        static_assert((std::is_base_of<Output, Outs>::value && ...), "All Logger Outputs Must Derive From Output");

        private:
            std::tuple<Outs...> outs;

        private:
            template<std::size_t... I, typename... Names>
            static std::tuple<Outs...> make_outs(
                std::index_sequence<I...>,
                std::string_view first,
                const std::tuple<Names...>& rest
            )
            {
                return std::tuple<Outs...>{
                    Outs(pick_name<I>(first, rest))...
                };
            }

        public:
            template<typename... Names>
            explicit Logger(std::string_view firstname, Names... rest);

            void write(
                Level lvl,
                std::string_view msg,
                const Source src
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
        std::string_view firstname,
        Names... rest
    ) : outs(this->make_outs(std::index_sequence_for<Outs...>{}, firstname, std::tuple<Names...>{rest...}))
    {}

    /**
     * @brief Write
     * 
     * Normal çıktı işleminin aynısı fakat
     * bunu daha okunabilir yapmayı amaçlamak
     * 
     * @param Level Status
     * @param string_view Message
     * @param Source Src
     */
    template<class... Outs>
    void Logger<Outs...>::write(
        Level lvl,
        std::string_view msg,
        const Source src
    ) noexcept
    {
        if constexpr (dev::config::logger) {
            std::string text;
            text.reserve(256);

            text.push_back('[');
            text.append(tools::time::current_timestamp());
            text.push_back('|');
            text.append(src.file);
            text.push_back(':');
            text.append(src.func);
            text.push_back(':');
            text.append(std::to_string(src.line));
            text.append("] ");
            text.append(msg);

            std::apply([&](auto&... out) {
                (out.write(lvl, std::string_view{text}), ...);
            }, this->outs);

            const size_t arr_size = to_index(lvl);
            arr_size < tests.size() ? ++tests[arr_size] : ++tests[to_index(Level::Null)];
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
            for(size_t count = 0; count < tests.size(); ++count)
            {
                std::string result(dev::level::to_string(count) + ": " + std::to_string(tests[count].load()));

                std::apply([&](auto&... out){
                    (out.write("Logger", std::string_view{result}), ...);
                }, this->outs);
            }
        }
    }
}
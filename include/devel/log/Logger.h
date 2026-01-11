// Abdulkadir U. - 11/01/2026
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

#include <devel/config/DevelFeatures.h>
#include <devel/log/Levels.h>
#include <devel/log/Sinks.h>

// Namespace:
namespace devel::log
{
    // Struct:
    struct Logger
    {
        /**
         * @brief Write
         * 
         * Normal çıktı işleminin aynısı fakat
         * bunu daha okunabilir yapmayı amaçlamak
         * 
         * @note Performanslı olması için inline yapıldı
         * 
         * @tparam Sink Sink 
         * @param Level Status
         * @param string_view Message
         */
        template<typename Sink = ConsoleSink>
        static inline void write(
            Level lvl,
            std::string_view msg
        )
        {
            if constexpr (devel::features::logger) {
                Sink::write(lvl, msg);
                
                const size_t arr_size = devel::level::to_index(lvl);
                arr_size < tests.size() ? ++tests[arr_size] : ++tests[devel::level::to_index(Level::Null)];
            }
        }

        /**
         * @brief Print
         * 
         * Test sonuçlarının çıktısını vererekten
         * ne olup bittiğini geliştiriciye haberdar eder
         * 
         * @tparam Sink Sink
         */
        template<typename Sink = ConsoleSink>
        static inline void print() noexcept
        {
            if constexpr (devel::features::logger) {
                for(size_t counter = 0; counter < tests.size(); ++counter)
                    Sink::write("[ LOGGER ] " + std::string(devel::level::to_string(counter)) + ": " + std::to_string(tests[counter].load()));
            }
        }
    };
}
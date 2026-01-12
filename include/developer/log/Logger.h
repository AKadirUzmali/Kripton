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

#include <developer/core/Source.h>
#include <developer/config/Config.h>
#include <developer/log/Levels.h>
#include <developer/log/Output.h>

// Namespace:
namespace dev::log
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
         * @param Level Status
         * @param string_view Message
         */
        template<typename Writer = dev::output::Console>
        static inline void write(
            dev::level::Level lvl,
            const std::string& msg
        )
        {
            if constexpr (dev::config::logger)
                Writer::write(lvl, msg);
        }

        /**
         * @brief Write
         * 
         * Normal çıktı işleminin aynısı fakat
         * bunu daha okunabilir yapmayı amaçlamak
         * 
         * @note Performanslı olması için inline yapıldı
         * 
         * @param Level Status
         * @param string_view Message
         * @param Source& Src
         */
        template<typename Writer = dev::output::Console>
        static inline void write(
            dev::level::Level lvl,
            const std::string& msg,
            const dev::source::Source src
        )
        {
            if constexpr (dev::config::logger) {
                std::string text = '[' + std::string(src.file) + ':'
                    + std::string(src.func) + ':'
                    + std::to_string(src.line) + "] ";

                Writer::write(lvl, text + msg);
            }
        }

        /**
         * @brief Print
         * 
         * Test sonuçlarının çıktısını vererekten
         * ne olup bittiğini geliştiriciye haberdar eder
         */
        template<typename Writer = dev::output::Console>
        static inline void print() noexcept
        {
            if constexpr (dev::config::logger) {
                for(size_t counter = 0; counter < dev::level::tests.size(); ++counter)
                    Writer::write("LOGGER", std::string(dev::level::to_string(counter)) + ": " + std::to_string(dev::level::tests[counter].load()));
            }
        }
    };
}
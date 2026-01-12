// Abdulkadir U. - 12/01/2026
#pragma once

/**
 * Trace
 * 
 * Belirli bir aralıkta yapılan işlemleri
 * zamansal olarak hesaplamak için gereken bir yapı
 */

// Include:
#include <string>

#include <developer/config/Config.h>
#include <developer/trace/Timeline.h>
#include <developer/log/Levels.h>
#include <developer/log/Logger.h>

// Namespace:
namespace dev::trace
{
    // Struct:
    struct Scope
    {
        std::string name;
        time_point  start;

        explicit Scope(const std::string& n)
            : name(n)
        {
            if constexpr (dev::config::trace)
                start = clock::now();
        }

        ~Scope()
        {
            if constexpr (dev::config::trace) {
                const auto end = clock::now();
                const auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

                dev::log::Logger::write(
                    dev::level::Level::Info, name + " : " + std::to_string(dur.count()) + " us");
            }
        }
    };
}

// Define:
#if __DEVELOPER__
    #define TRACE_SCOPE(name) dev::trace::Scope _dev_scope_##__LINE__{ name }
    #define TRACE_FUNC(name)  TRACE_SCOPE(__func__)
#else
    #define TRACE_SCOPE(name) ((void)0)
    #define TRACE_FUNC(name)  ((void)0)
#endif
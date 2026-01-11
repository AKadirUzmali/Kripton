// Abdulkadir U. - 11/01/2026
#pragma once

/**
 * Trace
 * 
 * Belirli bir aralıkta yapılan işlemleri
 * zamansal olarak hesaplamak için gereken bir yapı
 */

// Include:
#include <devel/config/DevelFeatures.h>
#include <devel/trace/Timeline.h>

// Namespace:
namespace devel::trace
{
    // Struct:
    struct Scope
    {
        const char* name;
        time_point  start;

        explicit Scope(const char* n) noexcept
            : name(n)
        {
            if constexpr (devel::features::trace)
                start = clock::now();
        }

        ~Scope() noexcept
        {
            if constexpr (devel::features::trace) {
                const auto end = clock::now();
                const auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

                ::devel::log::Logger::write(
                    ::devel::log::Level::Info, std::string(name) + " : " + std::to_string(dur.count()) + " microsec");
            }
        }
    };
}

// Define:
#if __DEVELOPER__
    #define TRACE_SCOPE(name) ::devel::trace::Scope _dev_scope_##__LINE__{ name }
    #define TRACE_FUNC(name)  TRACE_SCOPE(__func__)
#else
    #define TRACE_SCOPE(name) ((void)0)
    #define TRACE_FUNC(name)  ((void)0)
#endif
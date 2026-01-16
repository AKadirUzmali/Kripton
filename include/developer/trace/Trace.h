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
    // Using Namespace:
    using namespace dev::level;
    using namespace dev::source;
    using namespace dev::log;
    using namespace dev::output::console;

    // Class:
    template<class LoggerT>
    class Scope final
    {
        private:
            LoggerT& logger;
            std::string_view name;
            time_point start {};
            Source src;

        public:
            explicit Scope(LoggerT& log, std::string_view outname, const Source source);
            ~Scope();
    };

    /**
     * @brief Scope
     */
    template<class LoggerT>
    Scope<LoggerT>::Scope
    (
        LoggerT& log,
        std::string_view outname,
        const Source source
    ) : logger(log), name(outname), src(source)
    {
        if constexpr (dev::config::trace) {
            start = clock::now();
        }
    }

    /**
     * @brief ~Scope
     */
    template<class LoggerT>
    Scope<LoggerT>::~Scope()
    {
        if constexpr (dev::config::trace) {
            const auto end = clock::now();
            const auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end -start);

            std::string text;

            text.reserve(this->name.size() + 32);
            text.append(name);
            text.append(": ");
            text.append(std::to_string(dur.count()));
            text.append(" us");

            this->logger.write(Level::Info, std::string_view{text}, this->src);
        }
    }
}

// Define:
#if __DEVELOPER__
    #define TRACE_SCOPE(logger, name) dev::trace::Scope<std::decay_t<decltype(logger)>> \
        _dev_scope_##__LINE__{ logger, \
            name, \
            dev::source::Source{__FILE__, __func__, __LINE__} \
        }
    #define TRACE_FUNC(logger, name)  TRACE_SCOPE(logger, __func__)
#else
    #define TRACE_SCOPE(logger, name) (sizeof(logger))
    #define TRACE_FUNC(logger, name)  (sizeof(logger))
#endif
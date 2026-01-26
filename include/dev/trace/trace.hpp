// Abdulkadir U. - 12/01/2026
// Abdulkadir U. - 21/01/2026
#pragma once

/**
 * Trace (İz)
 * 
 * Belirli bir aralıkta yapılan işlemleri
 * zamansal olarak hesaplamak için gereken bir yapı
 */

// Include
#include <cstring>
#include <string>

#include <dev/trace/timeline.hpp>
#include <dev/log/levels.hpp>
#include <dev/log/logger.hpp>

// Namespace
namespace dev::trace
{
    // Using Namespace
    using namespace dev::level;
    using namespace dev::source;
    using namespace dev::log;
    using namespace dev::output::console;
    using namespace dev::trace::timeline;

    // Template
    template<typename T>
    struct duration_suffix;

    template<>
    struct duration_suffix<std::chrono::microseconds> { static constexpr const char* const ss_str = "us"; };

    template<>
    struct duration_suffix<std::chrono::milliseconds> { static constexpr const char* const ss_str = "ms"; };

    template<>
    struct duration_suffix<std::chrono::seconds> { static constexpr const char* const ss_str = "sec"; };

    template<>
    struct duration_suffix<std::chrono::minutes> { static constexpr const char* const ss_str = "min"; };

    template<>
    struct duration_suffix<std::chrono::hours> { static constexpr const char* const ss_str = "hr"; };

    // Class
    template<class LoggerT, class DurationT = std::chrono::milliseconds>
    class Scope final
    {
        private:
            LoggerT& m_logger;
            const char* const m_name;
            const std::size_t m_name_len;
            time_point m_start;
            Source m_src;

        public:
            explicit Scope(
                LoggerT& ar_logger,
                const char* const ar_name,
                const Source ar_src
            );

            ~Scope();
    };

    /**
     * @brief Scope
     * 
     * Belirtilen kayıt edici nesneyi referans olarak alır
     * ve yapılacak işleme bir isim verir ve nerede olduğu
     * bilgisin de source ile alır. Bu sayede kayıt tutma
     * işlemleri için çok faydalı olur.
     * 
     * @note Sadece geliştirici modu için tasarlanmıştır
     * 
     * @tparam LoggerT& Logger
     * @param char* Name
     * @param Source Source
     */
    template<class LoggerT, class DurationT>
    Scope<LoggerT, DurationT>::Scope(
        LoggerT& ar_logger,
        const char* const ar_name,
        const Source ar_src
    )
    :   m_logger(ar_logger),
        m_name(ar_name),
        m_name_len(std::strlen(this->m_name)),
        m_start(clock::now()),
        m_src(ar_src)
    {}

    /**
     * @brief ~Scope
     * 
     * Varolan işlemin bitme süresini alır ve başlangıçtaki
     * süre ile karşılaştırıp not tutar. Not tutulan süreyi
     * bilgilendirme amaçlı mikrosaniye olarak çıktı verir
     * 
     * @note Sadece geliştirici modu için tasarlanmıştır
     */
    template<class LoggerT, class DurationT>
    Scope<LoggerT, DurationT>::~Scope()
    {
        static constexpr std::size_t ss_max_name_len = 64;
        static constexpr std::size_t ss_buffer_size = ss_max_name_len + 32;

        const auto tm_end = clock::now();
        const auto tm_dur =
            std::chrono::duration_cast<DurationT>(tm_end - this->m_start);

        char tm_buffer[ss_buffer_size];

        const std::size_t tm_name_len = (this->m_name_len < ss_max_name_len) ?
            this->m_name_len : ss_max_name_len;

        const int tm_len = std::snprintf(
            tm_buffer,
            sizeof(tm_buffer),
            "%.*s: %lld %s",
            static_cast<int>(tm_name_len),
            this->m_name,
            static_cast<long long>(tm_dur.count()),
            duration_suffix<DurationT>::ss_str
        );

        if( tm_len < 1 )
            return;

        const std::size_t tm_out_len =
            (static_cast<std::size_t>(tm_len) < sizeof(tm_buffer))
                ? static_cast<std::size_t>(tm_len)
                : sizeof(tm_buffer) - 1;

        this->m_logger.write(
            level_t::Info,
            std::string_view{ tm_buffer, tm_out_len },
            m_src
        );
    }
}
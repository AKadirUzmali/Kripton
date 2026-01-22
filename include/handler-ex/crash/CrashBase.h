// Abdulkadir U. - 01/12/2025
// Abdulkadir U. - 19/01/2026
#pragma once

/**
 * Crash Base (Çökme Temeli)
 * 
 * Program bazen bir sinyal nedeniyle ya da
 * beklenmedik bir hata sonucunda çökebilir.
 * Aktif olarak çalışan sistemleri başarılı
 * şekilde sonlandırmak için hata kontrolcüsüne
 * ihtiyacımız olur yoksa aksi halde sistem
 * belleği sızıntıları ya da dosyalarda bozulmalara
 * yol açabilir ki bu durum istenmeyen bir durumdur.
 * Temel bir çökme işleyen sınıf oluşturup diğer sınıflarda
 * bunu kullanacağız.
 * 
 * Güncelleme: 19/01/2026
 * 
 * Bazı gereksiz açıklama metinleri silindi ve
 * namespace isimlendirmesinde değişikliğe gidildi.
 * Bunun dışında versiyon kontrol sistemi eklendi
 */

// Include:
#include <iostream>
#include <vector>
#include <mutex>
#include <algorithm>

#include <csignal>
#include <cstdlib>

#include <kits/OutputKit.h>
#include <developer/Developer.h>

// Namespace:
namespace handler::crash
{
    // Class:
    class CrashBase
    {
        private:
            static inline std::vector<CrashBase*> s_instances {};
            static inline std::mutex s_list_mutex {};
            static inline bool s_initialized { false };
            static inline int signal_code { 0 };

        public:
            static inline const dev::version::Version ver { 1, 0, 0 };

        public:
            virtual ~CrashBase() noexcept;

            static inline int getSignal() noexcept;

        protected:
            CrashBase() noexcept;

        private:
            static void registerInstance(CrashBase*) noexcept;
            static void unregisterInstance(CrashBase*) noexcept;
            static void runCrashHandlers() noexcept;
            static void ensureInit() noexcept;
            static void installHandlers() noexcept;
            static void onSignal(int) noexcept;

        public:
            virtual void onCrash() noexcept {}
    };

    /**
     * @brief Destructor
     * 
     * Kayıtlı örneği temizler.
     */
    CrashBase::~CrashBase() noexcept
    {
        unregisterInstance(this);
    }

    /**
     * @brief Constructor
     * 
     * Başladığından emin olur ve
     * bu örneği kayıt eder.
     */
    CrashBase::CrashBase() noexcept
    {
        ensureInit();
        registerInstance(this);
    }

    /**
     * @brief Get Signal
     * 
     * Sinyal değerini döndürür
     * 
     * @return int
     */
    int CrashBase::getSignal() noexcept
    {
        return signal_code;
    }

    /**
     * @brief Register Instance
     * 
     * Kayıtlı örneği ayarlar.
     * 
     * @param CrashBase* Instance Pointer
     */
    [[maybe_unused]]
    void CrashBase::registerInstance(CrashBase* instance) noexcept
    {
        std::scoped_lock<std::mutex> lock(s_list_mutex);
        s_instances.push_back(instance);
    }

    /**
     * @brief Unregister Instance
     * 
     * Kayıtlı örneği kaldırır.
     * 
     * @param CrashBase* Instance Pointer
     */
    [[maybe_unused]]
    void CrashBase::unregisterInstance(CrashBase* instance) noexcept
    {
        std::scoped_lock<std::mutex> lock(s_list_mutex);
        auto it = std::remove(s_instances.begin(), s_instances.end(), instance);
        if( it != s_instances.end() )
            s_instances.erase(it, s_instances.end());
    }

    /**
     * @brief Run Crash Handlers
     * 
     * Kayıtlı tüm örneklerin onCrash metodunu çağırır.
     */
    [[maybe_unused]]
    void CrashBase::runCrashHandlers() noexcept
    {
        std::scoped_lock lock(s_list_mutex);
        for( auto it = s_instances.rbegin(); it != s_instances.rend(); ++it )
        {
            if( *it )
            {
                try {
                    (*it)->onCrash();
                } catch(...) { }
            }
        }
    }

    /**
     * @brief Ensure Initialization
     * 
     * Başlatıldığından emin olur.
     */
    [[maybe_unused]]
    void CrashBase::ensureInit() noexcept
    {
        if( s_initialized )
            return;

        s_initialized = true;
        installHandlers();
    }

    /**
     * @brief Install Handlers
     * 
     * Çökme işleyicilerini kurar.
     */
    [[maybe_unused]]
    void CrashBase::installHandlers() noexcept
    {
        std::signal(SIGSEGV, onSignal);
        std::signal(SIGABRT, onSignal);
        std::signal(SIGFPE,  onSignal);
        std::signal(SIGILL,  onSignal);
        std::signal(SIGTERM, onSignal);
        std::signal(SIGINT,  onSignal);
    }

    /**
     * @brief [Private] On Signal
     * 
     * Sinyal işleyici. Sinyal hatası durumunda çalışacak.
     * 
     * @param int Signal Code
     */
    [[maybe_unused]]
    void CrashBase::onSignal(int signal) noexcept
    {
        {
            std::scoped_lock lock(s_list_mutex);
            signal_code = signal;
        }

        runCrashHandlers();
        std::exit(EXIT_FAILURE);
    }
}
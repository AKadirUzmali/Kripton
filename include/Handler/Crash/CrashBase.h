// Abdulkadir U. - 01/12/2025
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
 */

// Include:
#include <Platform/Platform.h>

#if defined __PLATFORM_DOS__
    #include <windows.h>
#endif

#include <vector>
#include <mutex>
#include <algorithm>

#include <csignal>
#include <cstdlib>

// Namespace: Core::Handler
namespace core::handler
{
    // Class: CrashBase
    class CrashBase
    {
        private:
            static inline std::vector<CrashBase*> s_instances {};
            static inline std::mutex s_list_mutex {};
            static inline bool s_initialized { false };

        public:
            virtual ~CrashBase() noexcept;
            virtual void onCrash() noexcept {}

        protected:
            CrashBase() noexcept;

        private:
            static void registerInstance(CrashBase*) noexcept;
            static void unregisterInstance(CrashBase*) noexcept;
            static void runCrashHandlers() noexcept;
            static void ensureInit() noexcept;
            static void installHandlers() noexcept;

            #if defined __PLATFORM_POSIX__
                static void onSignal(int) noexcept;
            #elif defined __PLATFORM_DOS__
                static LONG WINAPI onException(EXCEPTION_POINTERS*) noexcept;
            #else
                #error "Crash handlers are not implemented for this platform."
            #endif
    };

    /**
     * @brief [Public] Destructor
     * 
     * Kayıtlı örneği temizler.
     */
    CrashBase::~CrashBase() noexcept
    {
        unregisterInstance(this);
    }

    /**
     * @brief [Protected] Constructor
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
     * @brief [Private] Register Instance
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
     * @brief [Private] Unregister Instance
     * 
     * Kayıtlı örneği kaldırır.
     * 
     * @param CrashBase* Instance Pointer
     */
    [[maybe_unused]]
    void CrashBase::unregisterInstance(CrashBase* instance) noexcept
    {
        std::scoped_lock<std::mutex> lock(s_list_mutex);
        s_instances.erase(
            std::remove(s_instances.begin(), s_instances.end(), instance),
            s_instances.end()
        );
    }

    /**
     * @brief [Private] Run Crash Handlers
     * 
     * Kayıtlı tüm örneklerin onCrash metodunu çağırır.
     */
    [[maybe_unused]]
    void CrashBase::runCrashHandlers() noexcept
    {
        std::scoped_lock<std::mutex> lock(s_list_mutex);

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
     * @brief [Private] Ensure Initialization
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
     * @brief [Private] Install Handlers
     * 
     * Çökme işleyicilerini kurar.
     */
    [[maybe_unused]]
    void CrashBase::installHandlers() noexcept
    {
        #if defined __PLATFORM_POSIX__
            std::signal(SIGSEGV, onSignal);
            std::signal(SIGABRT, onSignal);
            std::signal(SIGFPE,  onSignal);
            std::signal(SIGILL,  onSignal);
            std::signal(SIGTERM, onSignal);
            std::signal(SIGINT,  onSignal);
        #elif defined __PLATFORM_DOS__
            SetUnhandledExceptionFilter(&onException);
        #endif
    }

    #if defined __PLATFORM_POSIX__
        /**
         * @brief [Private] On Signal
         * 
         * Sinyal işleyici.
         * 
         * @param int Signal Code
         */
        [[maybe_unused]]
        void CrashBase::onSignal(int signal) noexcept
        {
            (void)signal;
            runCrashHandlers();
            std::exit(EXIT_FAILURE);
        }
    #elif defined __PLATFORM_DOS__
        /**
         * @brief [Private] On Exception
         * 
         * İstisna işleyici.
         * 
         * @param EXCEPTION_POINTERS* Exception Pointers
         * @return LONG
         */
        [[maybe_unused]]
        LONG WINAPI CrashBase::onException(EXCEPTION_POINTERS* exceptionInfo) noexcept
        {
            runCrashHandlers();
            std:exit(EXIT_FAILURE);
        }
    #endif
}
// Abdulkadir U. - 24/01/2026
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

// Include
#include <iostream>
#include <vector>
#include <mutex>
#include <atomic>
#include <algorithm>

#include <csignal>
#include <cstdlib>

// Namespace
namespace core::crash
{
    // Class
    class CrashHandler
    {
        private:
            static constexpr int ss_signal_list[] =
            { SIGSEGV, SIGABRT, SIGFPE, SIGILL, SIGTERM, SIGINT };

            static constexpr uint16_t ss_size_signals = sizeof(ss_signal_list) / sizeof(ss_signal_list[0]);

            static inline std::vector<CrashHandler*> s_instances {};
            static inline std::mutex s_list_mtx {};

            static inline std::atomic<int> s_signal_code { 0 };
            static inline std::atomic<bool> s_initialized { false };

        private:
            static void init() noexcept;

            static void install_handlers() noexcept;
            static void run_handlers() noexcept;

            static void register_instance(CrashHandler* ar_handler) noexcept;
            static void unregister_instance(CrashHandler* ar_handler) noexcept;

            static void signal_handler(int ar_signal) noexcept;

        protected:
            CrashHandler() noexcept;

        public:
            CrashHandler(const CrashHandler&) = delete;
            CrashHandler& operator=(const CrashHandler&) = delete;

            ~CrashHandler() noexcept;

            [[nodiscard]] static inline bool is_signal() noexcept;
            [[nodiscard]] static inline int get_signal() noexcept;

        protected:
            virtual void crashed() noexcept {};
    };

    /**
     * @brief CrashHandler
     * 
     * Başlangıç ayarlamalarını yapar ve
     * sonrasında ise şuan belirtilen nesneyi listeye
     * kayıt alır çünkü sonrasında kapatma işlemi
     * yaptığında sorun olmadan kapanmasını sağlar
     */
    CrashHandler::CrashHandler() noexcept
    {
        init();
        register_instance(this);
    }

    /**
     * @brief ~CrashHandler
     * 
     * Şuanki hata işleyicisini sonlandırmayı sağlar
     */
    CrashHandler::~CrashHandler() noexcept
    {
        unregister_instance(this);
    }

    /**
     * @brief Is Signal
     * 
     * Sinyal değerinin geçerli olup olmadığını
     * bildiren değer döndürür
     * 
     * @return bool
     */
    [[nodiscard]]
    bool CrashHandler::is_signal() noexcept
    {
        return s_signal_code.load(std::memory_order_relaxed) != 0;
    }

    /**
     * @brief Get Signal
     * 
     * Sinyal değerini döndürür
     * 
     * @return int
     */
    [[nodiscard]]
    int CrashHandler::get_signal() noexcept
    {
        return s_signal_code.load(std::memory_order_relaxed);
    }

    /**
     * @brief Init
     * 
     * Başlangıç işlemi sağlanmamışsa eğer
     * işlemleri yapar aksi halde tekrar tekrar
     * işlem yapmamak için fonksiyon sonlanır
     */
    void CrashHandler::init() noexcept
    {
        if( s_initialized.exchange(true) )
            return;

        install_handlers();
    }

    /**
     * @brief Install Handlers
     * 
     * Hata durum sinyallerini ayarlayarak sonrasında
     * sinyalin oluşması durumunda olan sinyalleri yakalar
     * ve belirtilen fonksiyonu çalıştırır
     */
    void CrashHandler::install_handlers() noexcept
    {
        for(int tm_sig : ss_signal_list) {
            std::signal(tm_sig, signal_handler);
        }
    }

    /**
     * @brief Register Instance
     * 
     * Yeni verilen hata kontrolcüsünü alır
     * ve boş olup olmadığını kontrol eder.
     * Sonra listeye yeni değer eklenmeden önce kilit
     * mekanizması ile kitler ve yeni kontrolcüyü
     * listeye ekler. Sonrasında ise kilit tekrar
     * açılır
     * 
     * @param CrashHandler* Handler
     */
    void CrashHandler::register_instance(
        CrashHandler* ar_handler
    ) noexcept
    {
        if( !ar_handler )
            return;

        std::scoped_lock tm_lock(s_list_mtx);

        if( std::find(s_instances.begin(), s_instances.end(), ar_handler) == s_instances.end() )
            s_instances.push_back(ar_handler);
    }

    /**
     * @brief Unregister Instance
     * 
     * Verilen hata kontrolcüsünü alır ve boş olup
     * olmadığını kontrol eder ve eğer boş ise fonksiyon durur.
     * Sonra listeye yeni değer eklenmeden önce kilit
     * mekanizması ile kitler ve kontrolcüyü listede aramaya başlar.
     * Listede silme işlemi yapar ama listenin başından sonuna
     * kadar elemanı arar ve bulduğunda elemanı döndürür
     * sonrasında ise eleman silinir aksi halde zaten liste
     * sonuna gelineceği için eleman silinememiş olur
     * 
     * @param CrashHandler* Handler
     */
    void CrashHandler::unregister_instance(
        CrashHandler* ar_handler
    ) noexcept
    {
        if( !ar_handler )
            return;

        std::scoped_lock tm_lock(s_list_mtx);

        if( std::find(s_instances.begin(), s_instances.end(), ar_handler) == s_instances.end() )
            return;

        s_instances.erase(
            std::remove(s_instances.begin(), s_instances.end(), ar_handler),
            s_instances.end()
        );
    }

    /**
     * @brief Run Handlers
     * 
     * Bir vektör ile çökme yakalayıcıları listesi tutar.
     * Listenin başlangıç elemanı kilit mekanizması sayesinde
     * alınır. Liste bitene kadar döngü ile işaretçi yoksa eğer
     * sonraki tura geçer ama işaretçi varsa işlemleri yapar.
     * Hata oluşması durumunda çalışması gerekecek fonksiyon çalışır.
     */
    void CrashHandler::run_handlers() noexcept
    {
        for(CrashHandler* tm_handle : s_instances) {
            if( tm_handle )
                tm_handle->crashed();
        }
    }

    /**
     * @brief Signal Handler
     * 
     * Belirtilen hata sinyallerinden birisi yakalanması durumunda
     * yapılması gerekenleri yapar. İlk önce sinyal kodunu depolar
     * ve sonrasında ise hata kontrolcülerini sırayla çalıştır.
     * Bu sayede hata oluşması durumunda oluşabilecek problemleri
     * en aza indirmiş olur ya da hiç olmaz.
     * 
     * @param int Signal
     */
    void CrashHandler::signal_handler(
        int ar_signal
    ) noexcept
    {
        s_signal_code.store(ar_signal, std::memory_order_relaxed);
        run_handlers();
    }
}

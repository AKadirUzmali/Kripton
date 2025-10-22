// Abdulkadir U. - 22/10/2025
/*
    Thread Pool (İşlem Havuzu)
    
    Programımız birden çok iş yapabilir ve bunu biz birden çok
    işlemciye olabildiğince adaletli dağıtmalıyız. Bunu ise
    thread yapısı ile sağlayacağız. Bu thread yapısında işlemler
    birbirinden bağımsız olmamalı, bu yüzden bir havuz (pool)
    tasarlayacağız ve bu işlemler bu havuz içerisinde düzenli
    bir şekilde bulunacak. İstenirse eğer, işlemin kendi sonlandırması
    geldiğinde havuzdan çıkarabilir ya da el ile (manuel) olarak da
    havuzdan çıkarma sağlanıp diğer işlemlere yer açılabilir.
    İstendiği durumda havuzda bulunabilecek en fazla işlem sınırı
    uygulanabilir fakat bu sınırında performansdan kayıp yaşanmaması
    adına ortalama hesaptan biraz fazla, güvenlik problemi oluşup
    sonsuz işlem üretmeye çalışmaması adına da bir sınırı olması
    en iyi seçenek olacaktır şuan için.
*/

// C++ Zorunluluğu
#ifndef __cplusplus
    #error "[PRE ERROR] C++ Required"
#endif

// THREAD POOL.H
#pragma once

// Include:
#include <vector>
#include <queue>
#include <thread>
#include <future>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <stdexcept>

// Namespace: Core
namespace Core
{
    // Class: ThreadPool
    class ThreadPool
    {
        private:
            std::queue<std::function<void()>> tasks;        // görev kuyruğu, lambda fonksiyonlar burada saklanır
            std::vector<std::thread> workers;               // çalışan görevleri tutacak
            mutable std::mutex queue_mutex;                 // kuyruğu koruyan mutex
            std::condition_variable cond_var;               // çalışanları (worker) uyandıran koşul değişkeni
            std::atomic<bool> running;                      // havuzun (pool) aktif olup olmadığını gösterir
            std::size_t max_queue_size;                     // kuyruk kapasitesi limiti

        private:
            void workerLoop();

        public:
            explicit ThreadPool(std::size_t, std::size_t);
            ~ThreadPool();

            template <typename F, typename... Args>
            auto submit(F&&, Args&&...)
                -> std::future<typename std::invoke_result_t<F, Args...>>;

            void shutdown();

            std::size_t queueSize() const;
            std::size_t threadCount() const;

        protected:
            static const std::size_t MAX_DEFAULT_THREAD = 1024;
    };

    /**
     * @brief [Public Constructor] Thread Pool
     * 
     * Thread Pool sınıfı, belirli sayıda çalışan işleri içeren genel amaçlı
     * bir havuzdur. Görevleri sıraya alır, boşta kalan işleri dağıtır ve
     * gelecekteki sonuçlarını döndürür. C++ 17 uyumluluğuna sahiptir.
     */
    ThreadPool::ThreadPool(std::size_t _thread_count = std::thread::hardware_concurrency(),
                           std::size_t _max_queue_size = ThreadPool::MAX_DEFAULT_THREAD)
    : running(true),                    // başlangıçta havuz çalışıyor
      max_queue_size(_max_queue_size)   // görev kuyruğu kapasitesi
    {
        // donanım çekirdeği sayısı tespit edilememiş
        if( !thread_count )
            thread_count = 1;

        // çalışan işleyiciler (worker thread) oluştur
        // burada her çalışan (worker), "workerLoop" fonksiyonuna girer
        // "this" gönderilir çünkü üye fonksiyon çağırıyoruz
        for( std::size_t worker_count = 0; worker_count < thread_count; ++worker_count )
            workers.emplace_back(&ThreadPool::workerLoop, this);
    }

    /**
     * @brief [Public Destructor] Thread Pool
     * 
     * Havuz nesnesi yok edilirken otomatik olarak shutdown() çağırılır.
     * Bu, tüm çalışanların (worker) güvenli şekilde kapanmasını sağlar
     */
    ThreadPool::~ThreadPool()
    {
        shutdown();
    }

    /**
     * @brief [Public] Submit
     * 
     * Yeni bir görevi (lambda, fonksiyon, callable) havuza ekler.
     * Görev, uygun bir worker boşaldığında çalıştırılır.
     * Geriye std::future döndürür. Görev tamamlandığında sonucu
     * alabilir.
     * 
     * Şablon (template), parametre olarak herhangi bir çağırılabilir
     * nesneyi kabul eder
     * 
     * @param F
     * @param Args
     * 
     * @return std::future<typename std::invoke_result_t<F, Args...>> 
     */
    template <typename F, typename... Args>
    auto ThreadPool::submit (F&& _f, Args&&... _args)
                -> std::future<typename std::invoke_result_t<F, Args...>>
    {
        // görevin dönüş türünü belirleme
        using R = typename std::invoke_result_t<F, Args...>;

        // görevi çalıştırıp sonucu future aracılığıyla döndürecek
        auto newtask = std::make_shared<std::packaged_task<R()>>
        ( std::bind(std::forward<F>(_f), std::forward<Args>(_args)...) );

        {
            // kuyruğu korumak için kilit
            std::unique_lock<std::mutex> lock(queue_mutex);

            // eğer havuz kapalıysa yeni işlem kabul etmesin
            if( !running )
                throw std::runtime_error("[THREAD POOL] ThreadPool is shutting down");

            // görev kuyruğu doluysa yenisini oluşturmasın
            if( tasks.size() >= max_queue_size )
                throw std::runtime_error("[THREAD POOL] Task queue is full");

            // görev kuyruğuna ekle
            tasks.emplace([newtask]() { (*newtask)(); });
        }

        // kuyruğa yeni iş olduğunu çalışanlara (worker) bildiriyoruz
        cond_var.notify_one();

        // görev tamamlandığında sonucu alabilmek için future döndür
        return newtask->get_future();
    }

    /**
     * @brief [Public] Shutdown
     *
     * Thread havuzunu güvenli şekilde kapatır
     * - Yeni görev kabul edilmez
     * - Kalan görevler tamamlanır
     * - Tüm çalışanların (worker) bitmesi beklenir (join)
     */
    void ThreadPool::shutdown()
    {
        {
            // mutex kilitliyoruz çünkü artık görev almayacak
            std::lock_guard<std::mutex> lock(queue_mutex);
            running = false; 
        }

        // bekleyen tüm işleri uyandır çünkü bazıları bekleme (wait)
        // durumunda olabilir
        cond_var.notify_all();

        // çalışan (worker) işlemleri (thread) kapat
        for( auto& worker : workers ) {
            if( worker.joinable() )     // bitebilmiş durumda mı?
                worker.join();          // bitmişi sonlandır
        }

        // listeyi temizle (isteğe bağlı)
        workers.clear();
    }

    /**
     * @brief [Public] Queue Size
     * 
     * Şu anda kuyruğa kaç görev eklendiğini verir
     * 
     * @return Queue Size
     */
    std::size_t ThreadPool::queueSize() const
    {
        // kuyruğa yeni bir iş eklenmesini ya da çıkarılmasını
        // engelliyoruz (kitliyoruz) bu sayede o anki
        // işlem sayısının bilgisini güvenle alabiliyoruz
        std::lock_guard<std::mutex> lock(queue_mutex);
        return tasks.size();
    }

    /**
     * @brief [Public] Thread Count
     * 
     *  Kaç aktif çalışan (worker) olduğunun bilgisini verir
     * 
     * @return Thread Count
     */
    std::size_t ThreadPool::threadCount() const
    {
        // herhangi bir kilit mekanizmasına gerek yok
        // direk aktif çalışan sayısının döndürür
        return workers.size();
    }

    /**
     * @brief [Private] Worker Loop
     * 
     * + Her çalışan işlemin çalıştığı ana döngü
     * + Sürekli olarak görev kuyruğunu dinler
     * + Yeni görev gelirse çalıştırır, yoksa bekler
     */
    void ThreadPool::workerLoop()
    {
        while( true )
        {
            std::function<void()> job; // çalıştırılacak görev fonksiyonu

            {
                // kuyruk için kilit (iş seçimi sırasında koruma)
                std::unique_lock<std::mutex> lock(queue_mutex);

                // kuyrukta görev yoksa veya kapanıyorsa bekle
                cond_var.wait(lock, [this] {
                    // bekleme koşulu: ya görev var ya da sistem kapanıyor
                    return !running || !tasks.empty();
                });

                // havuz kapanıyor ve görev açık kalmamışsa çıksın
                if( !running && tasks.empty() )
                    return;

                // bir görev al
                job = std::move(tasks.front());
                tasks.pop();
            }

            // kilit bırakıldıktan sonra görev çalıştırılır
            // böylece diğer thread'ler kuyruğa erişebilir
            job();
        }
    }
}
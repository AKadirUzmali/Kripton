// Abdulkadir U. - 24/01/2026
#pragma once

/**
 * Thread Pool (İşlem Havuzu)
 * 
 * ThreadPool, birden çok görevi (task) paralel olarak çalıştırmak için
 * önceden oluşturulmuş bir iş parçacığı (thread) havuzu kullanır.
 * 
 * Bu yapı, her görev için yeni thread oluşturmanın maliyetini ortadan kaldırır,
 * CPU çekirdeklerinin dengeli kullanımını sağlar ve kaynak tüketimini azaltır.
 */

// Include
#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <atomic>

#include <dev/developer.hpp>
#include <core/crash.hpp>

// Namespace
namespace pool::threadpool
{
    // Using Namespace
    using namespace core::crash;

    // Class
    class ThreadPool final : public virtual CrashHandler
    {
        private:
            std::vector<std::thread> m_workers;
            std::queue<std::function<void()>> m_tasks;
            
            std::mutex m_mtx;
            std::condition_variable m_convar;

            std::atomic<bool> m_stop { false };

        public:
            ThreadPool(const ThreadPool&) = delete;
            ThreadPool& operator=(const ThreadPool&) = delete;

            explicit ThreadPool(std::size_t ar_thread_count = std::thread::hardware_concurrency());
            ~ThreadPool();

            [[nodiscard]] inline bool is_running() const noexcept;
            [[nodiscard]] inline std::size_t get_thread_count() const noexcept;

            void enqueue(std::function<void()> ar_func) noexcept;
            void stop() noexcept;

        private:
            void shutdown() noexcept;

        protected:
            virtual void crashed() noexcept override;
    };

    /**
     * @brief ThreadPool
     * 
     * Verilen çekirdek miktarı kadar döngüde döner.
     * Eğer otomatik sistem yerine elle (manuel) olarak
     * değer girilmiş ve geçersiz ise, kendisini 1 çekirdeğe ayarlar.
     * İşleyicilerin içine yenisini eklemek için kendi
     * içinde döngü başlatır ve bu döngüde ya işlem havuzunun
     * durmasını ya da işlemler (tasks) listesinin boş olmamasını kontrol eder.
     * Eğer durmuş ise ve liste boş ise sonlanır ama aksi halde
     * yeni işlemi işlemler (tasks) listesinin önünden alır ve listenin
     * ön sırasındaki işlemi listeden kaldırır. Geçici olarak tuttuğumuz
     * işlem çalıştırılarak hem işlem çalıştırılmış olur hem de çalışanlar (workers)
     * listesine eklenmiş olur
     * 
     * @param size_t Thread Count
     */
    ThreadPool::ThreadPool(std::size_t ar_thread_count)
    {
        if( ar_thread_count == 0 )
            ar_thread_count = 1;

        for(std::size_t tm_count = 0; tm_count < ar_thread_count; ++tm_count)
        {
            this->m_workers.emplace_back([this]{
                while( true )
                {
                    std::function<void()> tm_task;

                    {
                        std::unique_lock<std::mutex> tm_lock(this->m_mtx);

                        this->m_convar.wait(tm_lock, [this]{
                            return this->m_stop.load(std::memory_order_acquire)
                                || CrashHandler::is_signal()
                                || !this->m_tasks.empty();
                        });

                        if( this->m_stop.load(std::memory_order_relaxed) || CrashHandler::is_signal() )
                            return;

                        tm_task = std::move(this->m_tasks.front());
                        this->m_tasks.pop();
                    }

                    try {
                        tm_task();
                    } catch(...) {}
                }
            });
        }
    }

    /**
     * @brief ~ThreadPool
     * 
     * Sınıf sonlandırıcısı önceden oluşturulmuş
     * sonlandırma fonksiyonunu çalıştırarak yapının
     * sonlanmasını sağlar
     */
    ThreadPool::~ThreadPool()
    {
        this->shutdown();
    }

    /**
     * @brief Is Running
     * 
     * İşlem havuzunun çalışıp çalışmadığını döndürecek
     * 
     * @return bool
     */
    [[nodiscard]]
    bool ThreadPool::is_running() const noexcept
    {
        return !this->m_stop.load(std::memory_order_relaxed);
    }

    /**
     * @brief Get Thread Count
     * 
     * Tutulan işlemlerin miktarını döndürecek
     * 
     * @return size_t
     */
    [[nodiscard]]
    std::size_t ThreadPool::get_thread_count() const noexcept
    {
        return this->m_workers.size();
    }

    /**
     * @brief Enqueue
     * 
     * Yeni bir görevi havuza ekler. Görev, uygun bir worker boşaldığında çalıştırılır.
     * Eş zamanlı erişimi engellesin ve yeni bir işi kuyruğa göndersin.
     * Yeni bir iş eklendiğinde dair haberdar etsin
     * 
     * @param function<void()> Function
     */
    void ThreadPool::enqueue(
        std::function<void()> ar_func
    ) noexcept
    {
        {
            std::unique_lock<std::mutex> tm_lock(this->m_mtx);

            if( this->m_stop.load(std::memory_order_relaxed) || CrashHandler::is_signal() )
                return;

            this->m_tasks.emplace(std::move(ar_func));
        }

        this->m_convar.notify_one();
    }

    /**
     * @brief Shutdown
     * 
     * Sonlandırma yapmadan önce hala çalışır durumda olmasını kontrol eder.
     * Durmuş ise eğer, zaten sonlandırılmış demektir. Tüm durumlara
     * bildirim gönderir. Döngü ile tüm çalışanları (workers) sırayla kontrol eder.
     * Eğer sonlandırılabilecek durumda ise ve ana işlem değilse sonlandırır
     */
    void ThreadPool::shutdown() noexcept
    {
        if( this->m_stop.exchange(true, std::memory_order_acq_rel) )
            return;

        this->m_convar.notify_all();

        const std::thread::id tm_self_id = std::this_thread::get_id();

        for(auto& tm_worker : this->m_workers) {
            if( tm_worker.joinable() && tm_worker.get_id() != tm_self_id )
                tm_worker.join();
        }
    }

    /**
     * @brief Crashed
     * 
     * Beklenmedik sonlanmaların oluşmasını durumunda
     * yapıda son yapılması gerekenleri yapar.
     * Sınıfı sonlandırarak işlemlerin başarı ve güvenle
     * sonlanmasını sağlar
     */
    void ThreadPool::crashed() noexcept
    {
        this->shutdown();
    }

    /**
     * @brief Stop
     * 
     * İşlem havuzunu durdurarak sonlandırır
     */
    void ThreadPool::stop() noexcept
    {
        this->shutdown();
    }
}
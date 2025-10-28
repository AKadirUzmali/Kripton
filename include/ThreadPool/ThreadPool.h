// Abdulkadir U. - 22/10/2025
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

// Include:
#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <string>

// Namespace: Core
namespace core
{
    // Class: Thread Pool
    class ThreadPool
    {
        private:
            std::vector<std::thread> workers;
            std::queue<std::function<void()>> tasks;

            std::mutex queue_mutex;
            std::condition_variable condition;

            std::atomic<bool> running = true;

        public:
            explicit ThreadPool(size_t = std::thread::hardware_concurrency());
            ~ThreadPool();

            inline bool isRunning() const noexcept;
            inline size_t threadCount() const noexcept;

            void enqueue(std::function<void()>) noexcept;
            void shutdown() noexcept;
    };

    /**
     * @brief [Public] Constructor
     * 
     * @param size_t Eş Zamanlı İşleyici Miktarı
     */
    ThreadPool::ThreadPool(size_t _threadcount)
    {
        for( size_t counter = 0; counter < _threadcount; ++counter )
        {
            workers.emplace_back([this] {
                while( true ) {
                    std::function<void()> newtask;

                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        condition.wait(lock, [this] {
                            return !running || !tasks.empty();
                        });

                        if( !running && tasks.empty() )
                            return;

                        newtask = std::move(tasks.front());
                        tasks.pop();
                    }

                    newtask();
                }
            });
        }
    }

    /**
     * @brief [Public] Destructor
     * 
     * Sınıf sonlandırıcısı
     */
    ThreadPool::~ThreadPool()
    {
        shutdown();
    }

    /**
     * @brief [Public] Is Running
     * 
     * Çalışıp çalışmadığını kontrol etsin
     * 
     * @return Is Running?
     */
    bool ThreadPool::isRunning() const noexcept
    {
        return running;
    }

    /**
     * @brief [Public] Thread Count
     * 
     * @return Çalışan İşlem Sayısı
     */
    size_t ThreadPool::threadCount() const noexcept
    {
        // aktif çalışanların sayısı
        return static_cast<size_t>(workers.size());
    }

    /** @brief [Public] Enqueue
     * 
     * Yeni bir görevi havuza ekler.
     * Görev, uygun bir worker boşaldığında çalıştırılır.
     */
    void ThreadPool::enqueue(std::function<void()> _newtask) noexcept
    {
        {
            // eş zamanlı erişimi engellesin ve
            // yeni bir işi kuyruğa göndersin
            std::unique_lock<std::mutex> lock(queue_mutex);
            tasks.emplace(std::move(_newtask));
        }

        // yeni bir iş eklendiğinde dair haberdar etsin
        condition.notify_one();
    }

    /**
     * @brief [Public] Shutdown
     *
     * Thread havuzunu güvenli şekilde kapatır
     * - Yeni görev kabul edilmez
     * - Kalan görevler tamamlanır
     * - Tüm çalışanların (worker) bitmesi beklenir (join)
     */
    void ThreadPool::shutdown() noexcept
    {
        // eş zamanlı erişimi durdurup
        // çalışıyor durumunu sonlandırsın
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            running = false;
        }

        // tüm thread yapısını uyandırsın
        condition.notify_all();

        // tüm çalışan işlemleri döngüye alsın
        // bitmiş olanları sonlandırsın
        for( auto& worker : workers )
            if( worker.joinable() )
                worker.join();
    }
}
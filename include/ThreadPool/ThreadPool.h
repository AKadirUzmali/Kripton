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

// Namespace: Core
namespace core
{
    // Class: Thread Pool
    class ThreadPool final
    {
        private:
            std::vector<std::thread> workers;
            std::queue<std::function<void()>> tasks;

            std::mutex queue_mutex;
            std::condition_variable condition;

            std::atomic<bool> stop;

        public:
            explicit ThreadPool(size_t = std::thread::hardware_concurrency());
            ~ThreadPool();

            inline bool isRunning() const noexcept;
            inline size_t threadCount() const noexcept;

            void enqueue(std::function<void()>) noexcept;
    };

    /**
     * @brief [Public] Constructor
     * 
     * İzlek havuzu ile aynı anda fazla işlem
     * yapabilmemizi sağlayacak olan sınıf oluşturucusu
     * 
     * @param size_t Thread Count
     */
    ThreadPool::ThreadPool(size_t _threadcount)
    : stop(false)
    {
        for( size_t counter = 0; counter < _threadcount; ++counter )
        {
            workers.emplace_back([this]{
                while( !this->stop )
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock, [this]{ return stop || !this->tasks.empty(); });

                        if( this->stop && this->tasks.empty() )
                            return;

                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }

                    task();
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
        {
            std::unique_lock<std::mutex> lock(this->queue_mutex);
            this->stop = true;
        }

        this->condition.notify_all();

        for( auto& worker : this->workers ) {
            if( worker.joinable() )
                worker.join();
        }
    }

    /**
     * @brief [Public] Is Running
     * 
     * Çalışıp çalışmadığını kontrol etsin
     * 
     * @return bool
     */
    bool ThreadPool::isRunning() const noexcept
    {
        return !this->stop;
    }

    /**
     * @brief [Public] Thread Count
     * 
     * @return size_t
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
}
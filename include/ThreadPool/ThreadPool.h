// Abdulkadir U. - 22/10/2025
#pragma once

// C++
#ifndef __cplusplus
    #error "[PRE ERROR] C++ Required"
#endif

/*
 * Thread Pool (İşlem Havuzu)
 * 
 * ThreadPool, birden çok görevi (task) paralel olarak çalıştırmak için
 * önceden oluşturulmuş bir iş parçacığı (thread) havuzu kullanır.
 * 
 * Bu yapı, her görev için yeni thread oluşturmanın maliyetini ortadan kaldırır,
 * CPU çekirdeklerinin dengeli kullanımını sağlar ve kaynak tüketimini azaltır.
 */

// Include:
#include <cstdint>
#include <vector>
#include <queue>
#include <thread>
#include <future>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <stdexcept>
#include <exception>
#include <string>

// Namespace: Core
namespace core
{
    // Class: Thread Pool Stop Error
    class threadpool_stop_error : public std::exception
    {
        std::string msg;

        public:
            explicit threadpool_stop_error(const std::string& message = "[THREADPOOL STOP ERROR] Thread Pool is not running")
            : msg(message) {}

            const char* what() const noexcept override {
                return msg.c_str();
            }
    };

    // Class: Thread Pool Full Error
    class threadpool_full_error : public std::exception
    {
        std::string msg;

        public:
            explicit threadpool_full_error(const std::string& message = "[THREADPOOL FULL ERROR] Thread Pool is full")
            : msg(message) {}

            const char* what() const noexcept override {
                return msg.c_str();
            }
    };
    
    // Type
    using sizetype = std::uint16_t;

    // Class: Thread Pool
    class ThreadPool
    {
        protected:
            static constexpr sizetype MIN_TASK_SIZE = 1;
            static constexpr sizetype MAX_TASK_SIZE = (sizetype)~0;
            static constexpr sizetype DEF_TASK_SIZE = 1024;

        private:
            std::vector<std::thread> workers;
            std::queue<std::function<void()>> tasks;

            mutable std::mutex queue_mutex;
            std::condition_variable condition;

            std::atomic<bool> running = true;

            sizetype max_tasks = DEF_TASK_SIZE;

        public:
            explicit ThreadPool(sizetype = static_cast<sizetype>(std::thread::hardware_concurrency()), sizetype = static_cast<sizetype>(DEF_TASK_SIZE));
            ~ThreadPool();

            sizetype task_size() const noexcept;
            sizetype thread_count() const noexcept;

            template<typename Function, typename... Args>
            auto add(Function&&, Args&&... args)
                -> std::future<typename std::invoke_result_t<Function, Args...>>;

            void shutdown() noexcept;
    };

    /**
     * @brief [Public] Constructor
     * 
     * @param uint16_t Eş Zamanlı İşlem Miktarı
     * @param uint16_t Toplam İşlem Miktarı
     */
    ThreadPool::ThreadPool(sizetype _threadcount, sizetype _max_task_limit)
    : max_tasks(_max_task_limit)
    {
        // hiç thread yoksa 1 yapsın çünkü
        // işlemci zaten en az 1 çekirdek içerir
        // ya da en üst limiti aşmısa, en üst limite ayarlasın
        if( _threadcount <= (MIN_TASK_SIZE - 1) ) _threadcount = 1;
        else if( _threadcount > MAX_TASK_SIZE ) _threadcount = MAX_TASK_SIZE;

        for( decltype(_threadcount) counter = 0; counter < _threadcount; ++counter )
        {
            workers.emplace_back([this] {
                while( running ) {
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
     * @brief [Public] Task Size
     * 
     * @return İşlem Miktarı
     */
    sizetype ThreadPool::task_size() const noexcept
    {
        // kilit koruması ile eş zamanlı olarak
        // erişmeyi engelliyoruz sonrasında ise
        // işlemlerin boyutunu döndürüyoruz
        std::lock_guard<std::mutex> lock(queue_mutex);
        return static_cast<sizetype>(tasks.size());
    }

    /**
     * @brief [Public] Thread Count
     * 
     * @return Çalışan İşlem Sayısı
     */
    sizetype ThreadPool::thread_count() const noexcept
    {
        // aktif çalışanların sayısı
        return static_cast<sizetype>(workers.size());
    }

    /** @brief [Public] Add
     * 
     * Yeni bir görevi (lambda, fonksiyon, callable) havuza ekler.
     * Görev, uygun bir worker boşaldığında çalıştırılır.
     * Geriye std::future döndürür. Görev tamamlandığında sonucu
     * alabilir.
     * 
     * Şablon (template), parametre olarak herhangi bir çağırılabilir
     * nesneyi kabul eder
     * 
     * @tparam Function 
     * @tparam Args 
     * @return Function, Lambda, Callable Type
     */
    template<typename Function, typename... Args>
    auto ThreadPool::add(Function&& _function, Args&&... _args)
        -> std::future<typename std::invoke_result_t<Function, Args...>>
    {
        // işlem bittiğinde dönecek veri türü tipi kısaltması
        using ReturnType = typename std::invoke_result_t<Function, Args...>;

        // yeni işlemi oluştursun fakat paylaşımlı şekilde oluştursun
        // oluşturulup çalıştırması beklenecek paket de ise tipini
        // başta belirttiğimiz dönüş tipinde yapsın, işlem içinde kullanılacak
        // fonksiyon direk verdiğimiz fonksiyon olsun ve
        // kullanılacak argümanlar ise verdiğimiz argümanlar olsun ki argüman
        // olmayabilir de, önemli değil. Bind kullandık çünkü verileri kopyalayıp
        // performans kaybı yaşatmaması adına
        auto newtask = std::make_shared<std::packaged_task<ReturnType()>>(            
            std::bind(std::forward<Function>(_function), std::forward<Args>(_args)...)
        );

        // işlemin gelecekteki halini saklasın
        // vakti geldiğinde buraya gelecek zaten
        std::future<ReturnType> future = newtask->get_future();

        {
            // eş zamanlı erişimi durdursun
            std::unique_lock<std::mutex> lock(queue_mutex);

            // işleyici havuzu durdurulmuş ise ona göre hata fırlatsın
            if( !running )
                throw core::threadpool_stop_error();

            // en fazla işlem oluşturma limiti aşıldı
            if( tasks.size() >= max_tasks )
                throw core::threadpool_full_error();

            // işlemlerin arasına yenisini eklesin ve çalıştırsın
            tasks.emplace([newtask]() { (*newtask)(); });
        }

        // yeni bir işlem olduğuna dair yeni bir bildiri yapsın
        condition.notify_one();

        // gelecekteki durumuna eriştik, şimdi bunu döndürsün
        return future;
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
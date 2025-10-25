// Abdulkadir U. - 24/10/2025

/**
 * Thread Pool Run Test
 * 
 * İşlem havuzunun çalışıp çalışmadığını test edeceğiz
*/

// Include:
#include <Test/Test.h>
#include <Platform/Platform.h>
#include <ThreadPool/ThreadPool.h>

#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <mutex>
#include <sstream>

// main
int main(void)
{
    // platform kontrolü
    if( !test::expect_eq(platform::is_platform(), true, "Operating System Supporting") )
        return EXIT_FAILURE;

    // thread pool oluştur
    core::ThreadPool tpool(4);

    // thread pool çalışıyor mu kontrol et
    if( !test::expect_eq(tpool.isRunning(), true, "Thread Pool is running") )
        return EXIT_FAILURE;

    // işlemleri sıraya ekleme
    std::mutex cout_mutex;

    for( size_t counter = 0; counter < 16; ++counter ) {
        tpool.enqueue([counter, &cout_mutex]{
            std::lock_guard<std::mutex> lock(cout_mutex);                   // aynı anda çıktı vermeyi engellesin

            std::stringstream ss;                                           // okunabilir çıktı için string stream
            ss << "Task " << std::setw(2) << std::right << counter          // işlem hakkında bilgiler
                << " is running on thread " << std::this_thread::get_id();
            test::expect_eq(true, true, ss.str());                          // test sonucu çıktısı
            std::this_thread::sleep_for(std::chrono::milliseconds(100));    // geçici bekleme süresi
        });
    }

    // thread pool sonlandırma
    tpool.shutdown();
    test::expect_eq(tpool.isRunning(), false, "Thread Pool Shutdowned");
    
    return 0;
}
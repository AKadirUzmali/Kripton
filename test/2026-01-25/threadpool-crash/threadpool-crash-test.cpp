// Abdulkadir U. - 2026/01/25

/**
 * ThreadPool Crash Test (İşlem Havuzu Çökme Testi)
 * 
 * Program sırasında bir çok bağımsız işlem yapılmaktadır.
 * Bu işlemlerin tek çekirdekte toplanmadan dağıtılmış ve
 * performanslı çalışabilmesi için ThreadPool (İşlem Havuzu)
 * yapısında ihtiyaç vardır fakat bu yapının da güvenliğe ihtiyacı
 * vardır. Program da aniden oluşabilecek istenmeyen durumlarda
 * programı güvenle sonlandırabilecek bir yapı gereklidir.
 * Bu yapıyı CrashHandler (Çökme Tutucu) yapısı ile yaptık.
 * ThreadPool yapısı da bu sınıfı içerdiği için çökme durumlarında
 * ne yapacağını görmek amaçlı bunu test ediyoruz
 * 
 * Derleme:
 *  Bsd     :: g++ -I../../../include -std=c++17 -Wall -Werror -Wextra threadpool-crash-test.cpp -pthread -o bsd/threadpool-crash-test.bsd
 *  Linux   :: g++ -I../../../include -std=c++17 -Wall -Werror -Wextra threadpool-crash-test.cpp -o linux/threadpool-crash-test.linux
 *  Windows :: g++ -I../../../include -std=c++17 -Wall -Werror -Wextra threadpool-crash-test.cpp -o windows/threadpool-crash-test.exe
 */

// Include
#define __BUILD_TYPE__ build_t::Debug
#include <core/buildtype.hpp>
#include <dev/developer.hpp>
#include <pool/threadpool.hpp>

#include <kits/toolkit.hpp>
#include <kits/outputkit.hpp>
#include <kits/hashkit.hpp>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>

// Using Namespace
using namespace core::platform;
using namespace core::buildtype;
using namespace core::version;
using namespace dev::log;
using namespace dev::output::console;
using namespace dev::output::file;
using namespace dev::trace;
using namespace tools::charset;
using namespace tools::hash::vch;
using namespace tools::time;
using namespace pool::threadpool;

// Static
static inline constexpr Version ss_ver(0, 5, 2);
static inline constexpr Vch ss_verhash("20260125|test|threadpool-crash|debug|patch|last-test-and-patch|windows-patch", 202601252119);

static std::string ss_filename = "2026-01-25-threadpool-crash-" + utf::to_lower(std::string(current_os_name()));
static Logger<FileOut, ConsoleOut> ss_testlog("logs/" + ss_filename, ss_filename + "-console");
static ThreadPool ss_tpool;

// main
int main(void)
{
    Scope<Logger<FileOut, ConsoleOut>> tm_trace(ss_testlog, "main", GET_SOURCE);

    ss_testlog.write(level_t::Info, ss_verhash.c_str(), GET_SOURCE);
    ss_testlog.write(level_t::Debug, ss_buildtype.c_str(), GET_SOURCE);
    ss_testlog.write(level_t::Info, "Thread Count: " + std::to_string(ss_tpool.get_thread_count()), GET_SOURCE);

    for(uint16_t tm_count = 0; tm_count < 16; ++tm_count)
    {
        ss_tpool.enqueue([tm_count]{
            std::stringstream tm_strstream;
            tm_strstream << "Task " << std::setw(2) << std::right << tm_count
                << " is running on thread " << std::this_thread::get_id();

            ss_testlog.write(level_t::Warn, tm_strstream.str(), GET_SOURCE);

            while(!ss_testlog.is_signal()) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        });
    }
}
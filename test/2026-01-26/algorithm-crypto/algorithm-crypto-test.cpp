// Abdulkadir U. - 2026/01/26

/**
 * Algorithm Crypto Test (Algoritma Kripto Test)
 * 
 * Algoritma sınıfını baz olan kriptografik şifreleme/çözme
 * sınıflarımızı genel olarak daha rahat kullanmak adına geliştirilmiş
 * olan Crypto sınıfı sayesinde kriptografik işlemleri daha modern ve
 * rahat şekilde yapabileceğiz
 * 
 * Derleme:
 *  Bsd     :: g++ -I../../../include -std=c++17 -Wall -Werror -Wextra algorithm-crypto-test.cpp -pthread -o bsd/algorithm-crypto-test.bsd
 *  Linux   :: g++ -I../../../include -std=c++17 -Wall -Werror -Wextra algorithm-crypto-test.cpp -o linux/algorithm-crypto-test.linux
 *  Windows :: g++ -I../../../include -std=c++17 -Wall -Werror -Wextra algorithm-crypto-test.cpp -o windows/algorithm-crypto-test.exe
 */

// Include
#define __BUILD_TYPE__ build_t::Debug
#include <core/buildtype.hpp>

#include <dev/developer.hpp>

#include <pool/threadpool.hpp>
#include <pool/cryptopool.hpp>

#include <kits/toolkit.hpp>
#include <kits/outputkit.hpp>
#include <kits/hashkit.hpp>

// Using Namespace
using namespace core::platform;
using namespace core::buildtype;
using namespace core::version;

using namespace dev::output::file;
using namespace dev::output::console;
using namespace dev::log;
using namespace dev::trace;

using namespace tools::charset;
using namespace tools::hash::vch;
using namespace tools::time;

using namespace pool::threadpool;
using namespace pool::cryptopool;

// Static
static inline constexpr Version ss_ver(0, 6, 2);
static inline constexpr Vch ss_verhash("20260126|test|algorithm-crypto|threadpool|patch-all|console", 202601262319 + 01270151); // 2026-01-26 23:19 / 2026-01-27 01:51

static std::string ss_filename = "2026-01-26-algorithm-crypto-" + utf::to_lower(std::string(current_os_name()));
static Logger<FileOut, ConsoleOut> ss_testlog("logs/" + ss_filename, ss_filename + "-console");
static ThreadPool ss_tpool;
static Crypto<Xor> ss_cipher("test-cipher", U"key-2026-cipher-0126");

/**
 * @brief Thread Cipher
 * 
 * Fonksiyonun genel amacı kriptoloji ile
 * çoklu işlemin birleşimini göstermek.
 * Verilen metin diğer işlem izleklerine de
 * verileceği için ve thread-safe bile bile yapılmadığı
 * için, tahminen metin son olarak saçmalamış olacak ama
 * test etmek için değer :)
 * 
 * Not: Test sonrası metin orijinali gibi duruyor.
 *      Kendi yazdığım kodumu tebrik ediyorum, beni g*t etti xD
 *      Saatlerce durmadan çalışınca haliyle kayış kopuyo :)
 * 
 * Not 2: Fonksiyon performans açısından berbat evet ama amaç test olduğu
 * için herhangi bir şekilde performans üzerine durulmadı. Tek amaç, daha
 * iyi bir çıktı sağlamaktı.
 * 
 * @param uint16_t Count
 * @tparam Crypto<Algo>& Crypto
 * @tparam Logger<Args...>& Logger
 * @param string& Text
 */
template<typename Algo, class... Args>
void thread_cipher(
    const uint16_t ar_count,
    Crypto<Algo>& ar_crypto,
    Logger<Args...>& ar_logger,
    std::string& ar_text
) noexcept
{
    std::string tm_str("thread-cipher-" + std::to_string(ar_count));
    Scope<Logger<Args...>, std::chrono::microseconds> tm_trace(ar_logger, tm_str.c_str(), GET_SOURCE);

    std::stringstream tm_sstream;
    tm_sstream << "Task " << std::setw(2) << std::right << ar_count
        << " is running on thread " << std::this_thread::get_id();
    ar_logger.write(level_t::Debug, tm_sstream.str(), GET_SOURCE);

    std::stringstream tm_cryptostream;
    tm_cryptostream << "Task " << std::setw(2) << std::right << ar_count
        << " - Before Encrypt: " << ar_text;

    ar_crypto.encrypt(ar_text);
        tm_cryptostream << " | After Encrypt: " << ar_text;

    ar_crypto.decrypt(ar_text);
        tm_cryptostream << " | After Decrypt: " << ar_text;

    ar_logger.write(level_t::Info, tm_cryptostream.str(), GET_SOURCE);
}

// main
int main(void)
{
    tools::console::enable_utf8_console();

    ss_testlog.write(level_t::Info, ss_verhash.c_str(), GET_SOURCE);
    ss_testlog.write(level_t::Debug, ss_buildtype.c_str(), GET_SOURCE);
    ss_testlog.write(level_t::Info, "Thread Count: " + std::to_string(ss_tpool.get_thread_count()), GET_SOURCE);
    ss_testlog.write(level_t::Info, "Algorithm Name: " + ss_cipher.algorithm().get_name(), GET_SOURCE);
    ss_testlog.write(level_t::Info, "Algorithm Key: " + ss_cipher.algorithm().get_key(), GET_SOURCE);

    std::string tm_u32str = utf::to_utf8(U"Hello 😁, It's Crypto 😌");
    std::string tm_str_for_thread = tm_u32str;

    for(uint16_t tm_count = 0; tm_count < 16; ++tm_count)
    {
        ss_tpool.enqueue([tm_count, &tm_str_for_thread]{
            thread_cipher(tm_count, ss_cipher, ss_testlog, tm_str_for_thread);
        });
    }

    ss_testlog.write(level_t::Info, "Original Text: " + tm_u32str, GET_SOURCE);

    ss_cipher.encrypt(tm_u32str);
    ss_testlog.write(level_t::Info, "Original Text After Encrypt: " + tm_u32str, GET_SOURCE);

    ss_cipher.decrypt(tm_u32str);
    ss_testlog.write(level_t::Info, "Original Text After Decrypt: " + tm_u32str, GET_SOURCE);

    while( !ss_testlog.is_signal() ) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    return EXIT_FAILURE;
}
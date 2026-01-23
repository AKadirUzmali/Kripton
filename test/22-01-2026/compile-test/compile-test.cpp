// Abdulkadir U. - 22/01/2026

/**
 * Compile Test (Derleme Testi)
 * 
 * Projedeki tüm başlık dosyalarını dahil ederek
 * derleme testi yapıyoruz ve bu sayede syntax
 * hatası olup olmadığını rahatça tespit edebileceğiz
 * ama aksi halde sonra da kullanabilmek için precompiled
 * cpp dosyası derlemesi yaparak da yapabiliriz ama her değişimde
 * her işletim sistemi için tekrar tekrar precompile dosyası
 * derlemeliyiz. Bazı dosyalar tekrar tekrar #include olacak
 * fakat pragma once ile derleyici bazında tekrar tekrar #include
 * etmesini önlemiş oluyoruz ve herhangi bir hata ya da dosya
 * boyutu şişmesi yapmamış oluyoruz.
 * 
 * Ek olarak da diğer testleri de yaparak basitçe test etmiş oluyoruz.
 * Test fonksiyonlarını da kullanıyoruz.
 * 
 * Derleme: g++ -I../../../include -Wall -Werror -Wextra compile-test.cpp -o <os>/compile-test
 */

// Define
#define __DEVELOPER__ 1

// Include
#include <core/platform.hpp>
#include <core/flag.hpp>
#include <core/status.hpp>
#include <core/version.hpp>

#define __BUILD_TYPE__ build_t::Debug
#include <core/buildtype.hpp>

#include <tools/charset/utf.hpp>
#include <tools/console/color.hpp>
#include <tools/hash/vch.hpp>
#include <tools/time/time.hpp>

#include <kits/toolkit.hpp>
#include <kits/outputkit.hpp>
#include <kits/hashkit.hpp>

#include <dev/config/build.hpp>
#include <dev/config/config.hpp>
#include <dev/core/source.hpp>
#include <dev/test/expect.hpp>
#include <dev/test/assert.hpp>
#include <dev/trace/timeline.hpp>
#include <dev/trace/trace.hpp>
#include <dev/log/levels.hpp>
#include <dev/log/output.hpp>
#include <dev/log/output/consoleout.hpp>
#include <dev/log/output/fileout.hpp>
#include <dev/log/macros.hpp>
#include <dev/log/logger.hpp>

// Using Namespace
using namespace core::platform;
using namespace core::buildtype;
using namespace core::version;
using namespace dev::log;
using namespace dev::output::console;
using namespace dev::output::file;
using namespace tools::charset;
using namespace tools::hash::vch;

// Static
static inline constexpr Version ver(0, 5, 0);
static inline constexpr Vch verhash("compile-test|main|log|developer|buildtype|version=debug|last-test", 23012026);
static std::string filename = "18-01-2026-output-version-" + utf::to_lower(std::string(current_os_name()));
static Logger<FileOut, ConsoleOut> testlog("logs/" + filename, filename + "-console");

// main
int main(void)
{
    TRACE_SCOPE(testlog, "main");
    LOG(testlog, verhash.c_str());
    LOG(testlog, ss_buildtype.c_str());

    return 0;
}
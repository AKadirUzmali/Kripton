// Abdulkadir U. - 18/01/2026

/**
 * Output Version
 * 
 * Program oluşturulurken bir sürüm kodu verilir.
 * Bu kodu çıktı kısmında da görebilmek için 
 */

// Include:
#define __DEVEL__
#include <developer/Developer.h>

// Using Namespace:
using namespace dev::level;
using namespace dev::log;
using namespace dev::output::file;
using namespace dev::output::console;
using namespace dev::version;

// Static:
static Version app(0, 0, 0, build_t::Debug);
static std::string filename = "18-01-2026-output-version-" + tools::utf::to_lower(std::string(tools::os::current_os_name()));
static Logger<FileOut, ConsoleOut> testlog("logs/" + filename, filename + "-console");

// main
int main(void)
{
    TRACE_SCOPE(testlog, "main");
    LOG(testlog, "Test version: " + app.strVersion());
    LOG(testlog, "Test build type: " + app.strBuildType());

    return 0;
}
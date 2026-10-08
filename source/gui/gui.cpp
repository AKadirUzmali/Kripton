// Abdulkadir U. - 2026/10/01

/**
 * @brief GUI
 * 
 * Derleme:
 *  Linux       (G++): g++ -std=c++17 -Wall -Wextra -Iinclude/ source/gui/gui.cpp -o build/app-gui.linux `wx-config --cxxflags --libs`
 *  Windows (Mingw64): g++ -std=c++17 -Iinclude/ source/gui/gui.cpp `wx-config --cxxflags --libs` -Wall -Werror -Wextra -o build/app-gui.exe
 */

// Define
#define __BUILD_DEBUG__
#define __SIGNAL_DETECT__ SigInterrupt | SigAbort | SigIllegal | SigSegFault | SigTerminate

// Include
#include <gui/gui.hpp>

// Using Namespace
using namespace gui;

/**
 * @brief On Init
 * 
 * Gui için ana pencereyi oluşturur ve başlatır.
 * Başlatmadan önce pencereyi ortalar ve boyutunu ayarlar.
 * 
 * @return bool
 */
bool App::OnInit()
{
    frame::MainFrame* tm_frame = new frame::MainFrame("Kripton");
    
    wxSize tm_fixed_size(800, 600);
    tm_frame->SetClientSize(tm_fixed_size);

    tm_frame->Center();
    tm_frame->Show(true);

    return true;
}

// App
wxIMPLEMENT_APP(gui::App);
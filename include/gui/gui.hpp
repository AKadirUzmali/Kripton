// Abdulkadir U. - 2026/10/01
#pragma once

// Include
#include <gui/frame/mainframe.hpp>

// Namespace
namespace gui
{
    // Using Namespace
    using namespace frame;

    // Class
    class App : public wxApp
    {
        public:
            virtual bool OnInit();
    };

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
        MainFrame* tm_frame = new MainFrame("Kripton");
        
        wxSize tm_fixed_size(800, 600);
        tm_frame->SetClientSize(tm_fixed_size);

        tm_frame->Center();
        tm_frame->Show(true);   

        return true;
    }
}
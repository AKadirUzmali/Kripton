// Abdulkadir U. - 2026/10/01
#pragma once

// Include
#include <wx/wx.h>
#include <gui/frame/mainframe.hpp>

// Namespace
namespace gui
{
    // Class
    class App : public wxApp
    {
        public:
            virtual bool OnInit();
    };
}
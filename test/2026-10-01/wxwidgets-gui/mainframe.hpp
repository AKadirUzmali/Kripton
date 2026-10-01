// Abdulkadir U. - 2026/10/01
#pragma once

// Include
#include <wx-3.2/wx/wx.h>
#include <wx-3.2/wx/spinctrl.h>

/*

// Class
class MainFrame : public wxFrame
{
    public:
        MainFrame(const wxString& ar_title);

    private:
        void OnButtonClicked(wxCommandEvent& ar_event);
        void OnSliderChanged(wxCommandEvent& ar_event);
        void OnTextChanged(wxCommandEvent& ar_event);

        wxDECLARE_EVENT_TABLE();
};

// Enum
enum IDs
{
    BUTTON_ID = 2,
    SLIDER_ID = 3,
    TEXT_ID = 4
};

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_BUTTON(BUTTON_ID, MainFrame::OnButtonClicked)
    EVT_SLIDER(SLIDER_ID, MainFrame::OnSliderChanged)
    EVT_TEXT(TEXT_ID, MainFrame::OnTextChanged)
wxEND_EVENT_TABLE()
*/

/*
// Class
class MainFrame : public wxFrame
{
    public:
        MainFrame(const wxString& ar_title);

    private:
        void OnAnyButtonClicked(wxCommandEvent& ar_event);
        void OnButton1Clicked(wxCommandEvent& ar_event);
        void OnButton2Clicked(wxCommandEvent& ar_event);
        void OnClose(wxCloseEvent& ar_event);
};
*/

/*
// Class
class MainFrame : public wxFrame
{
    public:
        MainFrame(const wxString& ar_title);

    private:
        void OnMouseEvent(wxMouseEvent& ar_event);
};
*/

/*
// Class
class MainFrame : public wxFrame
{
    public:
        MainFrame(const wxString& ar_title);

    private:
        void OnKeyEvent(wxKeyEvent& ar_event);
};
*/

// Class
class MainFrame : public wxFrame
{
    public:
        MainFrame(const wxString& ar_title);
};

/**
 * @brief Main Frame
 */
MainFrame::MainFrame(const wxString& ar_title)
    : wxFrame(nullptr, wxID_ANY, ar_title)
{
    /*
    // Main Panel
    wxPanel* tm_pnl_main = new wxPanel(this);

    // Button
    wxButton* tm_btn = new wxButton(this, wxID_ANY, "Button", wxPoint(150, 50), wxSize(100, 35));

    // Checkbox
    wxCheckBox* tm_chkbox = new wxCheckBox(this, wxID_ANY, "Checkbox", wxPoint(150, 100),
        wxDefaultSize, wxCHK_3STATE | wxCHK_ALLOW_3RD_STATE_FOR_USER);

    // Static Text
    wxStaticText* tm_statictext = new wxStaticText(this, wxID_ANY, "Static Text:", wxPoint(50, 150), wxSize(100, 25),
        wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
    tm_statictext->SetBackgroundColour(wxColour(217, 255, 0));
    tm_statictext->SetForegroundColour(wxColour(18, 18, 18));

    // Editable Text
    wxTextCtrl* tm_txtctrl = new wxTextCtrl(this, wxID_ANY, "", wxPoint(150, 150), wxSize(200, 25),
        wxTE_PASSWORD);

    // Chose List
    wxArrayString tm_arrstr_list;
    tm_arrstr_list.Add("Item 1");
    tm_arrstr_list.Add("Item 2");
    tm_arrstr_list.Add("Item 3");

    wxChoice* tm_choice = new wxChoice(this, wxID_ANY, wxPoint(150, 375), wxSize(100, -1), tm_arrstr_list);
    tm_choice->SetSelection(0); // default selection

    // List Box
    wxListBox* tm_listbox = new wxListBox(this, wxID_ANY, wxPoint(300, 375), wxSize(100, 100), tm_arrstr_list,
        wxLB_MULTIPLE);

    // Radio Box
    wxRadioBox* tm_radiobox = new wxRadioBox(this, wxID_ANY, "Radio Box", wxPoint(0, 200), wxSize(200, 100),
        tm_arrstr_list, 3, wxRA_SPECIFY_ROWS);

    // Slider
    wxSlider* tm_slider = new wxSlider(this, wxID_ANY, 25, 0, 100, wxPoint(100, 500), wxSize(200, 50),
        wxSL_VALUE_LABEL);

    // Gauge
    wxGauge* tm_gauge = new wxGauge(this, wxID_ANY, 100, wxPoint(300, 300), wxSize(200, 25),
        wxGA_HORIZONTAL | wxGA_SMOOTH);
    tm_gauge->SetValue(50);
    */

    /*
    // Main Panel
    wxPanel* tm_pnl_main = new wxPanel(this);

    // Button
    wxButton* tm_btn = new wxButton(tm_pnl_main, BUTTON_ID, "Button", wxPoint(350, 280), wxSize(100, 40));

    // Slider
    wxSlider* tm_slider = new wxSlider(tm_pnl_main, SLIDER_ID, 0, 0, 100, wxPoint(300, 350), wxSize(200, 40));

    // Text Control
    wxTextCtrl* tm_txtctrl = new wxTextCtrl(tm_pnl_main, TEXT_ID, "", wxPoint(300, 420), wxSize(200, 40));

    // Status Bar
    CreateStatusBar();
    */

    /*
    // Main Panel
    wxPanel* tm_pnl_main = new wxPanel(this);

    // Button
    wxButton* tm_btn = new wxButton(tm_pnl_main, wxID_ANY, "Button", wxPoint(350, 280), wxSize(100, 40));

    // Slider
    wxSlider* tm_slider = new wxSlider(tm_pnl_main, wxID_ANY, 0, 0, 100, wxPoint(300, 350), wxSize(200, 40));

    // Text Control
    wxTextCtrl* tm_txtctrl = new wxTextCtrl(tm_pnl_main, wxID_ANY, "", wxPoint(300, 420), wxSize(200, 40));

    // Bind
    tm_btn->Bind(wxEVT_BUTTON, &MainFrame::OnButtonClicked, this);
    tm_slider->Bind(wxEVT_SLIDER, &MainFrame::OnSliderChanged, this);
    tm_txtctrl->Bind(wxEVT_TEXT, &MainFrame::OnTextChanged, this);

    // Unbind
    tm_btn->Unbind(wxEVT_BUTTON, &MainFrame::OnButtonClicked, this);

    // Status Bar
    CreateStatusBar();
    */

    /*
    // Panel
    wxPanel* tm_pnl_main = new wxPanel(this);

    // Buttons
    wxButton* tm_btn1 = new wxButton(tm_pnl_main, wxID_ANY, "Button 1", wxPoint(300, 250), wxSize(100, 40));
    wxButton* tm_btn2 = new wxButton(tm_pnl_main, wxID_ANY, "Button 2", wxPoint(300, 300), wxSize(100, 40));
    
    // Bind
    this->Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnClose, this);
    this->Bind(wxEVT_BUTTON, &MainFrame::OnAnyButtonClicked, this);

    // Bind Buttons
    tm_btn1->Bind(wxEVT_BUTTON, &MainFrame::OnButton1Clicked, this);
    tm_btn2->Bind(wxEVT_BUTTON, &MainFrame::OnButton2Clicked, this);

    // Status Bar
    CreateStatusBar();
    */

    /*
    // Panel
    wxPanel* tm_pnl_main = new wxPanel(this);

    // Button
    wxButton* tm_btn = new wxButton(tm_pnl_main, wxID_ANY, "Button", wxPoint(350, 280), wxSize(100, 40));

    // Status Bar
    wxStatusBar* tm_statusbar = CreateStatusBar();
    tm_statusbar->SetDoubleBuffered(true);

    // Mouse Event
    tm_pnl_main->Bind(wxEVT_MOTION, &MainFrame::OnMouseEvent, this);
    tm_btn->Bind(wxEVT_MOTION, &MainFrame::OnMouseEvent, this);
    */

    /*
    // Panel
    wxPanel* tm_pnl_main = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS);

    // Button
    wxButton* tm_btn1 = new wxButton(tm_pnl_main, wxID_ANY, "Button 1", wxPoint(300, 250), wxSize(100, 40), wxWANTS_CHARS);
    wxButton* tm_btn2 = new wxButton(tm_pnl_main, wxID_ANY, "Button 2", wxPoint(300, 300), wxSize(100, 40));

    tm_pnl_main->Bind(wxEVT_CHAR_HOOK, &MainFrame::OnKeyEvent, this);

    // Status Bar
    CreateStatusBar();
    */
}

/*
void MainFrame::OnButtonClicked(wxCommandEvent& ar_event)
{
    wxLogStatus("Button Clicked!");
}

void MainFrame::OnSliderChanged(wxCommandEvent& ar_event)
{
    wxString tm_str = wxString::Format("Slider Value: %d", ar_event.GetInt());
    wxLogStatus(tm_str);
}

void MainFrame::OnTextChanged(wxCommandEvent& ar_event)
{
    wxString tm_str = wxString::Format("Text: %s", ar_event.GetString());
    wxLogStatus(tm_str);
}
*/

/*
void MainFrame::OnClose(wxCloseEvent& ar_event)
{
    wxLogMessage("Frame Closed");
    ar_event.Skip();
}

void MainFrame::OnAnyButtonClicked(wxCommandEvent& ar_event)
{
    wxLogMessage("Button Clicked!");
}

void MainFrame::OnButton1Clicked(wxCommandEvent& ar_event)
{
    wxLogStatus("Button 1 Clicked!");
    ar_event.Skip();
}

void MainFrame::OnButton2Clicked(wxCommandEvent& ar_event)
{
    wxLogStatus("Button 2 Clicked!");
    ar_event.Skip();
}
*/

/*
void MainFrame::OnMouseEvent(wxMouseEvent& ar_event)
{
    wxPoint tm_pos = wxGetMousePosition();
    tm_pos = this->ScreenToClient(tm_pos);
    wxString tm_str = wxString::Format("Mouse Position: (x: %d, y: %d)", tm_pos.x, tm_pos.y);
    wxLogStatus(tm_str);
}
*/

/*
void MainFrame::OnKeyEvent(wxKeyEvent& ar_event)
{
    if( ar_event.GetKeyCode() == WXK_TAB )
    {
        wxWindow* tm_window = (wxWindow*)ar_event.GetEventObject();
        tm_window->Navigate();
    }

    wxChar tm_keychar = ar_event.GetUnicodeKey();

    if( tm_keychar == WXK_NONE )
    {
        int tm_keycode = ar_event.GetKeyCode();
        wxLogStatus("Key Pressed: %d", tm_keycode);
    }
    else
    {
        wxLogStatus("Key Pressed: %c", tm_keychar);
    }
}
*/
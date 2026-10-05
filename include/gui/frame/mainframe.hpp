// Abdulkadir U. - 2026/10/01
#pragma once

/**
 * 
 * g++ -std=c++17 -Iinclude/ source/gui/gui.cpp `wx-config --cxxflags --libs` -Wall -Werror -Wextra -o build/app-gui.exe
 */

// Include
#include <wx/wx.h>
#include <wx/valnum.h>

// Namespace
namespace gui::frame
{
    // Class
    class MainFrame : public wxFrame
    {
        private:
            struct ButtonTheme
            {
                wxColour m_normal;
                wxColour m_hover;
                wxColour m_pressed;
            };

            wxPanel* m_panel { nullptr };
            wxStaticText* m_title_kripton { nullptr };
            wxButton* m_btn_disconnect { nullptr };
            wxButton* m_btn_connect { nullptr };
            wxButton* m_btn_update { nullptr };
            wxButton* m_btn_send { nullptr };
            wxStaticText* m_user_count { nullptr };
            wxTextCtrl* m_chat_history { nullptr };
            wxTextCtrl* m_chat_input { nullptr };
            wxTextCtrl* m_max_same_ip_limit { nullptr };
            wxTextCtrl* m_max_connection_limit { nullptr };
            wxChoice* m_crypt_list { nullptr };
            wxTextCtrl* m_log_filepath { nullptr };
            wxTextCtrl* m_port { nullptr };
            wxTextCtrl* m_server_ip { nullptr };
            wxTextCtrl* m_username { nullptr };
            wxTextCtrl* m_password { nullptr };
            wxStaticText* m_connection_status { nullptr };
            wxRadioBox* m_client_or_server { nullptr };
            wxRadioBox* m_ipv4_or_v6 { nullptr };
            wxCheckBox* m_pwd_require { nullptr };
            wxCheckBox* m_logger { nullptr };

            virtual void SetupButtonTheme(wxButton* ar_btn, const ButtonTheme& ar_theme);
            virtual void AllowOnlyNumbers(wxKeyEvent& ar_event);

            virtual void CreateControls();

        public:
            MainFrame(const wxString& ar_title);
    };

    /**
     * @brief Main Frame
     */
    MainFrame::MainFrame(const wxString& ar_title)
        : wxFrame(nullptr, wxID_ANY, ar_title)
    {
        this->CreateControls();
    }

    /**
     * @brief Setup Button Theme
     */
    void MainFrame::SetupButtonTheme(wxButton* ar_btn, const ButtonTheme& ar_theme)
    {
        if( !ar_btn ) return;

        // İlk durum rengi
        ar_btn->SetBackgroundColour(ar_theme.m_normal);

        // Tıklanma (sol tuş)
        ar_btn->Bind(wxEVT_LEFT_DOWN, [ar_btn, ar_theme](wxMouseEvent& tm_event) {
            ar_btn->SetBackgroundColour(ar_theme.m_pressed);
            ar_btn->Refresh();
            tm_event.Skip();
        });

        // Bırakılma (sol tuş)
        ar_btn->Bind(wxEVT_LEFT_UP, [ar_btn, ar_theme](wxMouseEvent& tm_event) {
            ar_btn->SetBackgroundColour(ar_theme.m_hover);
            ar_btn->Refresh();
            tm_event.Skip();
        });

        // Fare üzerine geldi (hover)
        ar_btn->Bind(wxEVT_ENTER_WINDOW, [ar_btn, ar_theme](wxMouseEvent& tm_event) {
            ar_btn->SetBackgroundColour(ar_theme.m_hover);
            ar_btn->Refresh();
            tm_event.Skip();
        });

        // Fare ayrıldı (normal)
        ar_btn->Bind(wxEVT_LEAVE_WINDOW, [ar_btn, ar_theme](wxMouseEvent& tm_event) {
            ar_btn->SetBackgroundColour(ar_theme.m_normal);
            ar_btn->Refresh();
            tm_event.Skip();
        });
    }

    /**
     * @brief Allow Only Numbers
     */
    void MainFrame::AllowOnlyNumbers(wxKeyEvent& ar_event)
    {
        int tm_keycode = ar_event.GetKeyCode();

        if((tm_keycode >= '0' && tm_keycode <= '9') ||
            tm_keycode == WXK_BACK ||
            tm_keycode == WXK_DELETE ||
            tm_keycode == WXK_LEFT ||
            tm_keycode == WXK_RIGHT ||
            tm_keycode == WXK_TAB
        )
            ar_event.Skip();
        else
            wxBell();
    }

    /**
     * @brief Create Controls
     */
    void MainFrame::CreateControls()
    {
        // Font Tanımlamaları
        wxFont tm_header_font(wxFontInfo(wxSize(0, 36)).Bold());
        wxFont tm_main_font(wxFontInfo(wxSize(0, 18)));
        wxFont tm_chat_font(wxFontInfo(wxSize(0, 22)).Bold());
        wxFont tm_btn_font(wxFontInfo(wxSize(0, 14)).Bold());
        wxFont tm_small_font(wxFontInfo(wxSize(0, 12)).Bold());

	    // Renkler
	    wxColour tm_bg_colour(48, 48, 48);
	    wxColour tm_fg_colour(228, 228, 228);
        wxColour tm_title_colour(21, 118, 179);

        ButtonTheme tm_disconnect_theme = {
            wxColour(184, 50, 50),
            wxColour(148, 34, 34),
            wxColour(102, 18, 18)
        };

        ButtonTheme tm_connect_theme = {
            wxColour(34, 186, 69),
            wxColour(28, 156, 57),
            wxColour(19, 117, 39)
        };

        ButtonTheme tm_send_theme = {
            wxColour(49, 108, 176),
            wxColour(20, 54, 156),
            wxColour(11, 35, 102)
        };

        ButtonTheme tm_update_theme = tm_send_theme;

        // Windows RichEdit kontrolünün metin rengini siyah varsaymasını engeller
	    wxTextAttr tm_def_attr;
	    tm_def_attr.SetTextColour(tm_fg_colour);
	    tm_def_attr.SetBackgroundColour(tm_bg_colour);
	    tm_def_attr.SetFont(tm_chat_font);

        // Ana Panel
        this->m_panel = new wxPanel(this);
        this->m_panel->SetFont(tm_main_font);
        this->m_panel->SetBackgroundColour(wxColour(76, 76, 76));
        this->m_panel->SetForegroundColour(wxColour(218, 218, 218));

        // Ana Panel Dikey Sizer (Tüm Elemanları Üstten Alta Dizer)
        wxBoxSizer* tm_panel_sizer = new wxBoxSizer(wxVERTICAL);

        // Başlık (Kripton)
        this->m_title_kripton = new wxStaticText(this->m_panel, wxID_ANY, "Kripton",
            wxDefaultPosition, wxDefaultSize,
            wxALIGN_CENTER_HORIZONTAL | wxST_NO_AUTORESIZE
        );
        this->m_title_kripton->SetFont(tm_header_font);
        this->m_title_kripton->SetBackgroundColour(tm_title_colour);
        this->m_title_kripton->SetForegroundColour(tm_fg_colour);

        // proportion = 0 (Yükseklik sabit), wxEXPAND (Yatayda tam genişlik kaplasın)
        tm_panel_sizer->Add(this->m_title_kripton, 0, wxEXPAND | wxBOTTOM, 10);

        // Üst Ara Çubuk (Butonlar + Sayaç için Yatay Sizer)
        wxBoxSizer* tm_top_control_sizer = new wxBoxSizer(wxHORIZONTAL);

        // Bağlantı kes butonu
        this->m_btn_disconnect = new wxButton(this->m_panel, wxID_ANY, "Disconnect",
            wxDefaultPosition, wxSize(90, 30),
            wxBORDER_NONE
        );
        this->m_btn_disconnect->SetFont(tm_btn_font);
        this->m_btn_disconnect->SetForegroundColour(tm_fg_colour);
        this->SetupButtonTheme(this->m_btn_disconnect, tm_disconnect_theme);

        // Bağlan butonu
        this->m_btn_connect = new wxButton(this->m_panel, wxID_ANY, "Connect",
            wxDefaultPosition, wxSize(90, 30),
            wxBORDER_NONE
        );
        this->m_btn_connect->SetFont(tm_btn_font);
        this->m_btn_connect->SetForegroundColour(tm_fg_colour);
        this->SetupButtonTheme(this->m_btn_connect, tm_connect_theme);

        // Güncelle Butonu
        this->m_btn_update = new wxButton(this->m_panel, wxID_ANY, "Update",
            wxDefaultPosition, wxSize(90, 30),
            wxBORDER_NONE
        );
        this->m_btn_update->SetFont(tm_btn_font);
        this->m_btn_update->SetForegroundColour(tm_fg_colour);
        this->SetupButtonTheme(this->m_btn_update, tm_update_theme);

        // Kullanıcı sayacı
        this->m_user_count = new wxStaticText(this->m_panel, wxID_ANY, "",
            wxDefaultPosition, wxSize(80, -1),
            wxALIGN_CENTER_HORIZONTAL | wxST_NO_AUTORESIZE
        );

        this->m_user_count->SetForegroundColour(tm_fg_colour);
        this->m_user_count->SetFont(tm_chat_font);

        // Disconnect Butonu (Sol tarafta)
        tm_top_control_sizer->Add(this->m_btn_disconnect, 0, wxRIGHT, 10);

        // Esnek Boşluk (Disconnect ile Connect arasını pencere büyüdükçe açar)
        tm_top_control_sizer->AddStretchSpacer(1);

        // Connect ve User Counter (Sağ tarafta yan yana)
        tm_top_control_sizer->Add(this->m_user_count, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
        tm_top_control_sizer->Add(this->m_btn_update, 0, wxRIGHT, 10);
        tm_top_control_sizer->Add(this->m_btn_connect, 0);

        // Üst Ara Çubuğu Ana Sizer'a Ekle
        tm_panel_sizer->Add(tm_top_control_sizer, 0,
            wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10
        );

        // Mesaj ekranı
        this->m_chat_history = new wxTextCtrl(this->m_panel, wxID_ANY, "",
            wxDefaultPosition, wxDefaultSize,
            wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2 | wxBORDER_NONE
        );

        this->m_chat_history->SetEditable(false);

        this->m_chat_history->SetBackgroundColour(tm_bg_colour);
        this->m_chat_history->SetForegroundColour(tm_fg_colour);
        this->m_chat_history->SetFont(tm_chat_font);
	    this->m_chat_history->SetDefaultStyle(tm_def_attr);

        // Pencere dikeyde büyüdükçe Chat alanı kalan TÜM dikey alanı kaplar!
        // wxEXPAND: Yatayda da tam genişlik kaplar.
        tm_panel_sizer->Add(this->m_chat_history, 1,
            wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10
        );

        // Mesaj yazma kısmı
        this->m_chat_input = new wxTextCtrl(this->m_panel, wxID_ANY, "",
            wxDefaultPosition, wxSize(-1, 50),
            wxTE_MULTILINE | wxTE_RICH2 | wxTE_PROCESS_ENTER | wxBORDER_NONE
        );

        this->m_chat_input->SetBackgroundColour(tm_bg_colour);
        this->m_chat_input->SetForegroundColour(tm_fg_colour);
        this->m_chat_input->SetFont(tm_chat_font);
	    this->m_chat_input->SetDefaultStyle(tm_def_attr);

        tm_panel_sizer->Add(this->m_chat_input, 0,
            wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10
        );

        // Orta Ara Çubuk (Gönder + Bağlantı durumu için Yatay Sizer)
        wxBoxSizer* tm_mid_control_sizer = new wxBoxSizer(wxHORIZONTAL);

        // Mesaj gönderme butonu
        this->m_btn_send = new wxButton(this->m_panel, wxID_ANY, "Send",
            wxDefaultPosition, wxSize(90, 30),
            wxBORDER_NONE
        );
        this->m_btn_send->SetFont(tm_btn_font);
        this->m_btn_send->SetForegroundColour(tm_fg_colour);
        this->SetupButtonTheme(this->m_btn_send, tm_send_theme);

        // Bağlantı durumu göstergesi
        this->m_connection_status = new wxStaticText(this->m_panel, wxID_ANY, "Not Connected",
            wxDefaultPosition, wxSize(140, 40),
            wxALIGN_CENTER_HORIZONTAL | wxST_NO_AUTORESIZE
        );

        // Gönder Butonu (Sol tarafta)
        tm_mid_control_sizer->Add(this->m_btn_send, 0, wxRIGHT, 10);

        // Esnek Boşluk (arasını pencere büyüdükçe açar)
        tm_mid_control_sizer->AddStretchSpacer(1);

        // Bağlantı durumu (Sağ tarafta)
        tm_mid_control_sizer->Add(this->m_connection_status, 0,
            wxRIGHT | wxALIGN_CENTER_VERTICAL, 10
        );

        // Üst Ara Çubuğu Ana Sizer'a Ekle
        tm_panel_sizer->Add(tm_mid_control_sizer, 0,
            wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10
        );

        // Alt RadioBox Ara Çubuk
        wxBoxSizer* tm_radio_control_sizer = new wxBoxSizer(wxHORIZONTAL);

        // Liste (istemci, sunucu)
        wxArrayString tm_client_server_list;
        tm_client_server_list.Add("Client");
        tm_client_server_list.Add("Server");

        // Liste (ip türü v4/v6)
        wxArrayString tm_ipv_type_list;
        tm_ipv_type_list.Add("v4");
        tm_ipv_type_list.Add("v6");

        // Radiobox, sunucu ya da istemci seçimi için
        this->m_client_or_server = new wxRadioBox(this->m_panel, wxID_ANY, "User Type",
            wxDefaultPosition, wxSize(150, -1),
            tm_client_server_list, 2, wxRA_SPECIFY_COLS
        );
        this->m_client_or_server->SetFont(tm_small_font);

        // Radiobox, ip türü seçimi için
        this->m_ipv4_or_v6 = new wxRadioBox(this->m_panel, wxID_ANY, "Ip Version",
            wxDefaultPosition, wxSize(100, -1),
            tm_ipv_type_list, 2, wxRA_SPECIFY_COLS
        );
        this->m_ipv4_or_v6->SetFont(tm_small_font);

        // Şifre gerekli seçimi
        this->m_pwd_require = new wxCheckBox(this->m_panel, wxID_ANY, "Password Require",
            wxDefaultPosition, wxDefaultSize
        );
        this->m_pwd_require->SetFont(tm_small_font);

        // Kayıt alınsın mı seçimi
        this->m_logger = new wxCheckBox(this->m_panel, wxID_ANY, "Log",
            wxDefaultPosition, wxDefaultSize
        );
        this->m_logger->SetFont(tm_small_font);

        // Şifreleme türleri
        wxArrayString tm_enc_list;
        tm_enc_list.Add("Xor");

        this->m_crypt_list = new wxChoice(this->m_panel, wxID_ANY,
            wxDefaultPosition, wxSize(100, -1),
            tm_enc_list
        );
        this->m_crypt_list->SetFont(tm_small_font);
        this->m_crypt_list->SetSelection(0);

        // İstemci/Sunucu (Sol tarafta)
        tm_radio_control_sizer->Add(this->m_client_or_server, 0, wxRIGHT, 10);

        // İp türü (Sağ tarafta)
        tm_radio_control_sizer->Add(this->m_ipv4_or_v6, 0, wxRIGHT, 10);

        // Checkboxlar (Sağ tarafta)
        tm_radio_control_sizer->Add(this->m_pwd_require, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);
        tm_radio_control_sizer->Add(this->m_logger, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);

        // Şifreleme türleri (Sağ tarafta)
        tm_radio_control_sizer->Add(this->m_crypt_list, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);

        // Ana Sizer'a Ekle
        tm_panel_sizer->Add(tm_radio_control_sizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

        // Alt Veri Kısmı (Limitler, Bilgiler vs. için Yatay Sizer)
        wxBoxSizer* tm_data_control_sizer = new wxBoxSizer(wxHORIZONTAL);

        // En fazla bağlantı ve en fazla aynı ip giriş limiti
        wxIntegerValidator<int> tm_numeric_val;

        this->m_max_same_ip_limit = new wxTextCtrl(this->m_panel, wxID_ANY, "",
            wxDefaultPosition, wxSize(100, -1),
            wxTE_RICH2 | wxTE_PROCESS_ENTER | wxBORDER_NONE
        );
        this->m_max_same_ip_limit->Bind(wxEVT_CHAR, &MainFrame::AllowOnlyNumbers, this);
        this->m_max_same_ip_limit->SetValidator(tm_numeric_val);
        this->m_max_same_ip_limit->SetHint("Max Same Ip");
        this->m_max_same_ip_limit->SetFont(tm_small_font);
        this->m_max_same_ip_limit->SetForegroundColour(tm_fg_colour);
        this->m_max_same_ip_limit->SetBackgroundColour(tm_bg_colour);

        this->m_max_connection_limit = new wxTextCtrl(this->m_panel, wxID_ANY, "",
            wxDefaultPosition, wxSize(100, -1),
            wxTE_RICH2 | wxTE_PROCESS_ENTER | wxBORDER_NONE
        );
        this->m_max_connection_limit->Bind(wxEVT_CHAR, &MainFrame::AllowOnlyNumbers, this);
        this->m_max_connection_limit->SetValidator(tm_numeric_val);
        this->m_max_connection_limit->SetHint("Max Connection");
        this->m_max_connection_limit->SetFont(tm_small_font);
        this->m_max_connection_limit->SetForegroundColour(tm_fg_colour);
        this->m_max_connection_limit->SetBackgroundColour(tm_bg_colour);

        tm_data_control_sizer->Add(this->m_max_same_ip_limit, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);
        tm_data_control_sizer->Add(this->m_max_connection_limit, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);

        // Ana Sizer'a Ekle
        tm_panel_sizer->Add(tm_data_control_sizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

        // Bütün Elemanlar Sizer'a Eklendikten Sonra Panelle Bağlama Yapılır
        this->m_panel->SetSizer(tm_panel_sizer);
        this->m_panel->Layout();
    }
}
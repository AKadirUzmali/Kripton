// Abdulkadir U. - 2026/10/01
#pragma once

/**
 * Main Frame (Ana Ekran)
 * 
 * Grafik arayüzünde yapılan her şeyin işlendiği ana dosya.
 * Butonlar, yazılar, kutucuklar ve dahası burada oluşturuluyor.
 * Ne işlem yapacakları belirleniyor ve sistemin ona göre
 * işleyişi sağlanıyor, bu sayede kullanıcıların uygulamayı
 * kolayca kullanabileceği bir arayüz ortaya çıkıyor.
 * wxWidgets ile geliştiriliyor ve ekran tasarımı bitti,
 * sırada ise ekranda bulunan elemanların işlevini sağlamak
 * ve programı çalıştırmayı denemek. (2026/10/05)
 */

// Include
#include <memory>

#include <wx/wx.h>
#include <wx/valnum.h>
#include <wx/richtext/richtextctrl.h>

#include <pool/socketpool.hpp>
#include <pool/cipherpool.hpp>

#include <kits/corekit.hpp>
#include <kits/toolkit.hpp>

#include <dev/developer.hpp>

// Namespace
namespace gui::frame
{
    // Using Namespace
    using namespace pool;

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

            struct ClientSession
            {
                std::unique_ptr<cipherpool::Algorithm> m_cipher;
                std::unique_ptr<socketpool::client::Client> m_client;
            };

            struct ServerSession
            {
                std::unique_ptr<cipherpool::Algorithm> m_cipher;
                std::unique_ptr<socketpool::server::Server> m_server;
            };

            wxPanel* m_panel { nullptr };
            wxStaticText* m_title_kripton { nullptr };
            wxButton* m_btn_disconnect { nullptr };
            wxButton* m_btn_connect { nullptr };
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
            wxTextCtrl* m_encryption_key { nullptr };
            wxStaticText* m_connection_status { nullptr };
            wxRadioBox* m_client_or_server { nullptr };
            wxRadioBox* m_ipv4_or_v6 { nullptr };
            wxCheckBox* m_pwd_require { nullptr };
            wxCheckBox* m_log_require { nullptr };

            std::vector<ClientSession> m_client_sessions;
            std::vector<ServerSession> m_server_sessions;

            virtual void SetupButtonTheme(wxButton* ar_btn, const ButtonTheme& ar_theme);

            virtual void CreateControls();
            virtual void SetupEvents();

            virtual void OnConnectClicked(wxCommandEvent& ar_event);
            virtual void OnDisconnectClicked(wxCommandEvent& ar_event);
            virtual void OnSendMessage(wxCommandEvent& ar_event);
            virtual void OnUserTypeChanged(wxCommandEvent& ar_event);
            virtual void OnIpTypeChanged(wxCommandEvent& ar_event);
            virtual void OnCryptSelectionChanged(wxCommandEvent& ar_event);
            virtual void OnPasswordRequireToggled(wxCommandEvent& ar_event);
            virtual void OnLogRequireToggled(wxCommandEvent& ar_event);

            virtual void HandleServer(socketpool::server::Server& ar_server, const socketpool::socket_t& ar_socket);
            virtual void HandleClient(socketpool::client::Client& ar_client);

        public:
            MainFrame(const wxString& ar_title);
    };
}

// Namespace
namespace gui::frame
{
    /**
     * @brief Main Frame
     */
    MainFrame::MainFrame(const wxString& ar_title)
        : wxFrame(nullptr, wxID_ANY, ar_title)
    {
        this->CreateControls();
        this->SetupEvents();
    }

    /**
     * @brief Setup Events
     * 
     * Butonlar, metinler ve dahasının işlevlerini belirleyecek
     * olan fonksiyonları ayarlar
     */
    void MainFrame::SetupEvents()
    {
        // Butonlar
        this->m_btn_connect->Bind(wxEVT_BUTTON, &MainFrame::OnConnectClicked, this);
        this->m_btn_disconnect->Bind(wxEVT_BUTTON, &MainFrame::OnDisconnectClicked, this);
        this->m_btn_send->Bind(wxEVT_BUTTON, &MainFrame::OnSendMessage, this);

        // Seçimler
        this->m_crypt_list->Bind(wxEVT_CHOICE, &MainFrame::OnCryptSelectionChanged, this);
        this->m_client_or_server->Bind(wxEVT_RADIOBOX, &MainFrame::OnUserTypeChanged, this);
        this->m_ipv4_or_v6->Bind(wxEVT_RADIOBOX, &MainFrame::OnIpTypeChanged, this);
        this->m_pwd_require->Bind(wxEVT_CHECKBOX, &MainFrame::OnPasswordRequireToggled, this);
        this->m_log_require->Bind(wxEVT_CHECKBOX, &MainFrame::OnLogRequireToggled, this);
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
            wxTE_MULTILINE | wxTE_RICH2 | wxBORDER_NONE
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
            wxDefaultPosition, wxSize(250, -1),
            wxALIGN_CENTER_HORIZONTAL | wxST_NO_AUTORESIZE
        );
        this->m_connection_status->SetFont(tm_btn_font);
        this->m_connection_status->SetForegroundColour(tm_fg_colour);
        this->m_connection_status->SetBackgroundColour(tm_bg_colour);

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
        this->m_log_require = new wxCheckBox(this->m_panel, wxID_ANY, "Log",
            wxDefaultPosition, wxDefaultSize
        );
        this->m_log_require->SetFont(tm_small_font);

        // Şifreleme türleri
        wxArrayString tm_enc_list;
        for( cipherpool::CipherInfo tm_cipher : cipherpool::st_cipher_list )
            tm_enc_list.Add(tm_cipher.m_name);

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
        tm_radio_control_sizer->Add(this->m_log_require, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);

        // Şifreleme türleri (Sağ tarafta)
        tm_radio_control_sizer->Add(this->m_crypt_list, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);

        // Ana Sizer'a Ekle
        tm_panel_sizer->Add(tm_radio_control_sizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

        // Alt Veri Kısmı (Limitler, Bilgiler vs. için Yatay Sizer)
        wxBoxSizer* tm_data_control_sizer = new wxBoxSizer(wxHORIZONTAL);

        // En fazla bağlantı ve en fazla aynı ip giriş limiti
        wxIntegerValidator<int> tm_max_same_ip_validator(nullptr, wxNUM_VAL_DEFAULT);
        this->m_max_same_ip_limit = new wxTextCtrl(this->m_panel, wxID_ANY, "",
            wxDefaultPosition, wxSize(100, -1),
            wxTE_RICH2 | wxBORDER_NONE,
            tm_max_same_ip_validator
        );
        this->m_max_same_ip_limit->SetHint("Max Same Ip");
        this->m_max_same_ip_limit->SetFont(tm_small_font);
        this->m_max_same_ip_limit->SetForegroundColour(tm_fg_colour);
        this->m_max_same_ip_limit->SetBackgroundColour(tm_bg_colour);

        wxIntegerValidator<int> tm_max_conn_validator(nullptr, wxNUM_VAL_DEFAULT);
        this->m_max_connection_limit = new wxTextCtrl(this->m_panel, wxID_ANY, "",
            wxDefaultPosition, wxSize(100, -1),
            wxTE_RICH2 | wxBORDER_NONE
        );
        this->m_max_connection_limit->SetHint("Max Connection");
        this->m_max_connection_limit->SetFont(tm_small_font);
        this->m_max_connection_limit->SetForegroundColour(tm_fg_colour);
        this->m_max_connection_limit->SetBackgroundColour(tm_bg_colour);

        tm_data_control_sizer->Add(this->m_max_same_ip_limit, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);
        tm_data_control_sizer->Add(this->m_max_connection_limit, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);

        // Port Numarası
        wxIntegerValidator<int> tm_port_validator(nullptr, wxNUM_VAL_DEFAULT);
        tm_port_validator.SetRange(socketpool::_MIN_PORT, socketpool::_MAX_PORT);

        this->m_port = new wxTextCtrl(this->m_panel, wxID_ANY, "",
            wxDefaultPosition, wxSize(50, -1),
            wxTE_RICH2 | wxBORDER_NONE,
            tm_port_validator
        );
        this->m_port->SetHint("Port");
        this->m_port->SetFont(tm_small_font);
        this->m_port->SetForegroundColour(tm_fg_colour);
        this->m_port->SetBackgroundColour(tm_bg_colour);

        tm_data_control_sizer->Add(this->m_port, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);

        // Kayıt Dosyası Konumu
        this->m_log_filepath = new wxTextCtrl(this->m_panel, wxID_ANY, "",
            wxDefaultPosition, wxSize(500, -1),
            wxTE_RICH2 | wxBORDER_NONE
        );
        this->m_log_filepath->SetHint("Log Filepath");
        this->m_log_filepath->SetFont(tm_small_font);
        this->m_log_filepath->SetForegroundColour(tm_fg_colour);
        this->m_log_filepath->SetBackgroundColour(tm_bg_colour);

        tm_data_control_sizer->Add(this->m_log_filepath, 0, wxALIGN_CENTER_VERTICAL, 10);

        // Ana Sizer'a Ekle
        tm_panel_sizer->Add(tm_data_control_sizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

        // Giriş Veri Kısmı (Kullanıcı Adı, Ip Adresi, Şifre vs. için Yatay Sizer)
        wxBoxSizer* tm_login_sizer = new wxBoxSizer(wxHORIZONTAL);

        // Kullanıcı Adı girme kısmı
        this->m_username = new wxTextCtrl(this->m_panel, wxID_ANY, "",
            wxDefaultPosition, wxSize(150, -1),
            wxTE_RICH2 | wxBORDER_NONE
        );
        this->m_username->SetHint("Username");
        this->m_username->SetFont(tm_small_font);
        this->m_username->SetForegroundColour(tm_fg_colour);
        this->m_username->SetBackgroundColour(tm_bg_colour);

        tm_login_sizer->Add(this->m_username, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);

        // Ip Adresi girme kısmı
        this->m_server_ip = new wxTextCtrl(this->m_panel, wxID_ANY, "",
            wxDefaultPosition, wxSize(150, -1),
            wxTE_RICH2 | wxBORDER_NONE
        );
        this->m_server_ip->SetHint("Ip Address");
        this->m_server_ip->SetFont(tm_small_font);
        this->m_server_ip->SetForegroundColour(tm_fg_colour);
        this->m_server_ip->SetBackgroundColour(tm_bg_colour);

        tm_login_sizer->Add(this->m_server_ip, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);

        // Şifre girme kısmı
        this->m_password = new wxTextCtrl(this->m_panel, wxID_ANY, "",
            wxDefaultPosition, wxSize(150, -1),
            wxTE_RICH2 | wxBORDER_NONE
        );
        this->m_password->SetHint("Password");
        this->m_password->SetFont(tm_small_font);
        this->m_password->SetForegroundColour(tm_fg_colour);
        this->m_password->SetBackgroundColour(tm_bg_colour);

        tm_login_sizer->Add(this->m_password, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);

        // Kriptografi Anahtarı girme kısmı
        this->m_encryption_key = new wxTextCtrl(this->m_panel, wxID_ANY, "",
            wxDefaultPosition, wxSize(300, -1),
            wxTE_RICH2 | wxBORDER_NONE
        );
        this->m_encryption_key->SetHint("Encryption Key");
        this->m_encryption_key->SetFont(tm_small_font);
        this->m_encryption_key->SetForegroundColour(tm_fg_colour);
        this->m_encryption_key->SetBackgroundColour(tm_bg_colour);

        tm_login_sizer->Add(this->m_encryption_key, 0, wxALIGN_CENTER_VERTICAL, 10);

        // Ana Sizer'a Ekle
        tm_panel_sizer->Add(tm_login_sizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

        // Bütün Elemanlar Sizer'a Eklendikten Sonra Panelle Bağlama Yapılır
        this->m_panel->SetSizer(tm_panel_sizer);
        this->m_panel->Layout();
    }

    /**
     * @brief On Connect Clicked
     * 
     * @param wxCommandEvent& Event
     */
    void MainFrame::OnConnectClicked(wxCommandEvent& ar_event)
    {
        // Gerekli değerleri alsın
        wxString tm_ip_str = this->m_server_ip->GetValue();
        wxString tm_port_str = this->m_port->GetValue().Trim();

        const int tm_ipv_type_index = this->m_ipv4_or_v6->GetSelection();

        // Port adresi değerini kontrol et
        int tm_port_int = 0;
        if( !tm_port_str.ToInt(&tm_port_int) )
        {
            wxMessageBox("Invalid port format, please just enter a number!", "Error",
                wxOK | wxICON_ERROR, this);

            return;
        }

        // Port adresinin geçerli olup olmadığını kontrol et
        if( !socketpool::Socket::is_valid_port(tm_port_int) )
        {
            wxMessageBox(wxString::Format("Port value must be between %d and %d!", socketpool::_MIN_PORT, socketpool::_MAX_PORT), "Error",
                wxOK | wxICON_ERROR, this);

            return;
        }

        // Port adresi değerini tutan değişken
        socketpool::socket_port_t tm_port = static_cast<socketpool::socket_port_t>(tm_port_int);

        // Ip türünü kontrol et
        socketpool::ipv_t tm_ipv_type = tm_ipv_type_index == 1 /* Ipv6 */
            ? socketpool::ipv_t::ipv6 : socketpool::ipv_t::ipv4;

        // Ip adresini kontrol et
        std::string tm_ip = tm_ip_str.ToUTF8().data();

        switch( tm_ipv_type )
        {
            // Ipv6
            case socketpool::ipv_t::ipv6:
                if( !socketpool::Socket::is_valid_ipv6(tm_ip) )
                {
                    wxMessageBox("Ipv6 adress is not valid!", "Error",
                        wxOK | wxICON_ERROR, this);

                    return;
                }
            break;

            // Ipv4
            case socketpool::ipv_t::ipv4:
            default:
                if( !socketpool::Socket::is_valid_ipv4(tm_ip) )
                {
                    wxMessageBox("Ipv4 adress is not valid!", "Error",
                        wxOK | wxICON_ERROR, this);

                    return;
                }
        }

        // Şifreleme türü değerini alsın
        wxString tm_cipher_type_index = this->m_crypt_list->GetStringSelection();
        cipherpool::ecipher_t tm_cipher_index = cipherpool::ecipher_t::Null;

        for( const cipherpool::CipherInfo& tm_cipher : cipherpool::st_cipher_list )
        {
            // Hala bulamadı, devam etsin
            if( tm_cipher_type_index != wxString::FromUTF8(tm_cipher.m_name) )
                continue;

            // eşleşme sağlandı
            tm_cipher_index = tm_cipher.m_type;
        }

        // Şifreleme türünü bulamadı
        if( tm_cipher_index == cipherpool::ecipher_t::Null )
        {
            wxMessageBox("Encryption type couldn't find in list!", "Error",
                wxOK | wxICON_ERROR, this);

            return;
        }

        // Şifreleme türünü bul
        wxString tm_enc_key_str = this->m_encryption_key->GetValue();
        const std::string tm_crypt_key = tm_enc_key_str.ToUTF8().data();

        // Kalan diğer değerleri alsın
        wxString tm_username_str = this->m_username->GetValue();
        wxString tm_pwd_str = this->m_password->GetValue();
        wxString tm_log_filepath_str = this->m_log_filepath->GetValue();
        wxString tm_max_conn_str = this->m_max_connection_limit->GetValue().Trim();
        wxString tm_max_same_ip_str = this->m_max_same_ip_limit->GetValue().Trim();

        const bool tm_is_pwd_required = this->m_pwd_require->IsChecked();
        const bool tm_is_log_required = this->m_log_require->IsChecked();

        const int tm_user_type_index = this->m_client_or_server->GetSelection();

        // En fazla bağlanabilir kullanıcı değeri
        socketpool::policy::max_conn_t tm_max_conn = socketpool::policy::_DEF_CONNECTION;

        int tm_max_conn_int = 0;
        if( tm_max_conn_str.ToInt(&tm_max_conn_int) )
            tm_max_conn = static_cast<socketpool::policy::max_conn_t>(tm_max_conn_int);

        // En fazla bağlanabilir aynı ip adresi değeri
        socketpool::policy::max_conn_t tm_max_same_ip = socketpool::policy::_DEF_SAME_IP_COUNT;

        int tm_max_same_ip_int = 0;
        if( tm_max_same_ip_str.ToInt(&tm_max_same_ip_int) )
            tm_max_same_ip = static_cast<socketpool::policy::max_conn_t>(tm_max_same_ip_int);

        // Şifreleme anahtarı
        std::unique_ptr<cipherpool::Algorithm> tm_cipher = nullptr;

        switch( tm_cipher_index )
        {
            case cipherpool::ecipher_t::Xor:
                tm_cipher = std::make_unique<cipherpool::Xor>("Network Cipher", tm_crypt_key);
            break;
            
            default:
                return;
        }
            
        // Kullanıcı türüne göre işlem
        switch( tm_user_type_index )
        {
            // Sunucu
            case 1:
            {
                auto tm_network = std::make_unique<socketpool::server::Server>
                (
                    *tm_cipher,
                    tm_log_filepath_str.ToUTF8().data(),
                    tm_username_str.ToUTF8().data(),
                    tm_pwd_str.ToUTF8().data(),
                    tm_is_pwd_required,
                    tm_port,
                    tm_ipv_type,
                    [this](socketpool::server::Server& ar_server, const socketpool::socket_t& ar_socket) {
                        this->HandleServer(ar_server, ar_socket);
                    },
                    tm_max_conn,
                    tm_max_same_ip,
                    (tm_is_log_required ? socketpool::_FLAG_SOCKET_LOGGER : 0)
                );

                // bilgi mesajı
                this->m_connection_status->SetLabel("Server creating...");

                // Çalıştır
                core::status::Status tm_status = tm_network->run();

                // Hata kontrolü
                if( !tm_status.is_ok() )
                {
                    // hata mesajı
                    this->m_connection_status->SetLabel(wxString::Format("Failed to run server, code: %d", tm_status.get_code()));
                    
                    // işaretçiyi temizle
                    tm_cipher.reset();
                    return;
                }
                else
                {
                    // başarı mesajı
                    this->m_connection_status->SetLabel("Server Is Running");

                    // listeye aktar
                    this->m_server_sessions.push_back(ServerSession{
                        std::move(tm_cipher),
                        std::move(tm_network)
                    });
                }
            }
            break;

            // İstemci
            default:
            {
                auto tm_network = std::make_unique<socketpool::client::Client>
                (
                    *tm_cipher,
                    tm_log_filepath_str.ToUTF8().data(),
                    tm_username_str.ToUTF8().data(),
                    tm_pwd_str.ToUTF8().data(),
                    tm_port,
                    tm_ipv_type,
                    tm_ip_str.ToUTF8().data(),
                    [this](socketpool::client::Client& ar_client) {
                        this->HandleClient(ar_client);
                    },
                    (tm_is_log_required ? socketpool::_FLAG_SOCKET_LOGGER : 0)
                );

                // bilgi mesajı
                this->m_connection_status->SetLabel("Connecting To Server...");

                // Çalıştır
                core::status::Status tm_status = tm_network->run();

                // Hata kontrolü
                if( !tm_status.is_ok() )
                {
                    // hata mesajı
                    this->m_connection_status->SetLabel(wxString::Format("Failed To Connect Server, code: %d", tm_status.get_code()));
                    
                    // işaretçiyi temizle
                    tm_cipher.reset();
                    return;
                }
                else
                {
                    // başarı mesajı
                    this->m_connection_status->SetLabel("Connected To Server");

                    // listeye aktar
                    this->m_client_sessions.push_back(ClientSession{
                        std::move(tm_cipher),
                        std::move(tm_network)
                    });
                }
            }
        }
    }

    /**
     * @brief On Disconnect Clicked
     * 
     * @param wxCommandEvent& Event
     */
    void MainFrame::OnDisconnectClicked(wxCommandEvent& ar_event)
    {
        // tüm sunucuları durdur
        for( auto& tm_session : this->m_server_sessions )
        {
            if( tm_session.m_server )
                tm_session.m_server->stop();
        }
        this->m_server_sessions.clear();

        // tüm istemcileri durdur
        for( auto& tm_session : this->m_client_sessions )
        {
            if( tm_session.m_client )
                tm_session.m_client->stop();
        }
        this->m_client_sessions.clear();

        // bilgi çıktısı
        this->m_connection_status->SetLabel("Connections Closed");
    }

    /**
     * @brief On Send Message
     * 
     * @param wxCommandEvent& Event
     */
    void MainFrame::OnSendMessage(wxCommandEvent& ar_event)
    {
        // SEND MESSAGE
        socketpool::DataPacket tm_datapack;

        // STATUS VARIABLE
        core::status::Status tm_status;

        const int tm_user_type_index = this->m_client_or_server->GetSelection();

        auto& tm_current_srv_session = this->m_server_sessions.back().m_server;
        auto& tm_current_cli_session = this->m_client_sessions.back().m_client;

        // data
        tm_datapack.m_name = this->m_username->GetValue().ToUTF8().data();
        tm_datapack.m_msg = this->m_chat_input->GetValue().ToUTF8().data();
        tm_datapack.m_pwd = this->m_password->GetValue().ToUTF8().data();

        switch( tm_user_type_index )
        {
            // server
            case 1:
                for( const auto& [tm_socket, tm_ctx] : tm_current_srv_session->get_clients() )
                {
                    // SEND
                    tm_status = tm_current_srv_session->send(tm_socket, tm_datapack);

                    // print to screen
                    wxDateTime tm_time_now = wxDateTime::Now();
                    wxString tm_time_str = tm_time_now.Format("[%H:%M:%S] ");

                    wxString tm_msg = tm_time_str + wxString::FromUTF8(tm_current_srv_session->get_policy().get_username()) + ": "
                        + wxString::FromUTF8(tm_datapack.m_msg) + "\n";

                    this->m_chat_history->AppendText(tm_msg);
                }
            break;

            // client
            default:
                tm_status = tm_current_cli_session->send(tm_current_cli_session->get_socket(), tm_datapack);

                // print to screen
                wxDateTime tm_time_now = wxDateTime::Now();
                wxString tm_time_str = tm_time_now.Format("[%H:%M:%S] ");

                wxString tm_msg = tm_time_str + wxString::FromUTF8(tm_current_cli_session->get_policy().get_username()) + ": "
                    + wxString::FromUTF8(tm_datapack.m_msg) + "\n";

                this->m_chat_history->AppendText(tm_msg);
        }

        this->m_chat_input->Clear();
        this->m_chat_input->SetFocus();
    }

    void MainFrame::OnUserTypeChanged(wxCommandEvent& ar_event)
    {

    }

    void MainFrame::OnIpTypeChanged(wxCommandEvent& ar_event)
    {

    }

    void MainFrame::OnCryptSelectionChanged(wxCommandEvent& ar_event)
    {

    }

    void MainFrame::OnPasswordRequireToggled(wxCommandEvent& ar_event)
    {

    }

    void MainFrame::OnLogRequireToggled(wxCommandEvent& ar_event)
    {

    }

    /**
     * @brief Handle Server
     * 
     * @param Server& Server Ref
     * @param socket_t& Client Socket Ref
     */
    void MainFrame::HandleServer(
        socketpool::server::Server& ar_server,
        const socketpool::socket_t& ar_socket
    )
    {
        // gecikme süresi
        const auto tm_timeout = std::chrono::seconds(ar_server.get_timeout());

        // çalışıyor
        while( ar_server.is_running() )
        {
            // SET FD
            fd_set tm_readfs;
            FD_ZERO(&tm_readfs);
            FD_SET(ar_socket, &tm_readfs);

            // gecikmeyi ayarla
            timeval tm_tv {};
            tm_tv.tv_sec = tm_timeout.count();
            tm_tv.tv_usec = 0;

            // SELECT
            int tm_ready = ::select(ar_socket + 1, &tm_readfs, nullptr, nullptr, &tm_tv);

            // SELECT ERR
            if( tm_ready < 0 )
                return;
            // SELECT TIMEOUT
            else if( tm_ready == 0 )
                return;

            // Find Ip Address
            const std::string tm_ip = socketpool::Socket::get_ip(ar_socket);

            // BAN CHECK
            if( ar_server.get_policy().is_connection_banned(ar_socket) )
            {
                // LOG IT
                ar_server.get_logger().write(
                    dev::level::level_t::Warn,
                    ar_server.get_policy().get_username(),
                    tm_ip + "/" + std::to_string(ar_socket) + " Banned Ip/Socket Tried To Connect Server",
                    GET_SOURCE
                );
                break;
            }
            // NOT ALLOW CHECK
            else if( !ar_server.get_policy().is_connection_allowed(tm_ip) )
            {
                // LOG IT (DEBUG)
                DEBUG_ONLY(ar_server.get_logger().write(dev::level::level_t::Warn,
                    ar_server.get_policy().get_username(),
                    tm_ip + " Ip Not Allowed",
                    GET_SOURCE)
                );
                break;
            }

            // DATA PACKET
            socketpool::DataPacket tm_datapack;

            // RECEIVE
            core::status::Status tm_status = ar_server.recv(ar_socket, tm_datapack);

            // RECV STATUS
            switch( tm_status.get_status() )
            {
                // OK
                case core::status::status_t::ok:
                {                    
                    // FIND CLIENT
                    auto tm_cli = ar_server.get_clients().find(ar_socket);
                    if( tm_cli != ar_server.get_clients().end() )
                    {
                        // TEMP CLIENT
                        socketpool::SocketCtx tm_storecli {};
                    
                        tm_storecli.m_ip = tm_cli->second.m_ip;
                        tm_storecli.m_user.m_same_user_count = tm_cli->second.m_user.m_same_user_count;
                        tm_storecli.m_user.m_try_passwd = tm_cli->second.m_user.m_try_passwd;
                        tm_storecli.m_user.m_username = tm_datapack.m_name;
                    
                        // UPDATE CLIENT DATA
                        ar_server.update_client(ar_socket, tm_storecli);
                    }

                    // print to screen
                    wxDateTime tm_time_now = wxDateTime::Now();
                    wxString tm_time_str = tm_time_now.Format("[%H:%M:%S] ");

                    wxString tm_msg = tm_time_str + wxString::FromUTF8(tm_cli->second.m_user.m_username) + ": "
                        + wxString::FromUTF8(tm_datapack.m_msg) + "\n";

                    this->m_chat_history->AppendText(tm_msg);
                }
                break;
            
                // ERROR
                case core::status::status_t::err:
                    // ERROR CODE
                    switch( tm_status.get_code() )
                    {
                        // NOT RECV BECAUSE OF CLIENT
                        case core::status::to_underlying(socketpool::socket_code_t::socket_not_recv_header):

                        // CLIENT CONNECTION CLOSED
                        case core::status::to_underlying(socketpool::socket_code_t::recv_socket_close_header):
                            {
                                // FIND CLIENT
                                auto tm_cli = ar_server.get_clients().find(ar_socket);
                                if( tm_cli != ar_server.get_clients().end() )
                                    tm_datapack.m_msg = "User (" + tm_cli->second.m_user.m_username + ") Disconnected";
                                else
                                    tm_datapack.m_msg = "(" + std::to_string(ar_socket) + "/" + tm_ip + ") Disconnected";
                            
                                // CLIENT MSG SENDER NAME
                                tm_datapack.m_name = "Server";
                            
                                // LOG IT
                                ar_server.get_logger().write(dev::level::level_t::Info, tm_datapack.m_msg, GET_SOURCE);
                            
                                // SEND MSG TO ALL CLIENTS
                                for( const auto& [tm_socket, tm_ctx] : ar_server.get_clients() )
                                {
                                    if( tm_socket != ar_socket )
                                        ar_server.send(tm_socket, tm_datapack);
                                }
                            }
                        return;

                        default:
                            return;
                    }
                return;
                
                // DATA RECEIVE ERROR
                default:
                    return;
            }

            // SENT TO ALL CLIENTS
            for( const auto& [tm_socket, tm_ctx] : ar_server.get_clients() )
                tm_status = ar_server.send(tm_socket, tm_datapack);
        }
    }

    /**
     * @brief Handle Client
     * 
     * @param Client& Client Ref
     */
    void MainFrame::HandleClient(
        socketpool::client::Client& ar_client
    )
    {
        // SEND MESSAGE
        socketpool::DataPacket tm_datapack;

        // STATUS VARIABLE
        core::status::Status tm_status;

        // SERVER IP
        const std::string tm_server_ip = ar_client.get_server_ip();

        while( true )
        {
            // RECV
            tm_status = ar_client.recv(ar_client.get_socket(), tm_datapack);

            // RECV STATUS
            switch( tm_status.get_status() )
            {
                // OK
                case core::status::status_t::ok:
                {
                    // print to screen
                    wxDateTime tm_time_now = wxDateTime::Now();
                    wxString tm_time_str = tm_time_now.Format("[%H:%M:%S] ");

                    wxString tm_msg = tm_time_str + wxString::FromUTF8(tm_datapack.m_name) + ": "
                        + wxString::FromUTF8(tm_datapack.m_msg) + "\n";

                    this->m_chat_history->AppendText(tm_msg);
                }
                break;

                // DATA RECEIVE ERROR
                default:
                    return;
            }
        }
    }
}
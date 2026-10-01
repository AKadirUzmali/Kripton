// Abdulkadir U. - 2026/10/01

/**
 * App (Uygulama)
 * 
 * wxWidgets GUI uygulaması için ana giriş noktasıdır.
 * Bu sınıf, wxApp sınıfından türetilmiştir ve uygulamanın başlatılmasını ve ana pencerenin oluşturulmasını yönetir.
 * Yapılan sunucu mesajlaşması için ihtiyaç duyulan gui ortamını sağlaması amacıyla test edilmiştir.
 * Aana uygulama için seçilmiş olan wxWidgets sürümü 3.2'dir ve C++17 dil standardı kullanılmıştır.
 * Ana uygulama için tasarlanacak gui menüsü olan wxWidgets framework'üne ait basit bir test örneğidir.
 * wxWidgets ile ilgili işlevler öğrenmek amaçlı test edilmiştir.
 * 
 * Derleme:
 *  Bsd     :: g++ -std=c++17 `wx-config --cxxflags --libs` -Wall -Werror -Wextra app.cpp -pthread -o bsd/app.bsd
 *  Linux   :: g++ -std=c++17 `wx-config --cxxflags --libs` -Wall -Werror -Wextra app.cpp -o linux/app.linux
 *  Windows :: g++ -std=c++17 `wx-config --cxxflags --libs` -Wall -Werror -Wextra app.cpp -o windows/app.exe
 * 
 * Çalıştırma:
 *  Bsd     :: ./bsd/app.bsd
 *  Linux   :: ./linux/app.linux
 *  Windows :: ./windows/app.exe
 * 
 * Örnek:
 *  Bsd     :: ./bsd/app.bsd
 *  Linux   :: ./linux/app.linux
 *  Windows :: ./windows/app.exe
 * 
 * Sonuç:
 *  Debian/GNU da test edilmiştir. wxWidgets GUI uygulaması başarıyla çalışmaktadır.
 */

// Include
#include "mainframe.hpp"
#include "app.hpp"

// Implementation
wxIMPLEMENT_APP(App);

/**
 * @brief On Init
 */
bool App::OnInit()
{
    MainFrame* tm_mainFrame = new MainFrame("Kripton");
    tm_mainFrame->SetClientSize(800, 600);
    tm_mainFrame->Center();
    tm_mainFrame->Show(true);

    return true;
}
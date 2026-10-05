// Abdulkadir U. - 2026/10/01

/**
 * @brief GUI
 * 
 * Derleme:
 *  Linux       (G++): g++ -std=c++17 -Wall -Wextra -Iinclude/ source/gui/gui.cpp -o build/app-gui.linux `wx-config --cxxflags --libs`
 *  Windows (Mingw64): g++ -std=c++17 -Iinclude/ source/gui/gui.cpp `wx-config --cxxflags --libs` -Wall -Werror -Wextra -o build/app-gui.exe
 */

// Include
#include <gui/gui.hpp>

// App
wxIMPLEMENT_APP(gui::App);
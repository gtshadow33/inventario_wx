#include <wx/wx.h>
#include "./headers/database.h"
#include "./headers/mainwindow.h"

class App : public wxApp {
public:
    bool OnInit() override {
        // Abrir base de datos SQLite
        static Database database("inventario.db");
        if (!database.abrir()) {
            wxMessageBox("No se pudo abrir la base de datos SQLite.",
                         "Error", wxICON_ERROR);
            return false;
        }

        MainWindow *win = new MainWindow(&database);
        win->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(App);

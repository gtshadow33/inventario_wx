#include "./headers/mainwindow.h"
#include <wx/msgdlg.h>
#include <wx/statbox.h>
#include <wx/sizer.h>

// IDs de eventos
enum {
    ID_BTN_AGREGAR = wxID_HIGHEST + 1,
    ID_BTN_EDITAR,
    ID_BTN_BORRAR,
    ID_BTN_BUSCAR,
    ID_LISTA
};

wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
    EVT_BUTTON    (ID_BTN_AGREGAR, MainWindow::OnAgregar)
    EVT_BUTTON    (ID_BTN_EDITAR,  MainWindow::OnEditar)
    EVT_BUTTON    (ID_BTN_BORRAR,  MainWindow::OnBorrar)
    EVT_BUTTON    (ID_BTN_BUSCAR,  MainWindow::OnBuscar)
    EVT_LIST_ITEM_SELECTED(ID_LISTA, MainWindow::OnSeleccion)
wxEND_EVENT_TABLE()

// -------------------------------------------------------------------------
MainWindow::MainWindow(Database *db)
    : wxFrame(nullptr, wxID_ANY, "Inventario de Ordenadores",
              wxDefaultPosition, wxSize(1280, 720))
{
    m_inventario = new Inventario(db);

    // ---- Panel principal ----
    wxPanel *panel = new wxPanel(this);
    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

    // ============================================================
    //  GRUPO FORMULARIO — layout horizontal en 2 bloques
    // ============================================================
    wxStaticBox      *box      = new wxStaticBox(panel, wxID_ANY, "Formulario de Ordenador");
    wxStaticBoxSizer *formSizer = new wxStaticBoxSizer(box, wxVERTICAL);

    // Fila horizontal: campos izquierda | descripción derecha | botones
    wxBoxSizer *rowSizer = new wxBoxSizer(wxHORIZONTAL);

    // --- Bloque izquierdo: grid de campos ---
    wxFlexGridSizer *grid = new wxFlexGridSizer(7, 2, 4, 8);
    grid->AddGrowableCol(1, 1);

    auto addField = [&](const wxString &label, wxTextCtrl *&ctrl) {
        grid->Add(new wxStaticText(panel, wxID_ANY, label), 0, wxALIGN_CENTER_VERTICAL);
        ctrl = new wxTextCtrl(panel, wxID_ANY);
        grid->Add(ctrl, 1, wxEXPAND);
    };

    addField("Nombre:",   m_txtNombre);
    addField("Grupo:",    m_txtGrupo);
    addField("CPU:",      m_txtCPU);

    // RAM (spinctrl especial)
    grid->Add(new wxStaticText(panel, wxID_ANY, "RAM (GB):"), 0, wxALIGN_CENTER_VERTICAL);
    m_spinRAM = new wxSpinCtrl(panel, wxID_ANY, "0", wxDefaultPosition,
                               wxDefaultSize, wxSP_ARROW_KEYS, 0, 1024, 0);
    grid->Add(m_spinRAM, 1, wxEXPAND);

    addField("GPU:",      m_txtGPU);
    addField("Disco:",    m_txtDisco);

    // Funciona (checkbox)
    grid->Add(new wxStaticText(panel, wxID_ANY, "Funciona:"), 0, wxALIGN_CENTER_VERTICAL);
    m_chkFunciona = new wxCheckBox(panel, wxID_ANY, "");
    m_chkFunciona->SetValue(true);
    grid->Add(m_chkFunciona, 0, wxALIGN_CENTER_VERTICAL);

    rowSizer->Add(grid, 2, wxEXPAND | wxRIGHT, 12);

    // --- Bloque derecho: descripción ---
    wxBoxSizer *descSizer = new wxBoxSizer(wxVERTICAL);
    descSizer->Add(new wxStaticText(panel, wxID_ANY, "Descripcion:"), 0, wxBOTTOM, 4);
    m_txtDescripcion = new wxTextCtrl(panel, wxID_ANY, "",
                                      wxDefaultPosition, wxDefaultSize,
                                      wxTE_MULTILINE);
    descSizer->Add(m_txtDescripcion, 1, wxEXPAND);

    rowSizer->Add(descSizer, 1, wxEXPAND);

    formSizer->Add(rowSizer, 1, wxEXPAND | wxALL, 8);

    // ---- Botones ----
    wxBoxSizer *btnSizer = new wxBoxSizer(wxHORIZONTAL);
    btnSizer->Add(new wxButton(panel, ID_BTN_AGREGAR, "Agregar"), 0, wxRIGHT, 6);
    btnSizer->Add(new wxButton(panel, ID_BTN_EDITAR,  "Editar"),  0, wxRIGHT, 6);
    btnSizer->Add(new wxButton(panel, ID_BTN_BORRAR,  "Borrar"),  0, wxRIGHT, 6);
    btnSizer->Add(new wxButton(panel, ID_BTN_BUSCAR,  "Buscar"),  0);
    formSizer->Add(btnSizer, 0, wxLEFT | wxRIGHT | wxBOTTOM, 8);

    // Formulario ocupa ~35% de la altura, tabla el resto
    mainSizer->Add(formSizer, 35, wxEXPAND | wxALL, 8);

    // ============================================================
    //  TABLA (wxListCtrl en modo reporte)
    // ============================================================
    m_lista = new wxListCtrl(panel, ID_LISTA,
                             wxDefaultPosition, wxDefaultSize,
                             wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_SUNKEN);

    // Columnas — anchos fijos para todas menos la última
    const wxString cols[]  = {"ID","Nombre","Grupo","CPU","RAM","GPU","Disco","Funciona","Descripcion"};
    const int      anchos[] = { 45,   150,    100,   120,  60,  120,   100,     80,          -1 };
    for (int i = 0; i < 9; ++i)
        m_lista->InsertColumn(i, cols[i], wxLIST_FORMAT_CENTRE, anchos[i] == -1 ? 200 : anchos[i]);

    mainSizer->Add(m_lista, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

    // Cuando la ventana cambie de tamaño, la columna Descripción ocupa el resto
    Bind(wxEVT_SIZE, [this](wxSizeEvent &e) {
        e.Skip();
        if (!m_lista) return;
        int total = m_lista->GetClientSize().GetWidth();
        int usado = 0;
        for (int i = 0; i < 8; ++i)
            usado += m_lista->GetColumnWidth(i);
        int resto = total - usado;
        if (resto > 50)
            m_lista->SetColumnWidth(8, resto);
    });

    panel->SetSizer(mainSizer);
    ActualizarTabla(m_inventario->listarTodos());
}

// -------------------------------------------------------------------------
//  Actualizar tabla
// -------------------------------------------------------------------------
void MainWindow::ActualizarTabla(const std::vector<Ordenador> &lista) {
    m_lista->DeleteAllItems();

    for (int i = 0; i < (int)lista.size(); ++i) {
        const Ordenador &o = lista[i];

        long idx = m_lista->InsertItem(i, std::to_string(o.id));
        m_lista->SetItem(idx, 1, o.nombre);
        m_lista->SetItem(idx, 2, o.grupo);
        m_lista->SetItem(idx, 3, o.cpu);
        m_lista->SetItem(idx, 4, std::to_string(o.ram));
        m_lista->SetItem(idx, 5, o.gpu);
        m_lista->SetItem(idx, 6, o.disco);
        m_lista->SetItem(idx, 7, o.funciona ? "Si" : "No");
        m_lista->SetItem(idx, 8, o.descripcion);
    }
}

// -------------------------------------------------------------------------
//  Formulario helpers
// -------------------------------------------------------------------------
Ordenador MainWindow::GetOrdenadorFormulario() const {
    Ordenador o;
    o.nombre      = m_txtNombre->GetValue().ToStdString();
    o.grupo       = m_txtGrupo->GetValue().ToStdString();
    o.cpu         = m_txtCPU->GetValue().ToStdString();
    o.ram         = m_spinRAM->GetValue();
    o.gpu         = m_txtGPU->GetValue().ToStdString();
    o.disco       = m_txtDisco->GetValue().ToStdString();
    o.funciona    = m_chkFunciona->GetValue();
    o.descripcion = m_txtDescripcion->GetValue().ToStdString();
    return o;
}

void MainWindow::LimpiarFormulario() {
    m_txtNombre->Clear();
    m_txtGrupo->Clear();
    m_txtCPU->Clear();
    m_spinRAM->SetValue(0);
    m_txtGPU->Clear();
    m_txtDisco->Clear();
    m_chkFunciona->SetValue(true);
    m_txtDescripcion->Clear();
}

void MainWindow::RellenarFormulario(long row) {
    m_txtNombre->SetValue(m_lista->GetItemText(row, 1));
    m_txtGrupo->SetValue(m_lista->GetItemText(row, 2));
    m_txtCPU->SetValue(m_lista->GetItemText(row, 3));

    long ram = 0;
    m_lista->GetItemText(row, 4).ToLong(&ram);
    m_spinRAM->SetValue((int)ram);

    m_txtGPU->SetValue(m_lista->GetItemText(row, 5));
    m_txtDisco->SetValue(m_lista->GetItemText(row, 6));
    m_chkFunciona->SetValue(m_lista->GetItemText(row, 7) == "Sí");
    m_txtDescripcion->SetValue(m_lista->GetItemText(row, 8));
}

// -------------------------------------------------------------------------
//  Eventos de botones
// -------------------------------------------------------------------------
void MainWindow::OnAgregar(wxCommandEvent &) {
    Ordenador o = GetOrdenadorFormulario();
    if (!o.isValid()) {
        wxMessageBox("El nombre y grupo son obligatorios y la descripcion menor a 30 palabras", "Error", wxICON_WARNING);
        return;
    }
    if (m_inventario->agregarOrdenador(o)) {
        wxMessageBox("Ordenador agregado", "OK", wxICON_INFORMATION);
        ActualizarTabla(m_inventario->listarTodos());
        LimpiarFormulario();
    } else {
        wxMessageBox("No se pudo agregar el ordenador", "Error", wxICON_ERROR);
    }
}

void MainWindow::OnEditar(wxCommandEvent &) {
    long sel = m_lista->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (sel == -1) {
        wxMessageBox("Seleccione un ordenador de la tabla", "Error", wxICON_WARNING);
        return;
    }


    long id = 0;
    m_lista->GetItemText(sel, 0).ToLong(&id);

    Ordenador o = GetOrdenadorFormulario();
    o.id = (int)id;

    if (!o.isValid()) {
        wxMessageBox("El nombre y grupo son obligatorios y la descripcion menor a 30 palabras", "Error", wxICON_WARNING);
        return;
    }

    if (m_inventario->modificarOrdenador(o)) {
        wxMessageBox("Ordenador modificado", "OK", wxICON_INFORMATION);
        ActualizarTabla(m_inventario->listarTodos());
        LimpiarFormulario();
    } else {
        wxMessageBox("No se pudo modificar el ordenador", "Error", wxICON_ERROR);
    }
}

void MainWindow::OnBorrar(wxCommandEvent &) {
    long sel = m_lista->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (sel == -1) {
        wxMessageBox("Seleccione un ordenador de la tabla", "Error", wxICON_WARNING);
        return;
    }

    int resp = wxMessageBox("Seguro que quieres borrar este ordenador",
                            "Confirmar", wxYES_NO | wxICON_QUESTION);
    if (resp != wxYES) return;

    long id = 0;
    m_lista->GetItemText(sel, 0).ToLong(&id);

    if (m_inventario->eliminarOrdenador((int)id)) {
        wxMessageBox("Ordenador eliminado", "OK", wxICON_INFORMATION);
        ActualizarTabla(m_inventario->listarTodos());
        LimpiarFormulario();
    } else {
        wxMessageBox("No se pudo eliminar el ordenador", "Error", wxICON_ERROR);
    }
}

void MainWindow::OnBuscar(wxCommandEvent &) {
    std::string nombre = m_txtNombre->GetValue().ToStdString();
    std::string grupo  = m_txtGrupo->GetValue().ToStdString();
    int         ram    = m_spinRAM->GetValue();

    ActualizarTabla(m_inventario->buscar(nombre, grupo, ram));
}

// -------------------------------------------------------------------------
//  Evento de selección en la lista
// -------------------------------------------------------------------------
void MainWindow::OnSeleccion(wxListEvent &evt) {
    long row = evt.GetIndex();
    RellenarFormulario(row);
}
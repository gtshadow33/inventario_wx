#pragma once
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/spinctrl.h>
#include "inventario.h"

class MainWindow : public wxFrame {
public:
    MainWindow(Database *db);

private:
    // --- Widgets del formulario ---
    wxTextCtrl  *m_txtNombre;
    wxTextCtrl  *m_txtGrupo;
    wxTextCtrl  *m_txtCPU;
    wxSpinCtrl  *m_spinRAM;
    wxTextCtrl  *m_txtGPU;
    wxTextCtrl  *m_txtDisco;
    wxCheckBox  *m_chkFunciona;
    wxTextCtrl  *m_txtDescripcion;

    // --- Tabla ---
    wxListCtrl  *m_lista;

    // --- Lógica ---
    Inventario  *m_inventario;

    // --- Helpers ---
    void        ActualizarTabla(const std::vector<Ordenador> &lista);
    Ordenador   GetOrdenadorFormulario() const;
    void        LimpiarFormulario();
    void        RellenarFormulario(long row);

    // --- Eventos ---
    void OnAgregar(wxCommandEvent &);
    void OnEditar(wxCommandEvent &);
    void OnBorrar(wxCommandEvent &);
    void OnBuscar(wxCommandEvent &);
    void OnSeleccion(wxListEvent &);

    wxDECLARE_EVENT_TABLE();
};

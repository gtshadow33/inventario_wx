# Inventario de Ordenadores — wxWidgets

Migración completa del proyecto Qt a wxWidgets + SQLite3 puro (C API).

## Dependencias

| Librería   | Ubuntu/Debian                  | Windows (vcpkg)              |
|------------|-------------------------------|------------------------------|
| wxWidgets  | `sudo apt install libwxgtk3.2-dev` | `vcpkg install wxwidgets`  |
| SQLite3    | `sudo apt install libsqlite3-dev`  | `vcpkg install sqlite3`    |
| CMake ≥3.16| `sudo apt install cmake`           | incluido en VS / winget    |

## Compilar (Linux / macOS)

```bash
mkdir build && cd build
cmake ..
cmake --build . -j$(nproc)
./InventarioWx
```

## Compilar (Windows con vcpkg + CMake)

```powershell
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

## Estructura de archivos

```
inventario_wx/
├── CMakeLists.txt
├── main.cpp          # wxApp — punto de entrada
├── mainwindow.h/cpp  # Ventana principal (equivalente a mainwindow.h/cpp + .ui de Qt)
├── inventario.h/cpp  # CRUD lógica de negocio
├── database.h/cpp    # Wrapper SQLite3
└── ordenador.h       # Struct Ordenador (sin dependencias externas)
```

## Equivalencias Qt → wxWidgets

| Qt                    | wxWidgets                     |
|-----------------------|-------------------------------|
| `QMainWindow`         | `wxFrame`                     |
| `QWidget + layouts`   | `wxPanel + wxSizer`           |
| `QLineEdit`           | `wxTextCtrl`                  |
| `QSpinBox`            | `wxSpinCtrl`                  |
| `QCheckBox`           | `wxCheckBox`                  |
| `QTextEdit`           | `wxTextCtrl` con `wxTE_MULTILINE` |
| `QPushButton`         | `wxButton`                    |
| `QTableWidget`        | `wxListCtrl` (modo `wxLC_REPORT`) |
| `QGroupBox`           | `wxStaticBox + wxStaticBoxSizer` |
| `QMessageBox`         | `wxMessageBox`                |
| `QSqlDatabase/Query`  | SQLite3 C API directa         |
| `QString`             | `std::string` / `wxString`    |
| `QVector<T>`          | `std::vector<T>`              |
| Signals & Slots       | `wxBEGIN/END_EVENT_TABLE` + `EVT_*` |

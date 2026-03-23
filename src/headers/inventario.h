#pragma once
#include <vector>
#include <string>
#include "ordenador.h"
#include "database.h"

class Inventario {
public:
    explicit Inventario(Database *db);

    bool agregarOrdenador(const Ordenador &o);
    bool modificarOrdenador(const Ordenador &o);
    bool eliminarOrdenador(int id);
    std::vector<Ordenador> listarTodos();
    std::vector<Ordenador> buscar(const std::string &nombre,
                                  const std::string &grupo,
                                  int ram);
private:
    Database *m_db;

    // Rellena un Ordenador desde una sentencia preparada (columnas 0-8: id,nombre,grupo,cpu,ram,gpu,disco,funciona,descripcion)
    static Ordenador fromStmt(sqlite3_stmt *stmt);
};

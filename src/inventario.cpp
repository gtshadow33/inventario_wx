#include "./headers/inventario.h"
#include <sqlite3.h>
#include <cstdio>
#include <cstring>

Inventario::Inventario(Database *db) : m_db(db) {}

// ---------- helper ----------
Ordenador Inventario::fromStmt(sqlite3_stmt *stmt) {
    Ordenador o;
    o.id          = sqlite3_column_int(stmt, 0);
    auto txt = [&](int col) -> std::string {
        const unsigned char *t = sqlite3_column_text(stmt, col);
        return t ? reinterpret_cast<const char*>(t) : "";
    };
    o.nombre      = txt(1);
    o.grupo       = txt(2);
    o.cpu         = txt(3);
    o.ram         = sqlite3_column_int(stmt, 4);
    o.gpu         = txt(5);
    o.disco       = txt(6);
    o.funciona    = sqlite3_column_int(stmt, 7) == 1;
    o.descripcion = txt(8);
    return o;
}

// ---------- Agregar ----------
bool Inventario::agregarOrdenador(const Ordenador &o) {
    if (!o.isValid()) return false;

    const char *sql =
        "INSERT INTO ordenadores (nombre,grupo,cpu,ram,gpu,disco,funciona,descripcion) "
        "VALUES (?,?,?,?,?,?,?,?);";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(m_db->getDB(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        fprintf(stderr, "agregarOrdenador prepare: %s\n", sqlite3_errmsg(m_db->getDB()));
        return false;
    }

    sqlite3_bind_text(stmt, 1, o.nombre.c_str(),      -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, o.grupo.c_str(),       -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, o.cpu.c_str(),         -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (stmt, 4, o.ram);
    sqlite3_bind_text(stmt, 5, o.gpu.c_str(),         -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, o.disco.c_str(),       -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (stmt, 7, o.funciona ? 1 : 0);
    sqlite3_bind_text(stmt, 8, o.descripcion.c_str(), -1, SQLITE_TRANSIENT);

    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    if (!ok) fprintf(stderr, "agregarOrdenador step: %s\n", sqlite3_errmsg(m_db->getDB()));
    sqlite3_finalize(stmt);
    return ok;
}

// ---------- Modificar ----------
bool Inventario::modificarOrdenador(const Ordenador &o) {
    if (!o.isValid() || o.id == 0) return false;

    const char *sql =
        "UPDATE ordenadores SET nombre=?,grupo=?,cpu=?,ram=?,gpu=?,disco=?,funciona=?,descripcion=? "
        "WHERE id=?;";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(m_db->getDB(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        fprintf(stderr, "modificarOrdenador prepare: %s\n", sqlite3_errmsg(m_db->getDB()));
        return false;
    }

    sqlite3_bind_text(stmt, 1, o.nombre.c_str(),      -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, o.grupo.c_str(),       -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, o.cpu.c_str(),         -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (stmt, 4, o.ram);
    sqlite3_bind_text(stmt, 5, o.gpu.c_str(),         -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, o.disco.c_str(),       -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (stmt, 7, o.funciona ? 1 : 0);
    sqlite3_bind_text(stmt, 8, o.descripcion.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (stmt, 9, o.id);

    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    if (!ok) fprintf(stderr, "modificarOrdenador step: %s\n", sqlite3_errmsg(m_db->getDB()));
    sqlite3_finalize(stmt);
    return ok;
}

// ---------- Eliminar ----------
bool Inventario::eliminarOrdenador(int id) {
    const char *sql = "DELETE FROM ordenadores WHERE id=?;";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(m_db->getDB(), sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, id);
    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return ok;
}

// ---------- Listar todos ----------
std::vector<Ordenador> Inventario::listarTodos() {
    std::vector<Ordenador> lista;
    const char *sql = "SELECT id,nombre,grupo,cpu,ram,gpu,disco,funciona,descripcion FROM ordenadores;";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(m_db->getDB(), sql, -1, &stmt, nullptr) != SQLITE_OK) return lista;
    while (sqlite3_step(stmt) == SQLITE_ROW)
        lista.push_back(fromStmt(stmt));
    sqlite3_finalize(stmt);
    return lista;
}

// ---------- Buscar ----------
std::vector<Ordenador> Inventario::buscar(const std::string &nombre,
                                           const std::string &grupo,
                                           int ram)
{
    std::vector<Ordenador> lista;

    // Construir SQL dinámicamente con OR entre las condiciones no vacías
    std::string sql = "SELECT id,nombre,grupo,cpu,ram,gpu,disco,funciona,descripcion FROM ordenadores";
    std::vector<std::string> conds;

    auto trimmed = [](const std::string &s) {
        size_t a = s.find_first_not_of(" \t\r\n");
        return (a == std::string::npos) ? std::string("") : s.substr(a);
    };

    bool hasNombre = !trimmed(nombre).empty();
    bool hasGrupo  = !trimmed(grupo).empty();
    bool hasRam    = ram > 0;

    if (hasNombre) conds.push_back("nombre LIKE ?");
    if (hasGrupo)  conds.push_back("grupo LIKE ?");
    if (hasRam)    conds.push_back("CAST(ram AS TEXT) LIKE ?");

    if (!conds.empty()) {
        sql += " WHERE ";
        for (size_t i = 0; i < conds.size(); ++i) {
            if (i) sql += " OR ";
            sql += conds[i];
        }
    }
    sql += ";";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(m_db->getDB(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        fprintf(stderr, "buscar prepare: %s\n", sqlite3_errmsg(m_db->getDB()));
        return lista;
    }

    int idx = 1;
    if (hasNombre) sqlite3_bind_text(stmt, idx++, ("%" + nombre + "%").c_str(), -1, SQLITE_TRANSIENT);
    if (hasGrupo)  sqlite3_bind_text(stmt, idx++, ("%" + grupo  + "%").c_str(), -1, SQLITE_TRANSIENT);
    if (hasRam)    sqlite3_bind_text(stmt, idx++, ("%" + std::to_string(ram) + "%").c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW)
        lista.push_back(fromStmt(stmt));

    sqlite3_finalize(stmt);
    return lista;
}

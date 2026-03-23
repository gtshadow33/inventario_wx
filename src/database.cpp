#include "./headers/database.h"
#include <cstdio>

Database::Database(const std::string &ruta) : m_ruta(ruta) {}

Database::~Database() {
    cerrar();
}

bool Database::abrir() {
    if (sqlite3_open(m_ruta.c_str(), &m_db) != SQLITE_OK) {
        fprintf(stderr, "Error al abrir DB: %s\n", sqlite3_errmsg(m_db));
        return false;
    }

    const char *sql =
        "CREATE TABLE IF NOT EXISTS ordenadores ("
        "id          INTEGER PRIMARY KEY AUTOINCREMENT,"
        "nombre      TEXT NOT NULL,"
        "grupo       TEXT NOT NULL,"
        "cpu         TEXT,"
        "ram         INTEGER,"
        "gpu         TEXT,"
        "disco       TEXT,"
        "funciona    INTEGER,"
        "descripcion TEXT);";

    char *errmsg = nullptr;
    if (sqlite3_exec(m_db, sql, nullptr, nullptr, &errmsg) != SQLITE_OK) {
        fprintf(stderr, "Error creando tabla: %s\n", errmsg);
        sqlite3_free(errmsg);
        return false;
    }
    return true;
}

void Database::cerrar() {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

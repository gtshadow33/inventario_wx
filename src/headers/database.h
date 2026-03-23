#pragma once
#include <string>
#include <sqlite3.h>

class Database {
public:
    explicit Database(const std::string &ruta = "inventario.db");
    ~Database();

    bool   abrir();
    void   cerrar();
    sqlite3* getDB() { return m_db; }

private:
    sqlite3*    m_db   = nullptr;
    std::string m_ruta;
};

#pragma once
#include <string>

struct Ordenador {
    int         id        = 0;
    std::string nombre;
    std::string grupo;
    std::string cpu;
    int         ram       = 0;
    std::string gpu;
    std::string disco;
    bool        funciona  = true;
    std::string descripcion;

    bool isValid() const {
        // Nombre y grupo son obligatorios
        auto trim = [](const std::string &s) {
            size_t a = s.find_first_not_of(" \t\r\n");
            return (a == std::string::npos) ? "" : s.substr(a);
        };
        return !trim(nombre).empty() && !trim(grupo).empty();
    }
};

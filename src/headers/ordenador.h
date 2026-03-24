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

    bool isValid() const ;
        
};

#include "./headers/ordenador.h"
#include <cctype>

bool Ordenador::isValid() const {
    auto trim = [](const std::string &s) {
        size_t a = s.find_first_not_of(" \t\r\n");
        return (a == std::string::npos) ? "" : s.substr(a);
    };

    auto wordCount = [](const std::string& s) {
        int count = 0;
        bool inWord = false;
        for (char c : s) {
            if (std::isspace(static_cast<unsigned char>(c))) {
                inWord = false;
            } else if (!inWord) {
                inWord = true;
                count++;
            }
        }
        return count;
    };

    return !trim(nombre).empty() &&
           !trim(grupo).empty() &&
           wordCount(descripcion) <= 30;
}
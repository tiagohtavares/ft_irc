// Função para dividir uma string com base em um delimitador `std::string`

#include "../../includes/Server.hpp"

std::vector<std::string> split(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != std::string::npos) {
        token = str.substr(start, end - start);
        tokens.push_back(token);
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }

    // Adicionar o último token
    token = str.substr(start);
    tokens.push_back(token);

    return tokens;
}
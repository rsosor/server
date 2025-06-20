#include <fstream>
#include <cstdlib>
#include <string>
#include <iostream>
#include <unordered_map>

std::unordered_map<std::string, std::string> load_env(const std::string& filename = ".env") {
    std::unordered_map<std::string, std::string> env_ump;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open .env file.\n";
        return env_ump;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        auto eq_pos = line.find('=');
        if (eq_pos == std::string::npos) continue;

        std::string key = line.substr(0, eq_pos);
        std::string value = line.substr(eq_pos + 1);

        // 去除前後空白
        key.erase(0, key.find_first_not_of(" \t\r\n"));
        key.erase(key.find_last_not_of(" \t\r\n") + 1);
        value.erase(0, value.find_first_not_of(" \t\r\n"));
        value.erase(value.find_last_not_of(" \t\r\n") + 1);

        env_ump[key] = value;
    }
    return env_ump;
}

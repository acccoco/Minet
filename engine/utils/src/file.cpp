
#include "../file.h"

#include <sstream>
#include <fstream>
#include <exception>

#include <spdlog/spdlog.h>


std::string File::file_load_str(const std::string &file_name) {
    std::ifstream fs;
    std::stringstream ss;

    SPDLOG_INFO("open file: {}", file_name);

    fs.open(file_name, std::ios::in);
    if (!fs.is_open()) {
        SPDLOG_ERROR("fail to open file: {}", file_name);
        throw std::exception();
    }

    ss << fs.rdbuf();
    fs.close();
    if (ss.str().empty()) {
        SPDLOG_ERROR("file empty: {}", file_name);
        throw std::exception();
    }

    return ss.str();
}


std::vector<std::string> File::file_load_lines(const std::string &file_name) {
    std::ifstream fs;
    std::string buffer;
    std::vector<std::string> lines;

    SPDLOG_INFO("open file: {}", file_name);
    fs.open(file_name, std::ios::in);
    if (!fs.is_open()) {
        SPDLOG_ERROR("failto open file: {}", file_name);
        throw std::exception();
    }

    while (std::getline(fs, buffer)) {
        lines.push_back(buffer + "\n");
    }

    return lines;
}

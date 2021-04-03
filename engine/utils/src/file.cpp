
#include "../file.h"

#include <sstream>
#include <exception>

#include <easylogging++.h>


std::string File::str_load(const std::string &file_name) {
    std::ifstream fs;
    std::stringstream ss;

    LOG(INFO) << "open file: " << file_name;

    fs.open(file_name, std::ios::in);
    if (!fs.is_open()) {
        LOG(ERROR) << "fail to open file: " << file_name;
        throw std::exception();
    }

    ss << fs.rdbuf();
    fs.close();
    if (ss.str().empty()) {
        LOG(ERROR) << "file empty: " << file_name;
        throw std::exception();
    }

    return ss.str();
}

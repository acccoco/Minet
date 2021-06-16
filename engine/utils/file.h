#ifndef RENDER_FILE_H
#define RENDER_FILE_H

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <exception>

#include <spdlog/spdlog.h>

class File {
public:
    /* 从文本文件中读取内容，返回字符串 */
    static std::string file_load_str(const std::string &file_name) {
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

    /* 从文本文件中读取内容，按行返回字符串的数组，每行都包括末尾的 \n */
    static std::vector<std::string> file_load_lines(const std::string &file_name) {
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
};


#endif //RENDER_FILE_H

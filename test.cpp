#include <vector>
#include <string>
#include <iostream>

#include <spdlog/spdlog.h>

class Face {
public:
    unsigned int a;
    unsigned int b;
    unsigned int c;

    Face() = default;
};


int main()
{
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%H:%M:%S][%^%L%$][%s:%#][%!] %v");

    std::vector<Face> faces;

    faces.push_back(Face{1, 2, 3});
    faces.push_back(Face{4, 5, 6});

    SPDLOG_INFO("sizeof Face: {}", sizeof(Face));

    SPDLOG_INFO("5th: {}", ((unsigned int*)(&faces[0]))[4]);
}

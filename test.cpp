#include <vector>
#include <string>
#include <iostream>

#include <spdlog/spdlog.h>



int main()
{
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%H:%M:%S][%^%L%$][%s:%#][%!] %v");

    SPDLOG_INFO("file: {}, dir: {}", __FILE__, __BASE_FILE__);
}


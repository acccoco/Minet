#include <map>
#include <vector>
#include <string>
#include <iostream>

#include <spdlog/spdlog.h>

class A {
public:
    int k;

    A(): k(233) {}
};


int main() {
    std::map<int, A> m;
    auto c = m[1];
    std::cout << c.k << std::endl;
}


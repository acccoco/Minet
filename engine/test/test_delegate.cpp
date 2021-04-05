#include <vector>
#include <iostream>

#include "../delegate.h"


class A {
public:
    void a(float x, float y) {
        std::cout << "A::a" << x << y << std::endl;
    }
};

class C {
public:
    void c(float x, float y) {
        std::cout << "C::c" << x << y << std::endl;
    }
};

void b(float x, float y) {
    std::cout << "b" << x << y << std::endl;
}

class D {
public:
    static void d(float x, float y) {
        std::cout << "D::d" << x << y << std::endl;
    }
};


int main()
{
    auto a = std::make_shared<A>();
    auto c = std::make_shared<C>();

    DelegateList<float, float> delegates;
    delegates.add(new FuncDelegate<float, float>(b));
    delegates.add(new MethodDelegate<A, float, float>(a, &A::a));
    delegates.add(new FuncDelegate<float, float>(b));
    delegates.add(new FuncDelegate<float, float>(b));
    delegates.add(new MethodDelegate<A, float, float>(a, &A::a));
    delegates.add(new MethodDelegate<C, float, float>(c, &C::c));
    delegates.add(new FuncDelegate<float, float>(D::d));

    delegates.invoke(12, 13);
}

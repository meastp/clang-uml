#pragma once

namespace clanguml {
namespace t00047 {

template <typename T> class A {
public:
    A() = delete;

    ~A() = default;

    A(int a_, T b_)
        : a(a_)
        , b(b_){};

    int a;
    T b;
};

}
}
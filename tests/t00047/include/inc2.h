#pragma once

#include "inc1.h"

namespace clanguml {
namespace t00047 {

template class A<double *>;

class B : public A<double *> {
public:
    B(int a_, double *b_);
};

B::B(int a_, double *b_)
    : A<double *>(a_, b_)
{
}

}
}
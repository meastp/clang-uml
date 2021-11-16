namespace clanguml {
namespace t20003 {

template <typename T, typename N = bool> T m3(T t, N n)
{
    if (n)
        return t;

    return {};
}

template <typename T> T m2(T t) { return m3(t, true); }

int m1() { return m2(2); }

}
}

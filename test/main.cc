#include <iostream>
#include "SuperString.hh"

int main(int argc, char const *argv[]) {
    SuperString s1 = SuperString::Const("bla");
    SuperString s2 = SuperString::Const("kla");
    SuperString s = s1 + s2 + s1;

    std::cout << s << "\n"; // blaklabla
    std::cout << s.substring(2, 9).ok() << "\n"; // aklabla
    return 0;
}

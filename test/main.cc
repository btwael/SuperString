#include <iostream>
#include "SuperString.hh"

SuperString func() {
    return SuperString::Const(" wael  ");
}

int main(int argc, char const *argv[])
{
    SuperString string = func();
    string = string * 2;
    std::cout << "-" << string.substring(1, 14).ok().trim() << "-";
    return 0;
}

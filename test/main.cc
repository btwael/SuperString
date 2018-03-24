#include <iostream>
#include "SuperString.hh"

SuperString func() {
    return SuperString::Const(" wael  ");
}

int main(int argc, char const *argv[])
{
    SuperString string = SuperString::Const("  Приве́т नमस्ते שָׁלוֹם  ", SuperString::Encoding::UTF8);
    SuperString string1 = SuperString::Const("wadel");
    std::cout << string.trim();
    return 0;
}

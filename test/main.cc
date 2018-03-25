#include <iostream>
#include "SuperString.hh"

SuperString func() {
    return SuperString::Const(" wael  ");
}

int main(int argc, char const *argv[]) {
    SuperString::Byte i[] = {0x62, 0x6c, 0xc3, 0xa5, 0x62, 0xc3, 0xa6, 0x72, 0x67, 0x72, 0xc3, 0xb8, 0x64, 0x00};
    SuperString string = SuperString::Const(
            (char *) i,
            SuperString::Encoding::ASCII);
    SuperString string1 = SuperString::Const("wadel");
    std::cout << string.trim();
    return 0;
}

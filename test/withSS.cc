#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "../src/SuperString.cc"

int main() {
    char *content;
    // file reading
    FILE *f = fopen("/Users/btwael/Downloads/longtextfile.txt", "r+");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    content = (char *) malloc(fsize + 1);
    fread(content, fsize, 1, f);
    fclose(f);

    SuperString string = SuperString::Copy(content, SuperString::Encoding::ASCII);
    free(content);

    std::vector<SuperString> lines;
    std::size_t last = 0;
    for(std::size_t i = 0, length = string.length(); i < length; i++) {
        int c = string.codeUnitAt(i).ok();
        if(c == '\n') {
            lines.push_back(string.substring(last, i).ok());
            last = i + 1;
        }
    }

    return 0;
}

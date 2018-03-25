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

    SuperString string = SuperString::Const(content, SuperString::Encoding::ASCII);

    std::vector<SuperString> lines;
    SuperString::Size last = 0;
    for(SuperString::Size i = 0; i < string.length(); i++) {
        int c = string.codeUnitAt(i).ok();
        if(c == '\n') {
            lines.push_back(string.substring(last, i).ok());
            last = i;
        }
    }

    // show each line
    //std::cout << lines.size();
    //for(SuperString::Size i = 0; i < lines.size(); i++) {
        //std::cout << lines.at(i);
    //}

    // free content
    free(content);
    return 0;
}

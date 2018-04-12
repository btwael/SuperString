#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>

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

    std::string string(content);
    free(content);

    std::vector<std::string> lines;
    unsigned long last = 0;
    for(unsigned long i = 0, length = string.size(); i < length; i++) {
        int c = string.at(i);
        if(c == '\n') {
            lines.push_back(string.substr(last, i - last));
            last = i + 1;
        }
    }

    return 0;
}

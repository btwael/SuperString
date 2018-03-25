#include <benchmark/benchmark.h>

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <string>

#include "SuperString.hh"

static void SplitToLines_SuperString(benchmark::State& state) {
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
    free(content);

    std::vector<SuperString> lines;
    SuperString::Size last = 0;
    for(SuperString::Size i = 0; i < string.length(); i++) {
        int c = string.codeUnitAt(i).ok();
        if(c == '\n') {
            lines.push_back(string.substring(last, i).ok());
            last = i;
        }
    }
}
// Register the function as a benchmark
BENCHMARK(SplitToLines_SuperString);

// Define another benchmark
static void SplitToLines_std_String(benchmark::State& state) {
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
    for(unsigned long i = 0; i < string.size(); i++) {
        int c = string.at(i);
        if(c == '\n') {
            lines.push_back(string.substr(last, last - i));
            last = i;
        }
    }
}
BENCHMARK(SplitToLines_std_String);

BENCHMARK_MAIN();

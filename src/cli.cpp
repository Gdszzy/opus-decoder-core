#include "core.h"
#include <cstdio>
#include <fstream>
#include <iostream>

int main(int argc, char **argv) {
  if(argc != 3) {
    std::cerr << "Invalid argc" << std::endl;
    return 1;
  }
  std::ifstream input(argv[1], std::ios::binary);
  if(!input) {
    std::cerr << "Failed to open input file" << std::endl;
    return 1;
  }
  std::ofstream output(argv[2]);
  if(!output) {
    std::cerr << "Failed to open output file" << std::endl;
    return 1;
  }
  input.seekg(0, std::ios::end);
  std::streampos length = input.tellg();
  input.seekg(0, std::ios::beg);
  int ret = decode(2, 16000, 20, 8, input, length, output);
  if(ret) {
    std::cerr << "Decode failed" << std::endl;
    return ret;
  }
  return 0;
}
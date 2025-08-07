#include "core.h"
#include <chrono>
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
  long long startTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::system_clock::now().time_since_epoch())
                            .count();
  int ret = decode(2, 16000, 20, 8, input, length, output);
  if(ret) {
    std::cerr << "Decode failed" << std::endl;
    return ret;
  }
  long long endTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                          std::chrono::system_clock::now().time_since_epoch())
                          .count();
  std::cout << "time used: " << endTime - startTime << std::endl;
  return 0;
}
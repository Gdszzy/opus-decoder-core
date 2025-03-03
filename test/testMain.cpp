#include "core.h"
#include <cstdio>
#include <fstream>

int main() {
  std::ifstream input("/home/ghost/文档/xwechat_files/wxid_s88qoerexckx22_851a/"
                      "msg/file/2025-03/test_2.0.0_audio.data-s0",
                      std::ios::binary);
  if(!input) {
    std::cerr << "Failed to open input file" << std::endl;
    return 1;
  }
  std::ofstream output("out.wav");

  input.seekg(0, std::ios::end);
  std::streampos length = input.tellg();
  input.seekg(0, std::ios::beg);
  decode(2, 16000, 20, 8, input, length, output);
  return 0;
}
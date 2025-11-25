#include "CLI/CLI.hpp"
#include "core.h"
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <unistd.h>

int main(int argc, char **argv) {
  CLI::App app{"Opus decoder cli"};

  std::string inputPath;
  std::string outputPath;
  int stdinLength = 0;
  bool use_stdout = false;

  auto in_group =
      app.add_option_group("input", "Input options")->require_option(1);
  in_group->add_option("-i", inputPath, "Input from file");
  in_group->add_option("--is", stdinLength,
                       "Input from stdin. Need provide file length");

  auto out_group =
      app.add_option_group("output", "Output options")->require_option(1);
  out_group->add_option("-o", outputPath, "Output to file");
  out_group->add_flag("--os", use_stdout, "Output to stdout");

  CLI11_PARSE(app, argc, argv);

  std::istream *inputStream;
  std::ifstream inputFile;
  if (stdinLength) {
    inputStream = &std::cin;
  } else {
    inputFile.open(inputPath, std::ios::binary);
    if (!inputFile) {
      std::cerr << "Failed to open source file" << std::endl;
      return 1;
    }
    inputStream = &inputFile;
  }

  std::ostream *outputStream;
  std::ofstream outputFileStream;
  if (use_stdout) {
    outputStream = &std::cout;
  } else {
    outputFileStream.open(outputPath, std::ios::binary);
    if (!outputFileStream) {
      std::cerr << "Failed to open output file" << std::endl;
      return 1;
    }
    outputStream = &outputFileStream;
  }

  std::streampos length = -1;
  if (stdinLength) {
    length = stdinLength;
  } else {
    inputFile.seekg(0, std::ios::end);
    length = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);
  }

  int ret = decode(2, 16000, 20, 8, *inputStream, length, *outputStream);
  if (ret) {
    std::cerr << "Decode failed" << std::endl;
    return ret;
  }
  return 0;
}
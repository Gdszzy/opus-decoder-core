#include "CLI/CLI.hpp"
#include "core.h"
#include <cstdint>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#ifdef _WIN32
#include <windows.h> 
#else
#include <unistd.h>
#endif

int main(int argc, char **argv) {
  CLI::App app{"Opus decoder cli"};

  std::string inputPath;
  std::string outputPath;
  std::string outputFormat = "wav";
  uint32_t inputSize = 0;

  app.add_option("-i,--input", inputPath, "Input opus path, or '-' for stdin")
      ->required();
  app.add_option("-o,--output", outputPath, "Output path, or '-' for stdout")
      ->required();
  app.add_option("-f,--format", outputFormat, "Output format")
      ->check(CLI::IsMember({"wav", "pcm"}))
      ->capture_default_str();
  auto input_size_option = app.add_option(
      "--input-size", inputSize,
      "Input size in bytes. Required for stdin input with wav output");

  CLI11_PARSE(app, argc, argv);

  bool use_stdin = inputPath == "-";
  bool use_stdout = outputPath == "-";
  bool writeWavHeader = outputFormat == "wav";
  if (use_stdin && writeWavHeader && input_size_option->count() == 0) {
    std::cerr << "--input-size is required when reading stdin with wav output"
              << std::endl;
    return 1;
  }

  std::istream *inputStream;
  std::ifstream inputFile;
  if (use_stdin) {
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
  if (use_stdin) {
    length = inputSize;
  } else {
    inputFile.seekg(0, std::ios::end);
    length = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);
  }

  int ret = decode(2, 16000, 20, 8, *inputStream, length, *outputStream,
                   writeWavHeader);
  if (ret) {
    std::cerr << "Decode failed" << std::endl;
    return ret;
  }
  return 0;
}

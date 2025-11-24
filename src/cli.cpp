#include "CLI/CLI.hpp"
#include "core.h"
#include <chrono>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <unistd.h>

int main(int argc, char **argv) {
  CLI::App app{"Opus decoder cli"};

  std::string sourceFile;
  std::string outputFile;
  bool use_stdout = false;

  app.add_option("-s,--source", sourceFile, "Source file")->required();
  auto opt_group =
      app.add_option_group("output", "Output options")->require_option(1);
  opt_group->add_option("-o,--output", outputFile, "Output file");
  opt_group->add_flag("-d", use_stdout, "Directly output to stdout");

  CLI11_PARSE(app, argc, argv);

  std::ifstream input(sourceFile, std::ios::binary);
  if (!input) {
    std::cerr << "Failed to open source file" << std::endl;
    return 1;
  }

  int out_fd = -1;

  if (use_stdout) {
    out_fd = STDOUT_FILENO;
  } else {
    out_fd = open(outputFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out_fd == -1) {
      perror("open");
      return 1;
    }
  }

  input.seekg(0, std::ios::end);
  std::streampos length = input.tellg();
  input.seekg(0, std::ios::beg);

  int ret = decode(2, 16000, 20, 8, input, length, out_fd);
  if (ret) {
    std::cerr << "Decode failed" << std::endl;
    if (!use_stdout) {
      close(out_fd);
    }
    return ret;
  }
  if (!use_stdout) {
    close(out_fd);
  }
  return 0;
}
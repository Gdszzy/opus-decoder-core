#include "CLI/CLI.hpp"
#include "core.h"
#include <CLI/CLI.hpp>
#include <chrono>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

int main(int argc, char **argv) {
  CLI::App app{"Opus decoder cli"};

  std::string sourceFile;
  std::string outputFile;
  std::string ffmpeg_args_str;

  app.add_option("-s,--source", sourceFile, "Source file")->required();
  app.add_option("-o,--output", outputFile, "Output file");
  app.add_option("-f,--ffmpeg", ffmpeg_args_str,
                 "FFMPEG parameters as a single string");

  CLI11_PARSE(app, argc, argv);

  std::ifstream input(sourceFile, std::ios::binary);
  if (!input) {
    std::cerr << "Failed to open source file" << std::endl;
    return 1;
  }

  int out_fd = -1;
  pid_t ffmpeg_pid = -1;
  bool use_ffmpeg = !ffmpeg_args_str.empty();

  if (use_ffmpeg) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
      perror("pipe");
      return 1;
    }

    ffmpeg_pid = fork();
    if (ffmpeg_pid == -1) {
      perror("fork");
      return 1;
    }

    if (ffmpeg_pid == 0) { // Child process
      close(pipefd[1]);    // Close write end
      if (dup2(pipefd[0], STDIN_FILENO) == -1) {
        perror("dup2");
        exit(EXIT_FAILURE);
      }
      close(pipefd[0]);

      // Split the ffmpeg arguments string by whitespace
      std::istringstream iss(ffmpeg_args_str);
      std::vector<std::string> ffmpeg_args(
          std::istream_iterator<std::string>{iss},
          std::istream_iterator<std::string>());

      std::vector<char *> c_args;
      c_args.push_back(const_cast<char *>("ffmpeg"));
      // Add hardcoded parameters for raw PCM input
      c_args.push_back(const_cast<char *>("-f"));
      c_args.push_back(const_cast<char *>("s16le"));
      c_args.push_back(const_cast<char *>("-ar"));
      c_args.push_back(const_cast<char *>("16000"));
      c_args.push_back(const_cast<char *>("-ac"));
      c_args.push_back(const_cast<char *>("2"));
      c_args.push_back(const_cast<char *>("-i"));
      c_args.push_back(const_cast<char *>("-"));
      for (const auto &arg : ffmpeg_args) {
        c_args.push_back(const_cast<char *>(arg.c_str()));
      }
      c_args.push_back(nullptr);

      execvp("ffmpeg", c_args.data());
      // execvp only returns on error
      perror("execvp");
      exit(EXIT_FAILURE);
    } else {            // Parent process
      close(pipefd[0]); // Close read end
      out_fd = pipefd[1];
    }
  } else {
    if (outputFile.empty()) {
      std::cerr << "Output file is required when not using --ffmpeg"
                << std::endl;
      return 1;
    }
    out_fd = open(outputFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out_fd == -1) {
      perror("open");
      return 1;
    }
  }

  input.seekg(0, std::ios::end);
  std::streampos length = input.tellg();
  input.seekg(0, std::ios::beg);
  long long startTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::system_clock::now().time_since_epoch())
                            .count();
  int ret = decode(2, 16000, 20, 8, input, length, use_ffmpeg, out_fd);
  if (ret) {
    std::cerr << "Decode failed" << std::endl;
    close(out_fd);
    return ret;
  }
  close(out_fd);

  if (use_ffmpeg) {
    int status;
    waitpid(ffmpeg_pid, &status, 0);
  }

  long long endTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                          std::chrono::system_clock::now().time_since_epoch())
                          .count();
  std::cout << "time used: " << endTime - startTime << std::endl;
  return 0;
}
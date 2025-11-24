#pragma once

#include <cstdint>
#include <iostream>

int decode(int channel, int sampleRate, int frameSizeMs, int frameRate,
           std::istream &reader, uint32_t size, bool raw_pcm, int out_fd);

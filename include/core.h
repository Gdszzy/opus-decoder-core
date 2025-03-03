#ifndef __CORE_H
#define __CORE_H

#include <iostream>
#include <cstdint>

int decode(int channel, int sampleRate, int frameSizeMs, int frameRate, std::istream &reader, uint32_t size, std::ostream &writer);

#endif // __CORE_H
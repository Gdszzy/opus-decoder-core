#include "core.h"
#include "opus.h"
#include <cstdint>
#include <cstring>
#include <memory>
#include <unistd.h>
#include <vector>

using WavHeader = uint8_t[44];

#define FILL_ARR(arr, ...)                                                     \
  {                                                                            \
    uint8_t values[] = {__VA_ARGS__};                                          \
    for (size_t i = 0; i < sizeof(arr) / sizeof(arr[0]); ++i) {                \
      arr[i] = values[i];                                                      \
    }                                                                          \
  }

void createWavHeader(WavHeader &h, uint32_t size, int channel, int sampleRate,
                     int format) {
  uint32_t totalDataLen = size + 36;
  uint32_t bitRate = channel * sampleRate * format;
  uint32_t byteRate = bitRate / 8;

  FILL_ARR(h, 'R', 'I', 'F', 'F', (uint8_t)(totalDataLen & 0xFF),
           (uint8_t)((totalDataLen >> 8) & 0xFF),
           (uint8_t)((totalDataLen >> 16) & 0xFF),
           (uint8_t)((totalDataLen >> 24) & 0xFF), 'W', 'A', 'V', 'E', 'f', 'm',
           't', ' ', (uint8_t)(format), 0, 0, 0, 1, 0, (uint8_t)(channel), 0,
           (uint8_t)(sampleRate & 0xFF), (uint8_t)((sampleRate >> 8) & 0xFF),
           (uint8_t)((sampleRate >> 16) & 0xFF),
           (uint8_t)((sampleRate >> 24) & 0xFF), (uint8_t)(byteRate & 0xFF),
           (uint8_t)((byteRate >> 8) & 0xFF),
           (uint8_t)((byteRate >> 16) & 0xFF),
           (uint8_t)((byteRate >> 24) & 0xFF),
           (uint8_t)((channel * format) / 8), 0, 16, 0, 'd', 'a', 't', 'a',
           (uint8_t)(size & 0xFF), (uint8_t)((size >> 8) & 0xFF),
           (uint8_t)((size >> 16) & 0xFF), (uint8_t)((size >> 24) & 0xFF));
}

void onOpusDecoderDelete(OpusDecoder *decoder) {
  opus_decoder_destroy(decoder);
}

int decode(int channel, int sampleRate, int frameSizeMs, int frameRate,
           std::istream &reader, uint32_t size, bool raw_pcm, int out_fd) {
  int pcmFrameSize =
      channel * sampleRate * frameSizeMs * 2 / 1000; // 1280 bytes = 640 uint16
  int opusFrameSize = pcmFrameSize / frameRate;
  int opusChannelSize = opusFrameSize / 2;
  int frameNumber = size / opusFrameSize;

  int decodedFrameNumber = pcmFrameSize / 4;

  std::vector<int16_t> pcmLeft(pcmFrameSize / 4); // 640 bytes = 320 uint16
  std::vector<int16_t> pcmRight(pcmFrameSize / 4);
  std::vector<int16_t> pcm(pcmFrameSize / 2);

  std::vector<uint8_t> opusBuffer(opusChannelSize);

  // write header
  if (!raw_pcm) {
    WavHeader header;
    createWavHeader(header, frameNumber * pcmFrameSize, channel, sampleRate,
                    16);
    write(out_fd, (char *)header, sizeof(header));
  }

  int error;
  // left
  OpusDecoder *leftDecPtr = opus_decoder_create(sampleRate, 1, &error);
  if (error != OPUS_OK) {
    return -1;
  }
  // wrap with unique_ptr
  auto leftDec = std::unique_ptr<OpusDecoder, void (*)(OpusDecoder *)>(
      leftDecPtr, &onOpusDecoderDelete);
  // right
  OpusDecoder *rightDecPtr = opus_decoder_create(sampleRate, 1, &error);
  if (error != OPUS_OK) {
    return -1;
  }
  // wrap with unique_ptr
  auto rightDec = std::unique_ptr<OpusDecoder, void (*)(OpusDecoder *)>(
      rightDecPtr, &onOpusDecoderDelete);
  for (int i = 0; i < frameNumber; i++) {
    // left ===============================
    reader.read((char *)opusBuffer.data(), opusChannelSize);
    if (!reader) {
      return -2;
    }
    // fill the pcm fuffer with 0
    std::fill(pcmLeft.begin(), pcmLeft.end(), 0);
    // decode left. pcm frame will be all zero if failed. just ignore it
    std::ignore =
        opus_decode(leftDec.get(), opusBuffer.data(), opusBuffer.size(),
                    pcmLeft.data(), pcmLeft.size(), 0);

    // right ===============================
    reader.read((char *)opusBuffer.data(), opusChannelSize);
    if (!reader) {
      return -2;
    }
    // fill the pcm fuffer with 0
    std::fill(pcmRight.begin(), pcmRight.end(), 0);
    // decode right. pcm frame will be all zero if failed. just ignore it
    std::ignore =
        opus_decode(rightDec.get(), opusBuffer.data(), opusBuffer.size(),
                    pcmRight.data(), pcmRight.size(), 0);
    // concat
    for (int j = 0; j < decodedFrameNumber; j++) {
      pcm[j * 2] = pcmLeft[j];
      pcm[j * 2 + 1] = pcmRight[j];
    }
    write(out_fd, (char *)pcm.data(), pcmFrameSize);
  }
  return 0;
}
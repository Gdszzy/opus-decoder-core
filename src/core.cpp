#include "core.h"
#include "opus.h"
#include <cstring>
#include <functional>
#include <memory>
#include <vector>

using WavHeader = uint8_t[44];

#define FILL_ARR(arr, ...)                                                     \
  {                                                                            \
    uint8_t values[] = {__VA_ARGS__};                                          \
    for(size_t i = 0; i < sizeof(arr) / sizeof(arr[0]); ++i) {                 \
      arr[i] = values[i];                                                      \
    }                                                                          \
  }

void createWavHeader(WavHeader &h, uint32_t size, int channel, int sampleRate,
                     int format) {
  uint32_t totalDataLen = size + 36;
  uint32_t bitRate = channel * sampleRate * format;
  uint32_t byteRate = bitRate / 8;

  FILL_ARR(h, 'R', 'I', 'F', 'F', totalDataLen & 0xFF,
           (totalDataLen >> 8) & 0xFF, (totalDataLen >> 16) & 0xFF,
           (totalDataLen >> 24) & 0xFF, 'W', 'A', 'V', 'E', 'f', 'm', 't', ' ',
           format, 0, 0, 0, 1, 0, channel, 0, sampleRate & 0xFF,
           (sampleRate >> 8) & 0xFF, (sampleRate >> 16) & 0xFF,
           (sampleRate >> 24) & 0xFF, byteRate & 0xFF, (byteRate >> 8) & 0xFF,
           (byteRate >> 16) & 0xFF, (byteRate >> 24) & 0xFF,
           (channel * format) / 8, 0, 16, 0, 'd', 'a', 't', 'a', size & 0xFF,
           (size >> 8) & 0xFF, (size >> 16) & 0xFF, (size >> 24) & 0xFF);
}

void onOpusDecoderDelete(OpusDecoder *decoder) {
  opus_decoder_destroy(decoder);
}

int decode(int channel, int sampleRate, int frameSizeMs, int frameRate,
           std::istream &reader, uint32_t size, std::ostream &writer) {
  int pcmFrameSize =
      channel * sampleRate * frameSizeMs * 2 / 1000; // 1280字节 640个uint16
  int opusFrameSize = pcmFrameSize / frameRate;
  int opusChannelSize = opusFrameSize / 2;
  int frameNumber = size / opusFrameSize;

  std::vector<int16_t> pcmLeft(pcmFrameSize / 4); // 640字节 320个uint64
  std::vector<int16_t> pcmRight(pcmFrameSize / 4);
  std::vector<int16_t> pcm(pcmFrameSize / 2);

  std::vector<uint8_t> opusBuffer(opusChannelSize);

  // write header
  {
    WavHeader header;
    createWavHeader(header, frameNumber * pcmFrameSize, channel, sampleRate,
                    16);
    writer.write((char *)header, sizeof(header));
  }

  int error;
  // left
  OpusDecoder *leftDecPtr =
      opus_decoder_create(sampleRate, 1, &error); // 采样率48000Hz，立体声2通道
  if(error != OPUS_OK) {
    return -1;
  }
  // wrap
  auto leftDec = std::unique_ptr<OpusDecoder, void (*)(OpusDecoder *)>(
      leftDecPtr, &onOpusDecoderDelete);
  // right
  OpusDecoder *rightDecPtr = opus_decoder_create(sampleRate, 1, &error);
  if(error != OPUS_OK) {
    return -1;
  }
  // wrap
  auto rightDec = std::unique_ptr<OpusDecoder, void (*)(OpusDecoder *)>(
      rightDecPtr, &onOpusDecoderDelete);
  for(int i = 0; i < frameNumber; i++) {
    // left
    reader.read((char *)opusBuffer.data(), opusChannelSize);
    if(!reader) {
      return -2;
    }
    int decodedFrameSize =
        opus_decode(leftDec.get(), opusBuffer.data(), opusBuffer.size(),
                    pcmLeft.data(), pcmLeft.size(), 0);
    if(decodedFrameSize < 0) {
      // return decodedFrameSize;
      std::fill(pcmLeft.begin(), pcmLeft.end(), 0);
    }
    // right
    reader.read((char *)opusBuffer.data(), opusChannelSize);
    if(!reader) {
      return -2;
    }
    decodedFrameSize =
        opus_decode(rightDec.get(), opusBuffer.data(), opusBuffer.size(),
                    pcmRight.data(), pcmRight.size(), 0);
    if(decodedFrameSize < 0) {
      // return decodedFrameSize;
      std::fill(pcmRight.begin(), pcmRight.end(), 0);
    }
    // concat
    for(int j = 0; j < decodedFrameSize; j++) {
      pcm[j * 2] = pcmLeft[j];
      pcm[j * 2 + 1] = pcmRight[j];
    }
    // write
    writer.write((char *)pcm.data(), pcmFrameSize);
  }
  return 0;
}
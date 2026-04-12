#include "core/audio.h"
#include "core/log.h"

#include <AL/al.h>
#include <AL/alc.h>

#include <fstream>
#include <iostream>

namespace {
void LoadWAV(const char* filename, std::vector<char>& data, ALenum& format,
             ALsizei& frequency) {
  std::ifstream file(filename, std::ios::binary);
  if (!file) {
    throw core::Error(
        std::format("Failed to open WAV file: {}", filename));
  }
  std::array<char, 4> chunk_id;
  std::array<char, 4> format_id;
  std::array<char, 4> subchunk_1_id;
  file.read(chunk_id.data(), 4);
  if (std::strncmp(chunk_id.data(), "RIFF", 4) != 0) {
  }
  file.ignore(4);  // Skip chunk size

  file.read(format_id.data(), 4);
  if (std::strncmp(format_id.data(), "WAVE", 4) != 0) {
    throw core::Error(
        std::format("Invalid WAV file (missing WAVE): {}", filename));
  }

  file.read(subchunk_1_id.data(), 4);
  if (std::strncmp(subchunk_1_id.data(), "fmt ", 4) != 0) {
    throw core::Error(
        std::format("Invalid WAV file (missing fmt ): {}", filename));
  }

  uint32_t subchunk_1_size = 0;
  file.read(reinterpret_cast<char*>(&subchunk_1_size), 4);

  uint16_t audio_format = 0;
  uint16_t channels = 0;
  uint16_t block_align = 0;
  uint16_t bits_per_sample = 0;
  uint32_t sample_rate = 0;
  uint32_t byte_rate = 0;
  file.read(reinterpret_cast<char*>(&audio_format), 2);
  file.read(reinterpret_cast<char*>(&channels), 2);
  file.read(reinterpret_cast<char*>(&sample_rate), 4);
  file.read(reinterpret_cast<char*>(&byte_rate), 4);
  file.read(reinterpret_cast<char*>(&block_align), 2);
  file.read(reinterpret_cast<char*>(&bits_per_sample), 2);

  if (audio_format != 1) {
    throw core::Error(std::format(
        "Unsupported WAV audio_format (only PCM supported): {}", filename));
  }
  if (subchunk_1_size > 16) {
    file.ignore(static_cast<std::streamsize>(subchunk_1_size) - 16);
  }

  char subchunk_2_id[4];
  uint32_t subchunk_2_size = 0;
  while (file.read(subchunk_2_id, 4)) {
    file.read(reinterpret_cast<char*>(&subchunk_2_size), 4);
    if (std::strncmp(subchunk_2_id, "data", 4) == 0) {
      break;
    }
    file.ignore(subchunk_2_size);
  }

  if (subchunk_2_size == 0) {
    throw core::Error(
        std::format("Invalid WAV file (missing data chunk): {}", filename));
  }

  data.resize(subchunk_2_size);
  file.read(data.data(), subchunk_2_size);

  if (bits_per_sample == 8) {
    format = (channels == 1) ? AL_FORMAT_MONO8 : AL_FORMAT_STEREO8;
  } else if (bits_per_sample == 16) {
    format = (channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
  } else {
    throw core::Error(std::format(
        "Unsupported WAV bit depth: {} in file {}", bits_per_sample, filename));
  }
  frequency = sample_rate;
}
}  // namespace

namespace core::audio {
ALCdevice* device = nullptr;
ALCcontext* context = nullptr;
std::map<unsigned int, Handle> loaded_map;
}  // namespace core::audio

core::audio::Handle::~Handle() {
  alDeleteSources(1, &source);
  alDeleteBuffers(1, &buffer);
}

void core::audio::Init() {
  device = alcOpenDevice(nullptr);
  if (device) {
    context = alcCreateContext(device, nullptr);
    alcMakeContextCurrent(context);
  }
  alGetError();
}

unsigned int core::audio::Load(const char* file) {
  std::vector<char> data;
  ALenum format;
  ALsizei freq;
  LoadWAV(file, data, format, freq);
  ALuint buffer;
  alGenBuffers(1, &buffer);
  alBufferData(buffer, format, data.data(), static_cast<ALsizei>(data.size()),
               freq);
  ALuint source = 0;
  alGenSources(1, &source);
  alSourcei(source, AL_BUFFER, buffer);
  alSourcei(source, AL_LOOPING, AL_FALSE);
  loaded_map.insert(
      {loaded_map.size(), Handle{.buffer = buffer, .source = source}});
  return loaded_map.end()->first;
}

void core::audio::Play(unsigned int handle) {
  if (!loaded_map.contains(handle)) {
    core::Log(std::format("Ignoring request to play nonexistent handle {}", handle), "Audio");
    return;
  }

  const auto& sound = loaded_map.at(handle);
  ALint state;
  alGetSourcei(sound.source, AL_SOURCE_STATE, &state);
  if (state != AL_PLAYING) {
    alSourcePlay(sound.source);
  }
}

void core::audio::Stop(unsigned int handle) {
  if (!loaded_map.contains(handle)) {
    core::Log(std::format("Ignoring request to play nonexistent handle {}", handle), "Audio");
    return;
  }

  const auto& sound = loaded_map.at(handle);
  ALint state;
  alGetSourcei(sound.source, AL_SOURCE_STATE, &state);
  if (state != AL_STOPPED) {
    alSourceStop(sound.source);
  }
}

void core::audio::Quit() {
  loaded_map.clear();
  alcMakeContextCurrent(nullptr);
  alcDestroyContext(context);
  alcCloseDevice(device);
  context = nullptr;
  device = nullptr;
}

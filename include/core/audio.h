#ifndef AUDIO_H
#define AUDIO_H
#include <AL/al.h>
#include <AL/alc.h>
namespace core {
namespace audio {
void Init();
unsigned int Load(const char* file);
void Play(unsigned int handle);
void Stop(unsigned int handle);
void Quit();
extern ALCdevice* device;
extern ALCcontext* context;
struct Handle {
  ALuint buffer;
  ALuint source;
  ~Handle();
};
extern std::map<unsigned int, Handle> loaded_map;
}  // namespace audio
}  // namespace core

#endif  // AUDIO_H

#ifndef DEGRADATION_H
#define DEGRADATION_H

namespace game {
  enum DegradationLevel {
    LEVEL_0, // Softening, artifacts
    LEVEL_1, // Washed color, log degrades
    LEVEL_2, // Vignetting, extreme blur. further log degradation
    LEVEL_3 // Silhouettes, dialogue degradation, only combat remains
  };
}

#endif // DEGRADATION_H
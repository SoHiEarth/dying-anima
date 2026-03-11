#ifndef EDITOR_ANIMATION_H
#define EDITOR_ANIMATION_H

#include "core/animation.h"

namespace editor {
void ShowAnimationWindow(bool show);
void AnimationWindow(Animation& animation);
namespace internal {
extern bool show_animation_window;
}  // namespace internal
}  // namespace editor

#endif  // EDITOR_ANIMATION_H

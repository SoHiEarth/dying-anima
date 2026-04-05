#ifndef TOOLTIP_H
#define TOOLTIP_H

namespace editor {
extern std::map<std::string, std::string> tooltip_map;
void LoadTooltips();
void SetTooltip(std::string_view tag);
}  // namespace editor

#endif  // TOOLTIP_H

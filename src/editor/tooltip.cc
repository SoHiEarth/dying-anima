#include "editor/tooltip.h"
#include <mutex>
#include <pugixml.hpp>
#include "core/path_resolve.h"

namespace editor {
std::map<std::string, std::string> tooltip_map;
}

namespace {
std::once_flag tooltips_initialized;
}

void editor::LoadTooltips() {
  pugi::xml_document doc;
  if (!doc.load_file((core::path::GetAssetPath() / "tooltip.xml").string().c_str())) {
    std::print("Failed to load tooltips: assets/tooltip.xml\n");
    return;
  }
  auto root = doc.child("tooltips");
  for (auto tooltip_node : root.children("tooltip")) {
    std::string tag = tooltip_node.attribute("tag").as_string();
    std::string text = tooltip_node.attribute("text").as_string();
    tooltip_map.insert(std::make_pair(tag, text));
  }
}

void editor::SetTooltip(std::string_view tag) {
  std::call_once(tooltips_initialized, LoadTooltips);
  if (ImGui::IsItemHovered()) {
    if (tooltip_map.contains(std::string(tag))) {
      ImGui::SetTooltip("%s", tooltip_map.at(std::string(tag)).c_str());
    }
  }
}

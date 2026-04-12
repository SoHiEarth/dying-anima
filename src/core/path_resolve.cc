#include "core/path_resolve.h"
#include <filesystem>
#include "core/log.h"

namespace {
std::vector<std::filesystem::path> fallback_paths{};
}

std::filesystem::path core::path::GetAssetPath() {
  if (std::filesystem::exists(std::filesystem::current_path() / "assets"))
    return std::filesystem::current_path() / "assets";

  for (const auto& fallback_path : GetFallbackPaths()) {
    if (std::filesystem::exists(fallback_path / "assets")) {
      return fallback_path / "assets";
    }
  }

  throw core::Error("Asset path not found in current directory or fallback paths", "Path Resolve");
}

std::vector<std::filesystem::path>& core::path::GetFallbackPaths() {
  return fallback_paths;
}

std::filesystem::path core::path::GetTempPath() {
  return std::filesystem::temp_directory_path();
}

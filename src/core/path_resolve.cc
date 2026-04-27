#include "core/path_resolve.h"

#include <filesystem>

#include "core/log.h"

#include <whereami.h>

namespace {
std::vector<std::filesystem::path> fallback_paths{};

std::once_flag add_path_flag;
}

std::filesystem::path core::path::GetAssetPath() {
  std::call_once(add_path_flag, [](){
    int length = wai_getExecutablePath(nullptr, 0, nullptr);
    std::string exec_path(length, '\0');
    wai_getExecutablePath(exec_path.data(), length, nullptr);

    std::filesystem::path exe_path(exec_path);
    std::filesystem::path exe_dir = exe_path.parent_path();

    fallback_paths.emplace_back(exe_dir);
  });
  if (std::filesystem::exists(std::filesystem::current_path() / "assets"))
    return std::filesystem::current_path() / "assets";

  for (const auto& fallback_path : GetFallbackPaths()) {
    if (std::filesystem::exists(fallback_path / "assets")) {
      return fallback_path / "assets";
    }
  }

  throw core::Error(
      "Asset path not found in current directory or fallback paths",
      "Path Resolve");
}

std::vector<std::filesystem::path>& core::path::GetFallbackPaths() {
  return fallback_paths;
}

std::filesystem::path core::path::GetTempPath() {
  return std::filesystem::temp_directory_path();
}

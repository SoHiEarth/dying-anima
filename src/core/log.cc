#include "core/log.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include "core/path_resolve.h"

namespace {
  std::fstream output_file;
  bool opened_file = false;
}

void core::Log(std::string_view content, std::string_view caller) {
  if (!opened_file) {
    if (!std::filesystem::exists(core::path::GetTempPath() / "log")) {
      std::filesystem::create_directory(core::path::GetTempPath() / "log");
    }
    output_file.open(core::path::GetTempPath() / "log/runtime.log", std::ios::out | std::ios::app);
    if (!output_file.is_open()) {
      std::cout << "[Log] Failed to open output file.";
    }
    opened_file = true;
  }

  const auto now = std::chrono::system_clock::now();
  auto formatted = std::format("[ {:%H:%M:%S} ] [ {} ] {}\n", now, caller, content);
  std::cout << formatted;
  if (output_file.is_open()) {
    output_file << formatted;
    output_file.flush();
  }
}

void core::Quit() {
  if (output_file.is_open()) {
    output_file.close();
  }
}

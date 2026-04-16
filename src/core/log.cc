#include "core/log.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <chrono>
#include "core/path_resolve.h"
#include <tinyfiledialogs.h>

namespace {
std::fstream output_file;
bool opened_file = false;

std::string FormatMessage(std::string_view content, std::string_view caller) {
  return std::format("[ {:%H:%M:%S} ] [ {} ] {}\n", std::chrono::system_clock::now(), caller, content);
}

void CheckFileOpen() {
  if (!opened_file) {
    if (!std::filesystem::exists(core::path::GetTempPath() / "log")) {
      std::filesystem::create_directory(core::path::GetTempPath() / "log");
    }
    output_file.open(core::path::GetTempPath() / "log/runtime.log", std::ios::out);
    if (!output_file.is_open()) {
      std::cout << FormatMessage(std::format("Failed to open output file {}", (core::path::GetTempPath() / "log/runtime.log").string()), "Log");
    }
    std::cout << FormatMessage(std::format("Opened output file {}", (core::path::GetTempPath() / "log/runtime.log").string()), "Log");
    opened_file = true;
  }
}

}

void core::Log(std::string_view content, std::string_view caller) {
  CheckFileOpen();
  auto formatted = FormatMessage(content, caller);
  std::cout << formatted;
  if (output_file.is_open()) {
    output_file << formatted;
    output_file.flush();
  }
}

core::Exception core::Error(std::string_view content, std::string_view caller) {
  CheckFileOpen();
  auto formatted = FormatMessage(content, caller);
  std::cout << formatted << std::endl;
  if (output_file.is_open()) {
    output_file << formatted;
    output_file.flush();
  }

  tinyfd_messageBox("Dying Anima Error", formatted.c_str(), "ok", "warning", 1);
  return Exception(std::format("{:%H:%M:%S}", std::chrono::system_clock::now()), content, caller);
}

void core::Quit() {
  if (output_file.is_open()) {
    output_file.close();
  }
}

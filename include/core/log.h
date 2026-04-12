#ifndef CORE_LOG_H
#define CORE_LOG_H

namespace core {
void Log(std::string_view content, std::string_view caller = "");
void Quit();
}  // namespace core

#endif  // CORE_LOG_H

#ifndef CORE_LOG_H
#define CORE_LOG_H

namespace core {

struct Exception : public std::exception {
  const char* what() const noexcept override { return message_.c_str(); }

  Exception(std::string_view timestamp, std::string_view content,
            std::string_view caller)
      : timestamp_(std::string(timestamp)),
        content_(std::string(content)),
        caller_(std::string(caller)) {
    message_ = "[ " + timestamp_ + " ] [ " + caller_ + "] " + content_;
  };

 private:
  std::string timestamp_;
  std::string content_;
  std::string caller_;
  std::string message_;
};

void Log(std::string_view content, std::string_view caller = "");
Exception Error(std::string_view content, std::string_view caller = "");
void Quit();
}  // namespace core

#endif  // CORE_LOG_H

#ifndef PATH_RESOLVE_H
#define PATH_RESOLVE_H

namespace core {
namespace path {
std::filesystem::path GetAssetPath();
std::vector<std::filesystem::path>& GetFallbackPaths();
std::filesystem::path GetTempPath();
}  // namespace path
}  // namespace core

#endif  // PATH_RESOLVE_H

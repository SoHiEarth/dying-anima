#include "atlas.h"
#include <print>
#include <filesystem>
#include <pugixml.hpp>
#include <format>
#include <stdexcept>

std::map<std::string, FontAtlasEntry> LoadFontAtlas(std::string_view filename) {
  std::map<std::string, FontAtlasEntry> font_atlas{};
  pugi::xml_document font_doc;
  pugi::xml_parse_result result = font_doc.load_file(filename.data());
  if (!result) {
    throw std::runtime_error(std::format("Failed to load font atlas XML: {}\n", result.description()));
  }
  pugi::xml_node fonts_node = font_doc.child("fonts");
  std::filesystem::path base_path = std::filesystem::path(filename).parent_path();
  for (pugi::xml_node font_node : fonts_node.children("font")) {
    FontAtlasEntry entry;
    auto tag = font_node.attribute("tag").as_string();
    entry.name = font_node.attribute("name").as_string();
    entry.file = (base_path / font_node.attribute("file").as_string()).string();
    font_atlas.insert({tag, entry});
  }
  std::print("- Font Atlas Result -\nLoaded {} fonts from atlas.\n", font_atlas.size());
  for (const auto &[tag, entry] : font_atlas) {
    std::print("Name: {} (Tag: {}) File: {})\n", entry.name, tag, entry.file);
  }
  return font_atlas;
}

std::map<std::string, ShaderAtlasEntry> LoadShaderAtlas(std::string_view filename) {
  std::map<std::string, ShaderAtlasEntry> shader_atlas{};
  pugi::xml_document shader_doc;
  pugi::xml_parse_result shader_result = shader_doc.load_file(filename.data());
  if (!shader_result) {
    throw std::runtime_error(std::format("Failed to load shader atlas XML: {}\n", shader_result.description()));
  }
  pugi::xml_node shaders_node = shader_doc.child("shaders");
  auto base_path = std::filesystem::path(filename).parent_path();
  for (pugi::xml_node shader_node : shaders_node.children("shader")) {
    ShaderAtlasEntry entry;
    auto tag = shader_node.attribute("tag").as_string();
    entry.name = shader_node.attribute("name").as_string();
    entry.vertex_file = (base_path / shader_node.attribute("vertex").as_string()).string();
    entry.fragment_file = (base_path / shader_node.attribute("fragment").as_string()).string();
    shader_atlas.insert({tag, entry});
  }
  std::print("- Shader Atlas Result -\nLoaded {} shaders from atlas.\n", shader_atlas.size());
  for (const auto &[tag, entry] : shader_atlas) {
    std::print("Name: {} (Tag: {}) Vertex File: {}, Fragment File: {})\n", entry.name, tag, entry.vertex_file, entry.fragment_file);
  }
  return shader_atlas;
}

std::map<std::string, TextureAtlasEntry> LoadTextureAtlas(std::string_view filename) {
  std::map<std::string, TextureAtlasEntry> texture_atlas{};
  pugi::xml_document texture_doc;
  pugi::xml_parse_result texture_result = texture_doc.load_file(filename.data());
   if (!texture_result) {
    throw std::runtime_error(std::format("Failed to load texture atlas XML: {}\n", texture_result.description()));
  }
  if (!texture_result) {
    throw std::runtime_error(std::format("Failed to load texture atlas XML: {}\n", texture_result.description()));
  }
  pugi::xml_node textures_node = texture_doc.child("textures");
  auto base_path = std::filesystem::path(filename).parent_path();
  for (pugi::xml_node texture_node : textures_node.children("texture")) {
    std::string name = texture_node.attribute("name").as_string();
    std::string path = (base_path / texture_node.attribute("path").as_string()).string();
    texture_atlas.insert({name, {path, new Texture(path)}});
  }
  std::print("- Texture Atlas Result -\nLoaded {} textures from atlas.\n", texture_atlas.size());
  for (const auto& [name, entry] : texture_atlas) {
    std::print("Name: {} (File: {}, Dimensions: {}x{})\n", name, entry.path, entry.texture->width, entry.texture->height);
  }
  return texture_atlas;
}
#include "atlas.h"
#include <print>
#include <filesystem>
#include <pugixml.hpp>
#include <format>
#include <stdexcept>
#include <mutex>
#define FontAtlas std::map<std::string, FontAtlasEntry>
#define ShaderAtlas std::map<std::string, ShaderAtlasEntry>
#define TextureAtlas std::map<std::string, TextureAtlasEntry>

std::once_flag font_flag, shader_flag, texture_flag;

void PrintFontResults(FontAtlas& atlas) {
  std::print("- Font Atlas Result -\nLoaded {} fonts from atlas.\n", atlas.size());
  for (const auto& [tag, entry] : atlas) {
    std::print("Name: {} (Tag: {}) File: {})\n", entry.name, tag, entry.file);
  }
}

FontAtlas LoadFontAtlas(std::string_view filename) {
  FontAtlas font_atlas{};
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
  std::call_once(font_flag, [](FontAtlas atlas) { PrintFontResults(atlas); }, font_atlas);
  return font_atlas;
}

void PrintShaderResults(ShaderAtlas& atlas) {
  std::print("- Shader Atlas Result -\nLoaded {} shaders from atlas.\n", atlas.size());
  for (const auto& [tag, entry] : atlas) {
    std::print("Name: {} (Tag: {}) Vertex File: {}, Fragment File: {})\n", entry.name, tag, entry.vertex_file, entry.fragment_file);
  }
}

ShaderAtlas LoadShaderAtlas(std::string_view filename) {
  ShaderAtlas shader_atlas{};
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
  std::call_once(shader_flag, [](ShaderAtlas atlas) { PrintShaderResults(atlas); }, shader_atlas);
  return shader_atlas;
}

void PrintTextureResults(TextureAtlas& atlas) {
  std::print("- Texture Atlas Result -\nLoaded {} textures from atlas.\n", atlas.size());
  for (const auto& [name, entry] : atlas) {
    std::print("Name: {} (File: {}, Dimensions: {}x{})\n", name, entry.path, entry.texture->width, entry.texture->height);
  }
}

TextureAtlas LoadTextureAtlas(std::string_view filename) {
  TextureAtlas texture_atlas{};
  pugi::xml_document texture_doc;
  pugi::xml_parse_result texture_result = texture_doc.load_file(filename.data());
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
  std::call_once(texture_flag, [](TextureAtlas atlas) { PrintTextureResults(atlas); }, texture_atlas);
  return texture_atlas;
}
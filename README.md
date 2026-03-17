# dying-anima
![Banner](assets/textures/banner.png)
**A story about Death and Immortality**
# Screenshots
*Screenshots coming soon*
# Story
Check out [the story](docs/story.md)
# More Info
## Dependencies
- Box2D
- Freetype
- glad
- glfw3
- glm
- imgui
- OpenGL
- pugixml
- EnTT
- tinyfiledialogs
# Installation
## Linux
### Build from source
1. Clone this repo: `git clone https://github.com/sohiearth/dying-anima --recursive`
2. cd into the newly cloned repo: `cd dying-anima`
3. Use the `linux` preset to configure cmake. `cmake --preset=linux`
4. If no vcpkg errors occurred, build the app using `cmake --build --preset=debug-linux` (Debug) or `cmake --build --preset=release-linux` (Release)
5. Run the app `./build/linux/debug/dying-anima` (Debug) || `./build/linux/release/dying-anima` (Release)
### Download
1. Download the latest Linux release.
2. Install dependencies from your package manager.
`pugixml`
Example on arch: `sudo pacman -Sy pugixml`
3. Run (make sure working directory contains the assets folder.)
## Windows
### Build from source
1. Clone the repo using Visual Studio.
2. Configure and build. Should be pretty simple.
### Download
1. Download the latest Windows release.
2. Run (make sure working directory contains the assets folder.)

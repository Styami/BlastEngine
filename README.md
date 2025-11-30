# Prerequisites:
Please install:
- Vulkan SDK [here](https://vulkan.lunarg.com/)
- CMake at least 3.28
- Slang compilation API (Follow build instructions of sources [here](https://github.com/shader-slang/slang/blob/master/docs/building.md) and create a slang install directorie that will be used by the project's CMake)

# Clone and build

Clone the repository with the following command line
```bash
git clone https://github.com/Styami/BlastEngine.git --recursive
```
Go to the project's root and enter these commands to build the project
```bash
cmake -B build
cmake --build build
```
Finally run from the root
```bash
build/BlastEngine
```

A 3D Vulkan renderer which works on Linux

Things I did :
- Uniform variable
- Loading texture
- Depth map

TODO :
- Improve camera's movements
- Multisampling
- Mipmaping
- Load gltf models
- Lighting
- Deferred rendering
- PBR

# Clone and build

Clone the repository with the following command line
```bash
git clone https://github.com/Styami/BlastEngine.git --recursive
```
Go to the project's root and enter these commands to build the project
```bash
cmake -B build -S .
cmake build -Dslang_DIR=/path/to/dir/with/slangConfig
cmake --build build
```
Finally run from the root
```bash
./build/BlastEngine
```

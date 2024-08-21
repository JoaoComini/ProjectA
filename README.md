# Project A - 3D game engine

### WIP: This is in an active development stage, so everything is highly unstable.


## Features
- Scripting via Lua
- Physics powered by [Jolt](https://github.com/jrouwe/JoltPhysics)
- Rendering with Vulkan

## Building from source

### Linux

#### Requirements
- Git

- CMake

- GCC 11.4+ or Clang 11+ (others may be supported but were not tested)

- Development libraries:
    - X11, Xcursor, Xinerama, Xi and XRandR.
    - Vulkan.

- Vulkan 1.3 compatible driver (or `VK_KHR_dynamic_rendering` support)
- Vulkan SDK for `VK_LAYER_KHRONOS_validation` (only on Debug build)

##### Debian/Ubuntu one-liner

```bash
sudo apt update
sudo apt install -y \
  build-essential \
  cmake \
  libx11-dev \
  libwayland-dev \
  libxcursor-dev \
  libxinerama-dev \
  libxi-dev \
  libxrandr-dev \
  libvulkan-dev
```

#### Cloning the repository

We use git submodules to manage third-party dependencies, so you should clone it with the submodules: 

```bash
git clone --recurse-submodules https://github.com/JoaoComini/ProjectA.git
```

#### Compiling with CMake

```bash
cd ProjectA
mkdir build && cd build
cmake ..
make
```

This will compile all the dependencies and binaries. The binaries will be located at the `bin` folder.


## Opening the Editor

You can open the Editor by running:

```bash
./bin/Editor <path-to-your-project>
```

There's a sandbox project included with the repository on `Editor/Sandbox`, so you can run:
```bash
./bin/Editor ../Editor/Sandbox/Sandbox.paproj
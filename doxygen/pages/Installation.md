# Installation

## Linux

### Tools
First, install a compiler, GNU Make and CMake:
```sh
sudo apt install build-essential cmake
```

### Google Test
See the installation instructions here: 
https://tttapa.github.io/Pages/Ubuntu/Software-Installation/GoogleTest.html

### Build and install

```sh
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$HOME/.local"
make install -j$(nproc) # Build and install the optimized release version
make test # Run the tests
cmake .. -DCMAKE_BUILD_TYPE=Debug
make install -j$(nproc) # Build and install the debug version
make test # Run the tests in debug mode (with extra checks)
```
Installing both the release and debug versions can be very useful for checking
certain things during development, and switching to an optimized version later.

If you install the library locally, as demonstrated in the previous snippet,
you might have to set some environment variables, as explained here:
https://tttapa.github.io/Pages/Ubuntu/Software-Installation/Installing-Locally.html

Specifically, you need to add `~/.local` to the `CMAKE_PREFIX_PATH` by adding
the following to your `~/.profile` file:
```sh
CMAKE_PREFIX_PATH="$HOME/.local:$CMAKE_PREFIX_PATH"
```
Then source it (`. ~/.profile`) or log out and back in again.

## Windows

==TODO==

## OSX

==TODO==

***

# Usage

Once the library is installed, you can use it in your own projects.

For example:

**main.cpp**
```cpp
#include <alhpa-lib/overlay_alpha.hpp>

int main() {
    // use overlay_alpha here
}
```

**CMakeLists.txt**
```cmake
cmake_minimum_required(VERSION 3.11)
project(Project)

# Find the library you just installed:
find_package(ARM-NEON-Compositor REQUIRED)

add_executable(main main.cpp)
# Link your executable with the library:
target_link_libraries(main PRIVATE ARM-NEON-Compositor::alpha-lib)
```

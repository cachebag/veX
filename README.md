# veX

**veX** is a 2D platformer built using C++ and the SFML library.

The current implementation of this game will be to flesh out the P vs NP Quest for CSC 1100. 

##

## Some things to note:
- `enemy.cpp` is the "sentry", within the context of the quest. any future additions to this NPC will be called 'enemy' for the sake of consistency.
- The code is very hackish. Particularly in how player movement/animation is handled, the asset manager/level editor and the parallax factor calculations within `background.cpp`
- The asset management needs LOTS of work, but for the sake of completing this project on time, I will not spend any effort to flesh out the asset manager yet.
- The resolution scaling is horrible. Right now, the game runs best on 1920x1080 monitors and even then, across platforms it may be pretty buggy. This is admittedly due to my lack of expertise and willingness to figure out how to fix this. It could be an embarassingly easy fix, but I have never built a game from scratch with no engine so this will be the extent of my solution. If your laptop/desktop doesn't support 1920x1080, I am sorry. 

## 

All background art, platforms, textures, etc. were pulled from [here](https://ansimuz.itch.io/gothicvania-patreon-collection), however the enemy, player and weapon designs were all created from scratch using Gimp.

## Requirements

To build and run this project, you need to have the following dependencies installed:

### 1. C++ Compiler
- A modern C++ compiler that supports C++11 or higher (e.g., GCC, Clang, or MSVC).

### 2. SFML (Simple and Fast Multimedia Library)
- **Version**: SFML 2.5 or later
- **Download**: [https://www.sfml-dev.org/download.php](https://www.sfml-dev.org/download.php)

Ensure that SFML is correctly installed and linked with your project. You will need the following SFML modules:
   - `sfml-graphics`
   - `sfml-window`
   - `sfml-system`
   - `sfml-audio` 

### 3. Native File Dialog (Optional)
- **nfd.h** is used in this project to provide cross-platform file dialogs.
- **Download**: [https://github.com/mlabbe/nativefiledialog](https://github.com/mlabbe/nativefiledialog)

### 4. CMake
- CMake is used to build the project.
- **Version**: CMake 3.10 or later
- **Download**: [https://cmake.org/download/](https://cmake.org/download/)

### Building Instructions

To compile the project:

1. Clone the repository:
   ```bash
   git clone git@github.com:cachebag/veX.git
   cd veX
   ```

2. Make sure the required dependencies are installed and properly linked (SFML, nfd, etc.).

3. **Using CMake** (recommended):
   - Create a build directory and run CMake:
     ```bash
     mkdir build
     cd build
     cmake ..
     make
     ```

4. **Manually** (if not using CMake):
   - Compile the project using your preferred C++ compiler. Example using g++:
     ```bash
     g++ -std=c++11 -o veX src/*.cpp -I include -lsfml-graphics -lsfml-window -lsfml-system
     ```

### Notes

- You can't run the game in the build directory. You will need to cd back into root and run ./build/veX
- This project was developed on Linux. It *should* work on macOS, but you'll need to recompile `nfd` (Native File Dialog) and ensure that the Cocoa framework is properly linked during the build process. (I do have a branch configured to work on MacOS but it is pretty unstable).
- If you're on Windows, good luck. Maybe WSL?






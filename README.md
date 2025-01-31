# raytris
![Screenshot from 2024-12-23 22-28-01](https://github.com/user-attachments/assets/c8f5f527-d528-462e-b32c-dd5fe730cced)
## Controls
![Screenshot from 2025-01-25 19-41-58](https://github.com/user-attachments/assets/ca135453-44c3-4366-9b8d-aca5f83c6481)
### Menu
- Esc: Close raytris
- Up/Down: Select game mode
- F: Change resolution
### Single player
- Left/Right: Move falling piece left/right
- Up/Z: Turn falling piece clockwise/counterclockwise
- Down: Soft drop falling piece
- Space: Hard drop falling piece
- A: Turn falling piece 180 degrees
- C: Swap falling piece with hold piece
- Enter: (In Game)Pause game
- R: Restart Game
### Two players
- TODO: Describe two players controls
## Depencencies
You need to have a C++ 23 compiler, CMake and raylib installed. The cmake script will try to install it for you, but you still need to have raylib's dependencies installed.
If you are building for Web you will also need emscripten.
## Building
# Building for Desktop
1. Clone the repo with `git clone https://github.com/All23tor/raytris/` and `cd raytris` to it
2. `cmake -S . -B build` to configure build directory
3. `cmake --build build` to build
5. `cd build && ./raytris` to run :D

# Building for Web
Before building make sure you have the emscripten binaries in your path.
1. Clone the repo with `git clone https://github.com/All23tor/raytris/` and `cd raytris` to it
2. `emcmake cmake -S . -B build-web -DPLATFORM=Web` to configure build directory
3. `cmake --build build-web` to build
5. `cd build-web && emrun raytris.html` to run :D


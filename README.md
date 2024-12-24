# raytris
![Screenshot from 2024-12-23 22-28-01](https://github.com/user-attachments/assets/c8f5f527-d528-462e-b32c-dd5fe730cced)
## Controls
![Screenshot from 2024-12-23 22-28-08](https://github.com/user-attachments/assets/49f18a06-2340-44ba-91c8-a7c03f36b1dc)
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
You need to have a C++ 23 compiler and raylib installed. The cmake script will try to install it for you, but you still need to have raylib's dependencies installed.
## Building
1. Clone the repo with `git clone https://github.com/All23tor/raytris/` and `cd raytris` to it
2. `mkdir build && cd build` to make build directory
3. `cmake .. && make` to build
5. `./raytris` to run :D

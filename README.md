# System Programming course project
## A - SpcInvdrs

A distributed, multiplayer variation of the classic **Space Invaders** game, implemented in C. The project features astronauts controlled by players, aliens moving randomly, and a central server managing game logic. The game supports up to **8 players simultaneously**.

## Features
- Players control astronauts with arrow keys.
- Fire lasers with the **spacebar** to destroy aliens.
- **NCurses** interface for client and game display.
- Uses **ZeroMQ** TCP sockets for communication.
- Distributed architecture:
  - **Server** manages game logic and player actions.
  - **Clients** allow astronaut control.
  - **Display** mirrors the game's state.
  
## Requirements
- C Compiler (GCC) 
- Libraries:  
  - [ZeroMQ](https://zeromq.org/) (`libzmq3-dev`)  
  - [NCurses](https://invisible-island.net/ncurses/) (`libncurses5-dev`)
  
```bash
dnf install zeromq-devel
dnf install czmq-devel
dnf install ncurses-devel
```

## Project Files
```
project
├── game-server.c   
├── astronaut-client.c
├── outer-space-display.c 
├── common.h
└── Makefile
```

- **`game-server.c`**: Core logic for managing the game state.
- **`astronaut-client.c`**: Allows player control via a terminal-based interface.
- **`outer-space-display.c`**: Real-time visualization of the game grid.
-  **`common.h`**: Shared constants and message definitions.
- **`Makefile`**: Automates compilation of all components.

## Build and Run
Build the project running into the directory:
```
make
```
Start in different terminals server, clients and display, rispectively:
```
./game-server
./astronaut-client
./outer-space-display
```

## Controls
- **Arrow Keys**: Move the astronaut (depending on assigned region).  
- **Spacebar**: Fire laser.  
- **Q**: Quit the game.

## Game Rules
1. **Movement**:
   - Each astronaut is assigned a specific region in the game grid.
   - Movement is restricted to the region assigned.
2. **Zapping**:
   - Lasers traverse the entire grid, destroying any aliens in their path.
   - Other astronauts hit by a laser are stunned for **10 seconds**.
   - Cooldown for firing lasers: **3 seconds**.
3. **Winning**:
   - The game ends when all aliens are destroyed.
   - The astronaut with the highest score wins.
  
## Error Handling
#TODO
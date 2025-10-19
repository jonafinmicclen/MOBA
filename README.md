# MOBA Project

This is a C++ MOBA project using:

- OpenGL
- SDL2
- GLEW
- ENet networking

---

## Next focus

- [x] Get server/client to send and recieve custom packets that can easily be modified for game purposes. 
- [ ] Begin implementing game logic!

---


## Intended Classes

- NetClient
- NetServer
- Client - Pre game client
- GameClient - ActualGame (Game, NetClient)
- Game - Core game logic with optional audio and rendering if part of server.
- Server - Game server (Game, NetServer)

---


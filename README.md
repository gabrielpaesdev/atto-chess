# atto-chess

A small 2D chess game written in C++ with SDL2. The compiled binary is around 26 KB.

Still in beta. You can play a full game start to checkmate on Linux, but there's rough edges and stuff I haven't gotten to yet.

## Current state

- Local 2-player, pass-and-play on one machine
- Basic built-in AI to play against
- Menu language toggle (English / Portuguese)
- Credits screen

Move validation and the core rules all work. Tested on Linux.

## Missing / in progress

- Animations — pieces just snap to their tile right now, no sliding
- Sound effects for moves, captures, check, game over
- A better AI. It's pretty deterministic at the moment and gets slow searching deeper, so I'm working on the eval function, pruning, and giving it some variety in play
- Chess clock (bullet/blitz/rapid, increments)
- Cleaner match UI — captured piece counters, better move indicators
- LAN multiplayer
- Windows and Android builds (maybe iOS eventually)

## Requirements

You'll need a C++ compiler and the SDL2 dev headers:

```bash
# Debian / Ubuntu / Mint
sudo apt update
sudo apt install build-essential libsdl2-dev
```.

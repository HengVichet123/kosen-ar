# kosen-ar

AR projects built for kosen (高専) graduation research using **ARToolKit 2.72**, **OpenGL/GLUT**, and **C++** on Linux. A static JPEG image with a Hiro AR marker serves as the scene anchor — no live camera required.

---

## EarthDefender

An AR space shooter. 3D enemies spawn from portals on the AR board and move toward Earth. Shoot them down before they reach it.

**Tech:** C++ · OpenGL · ARToolKit · OpenCV · GLMetaseq (.mqo models)

**Features**
- 5 enemy types with different speeds, health, and behaviors (teleporter, swarmer queen, etc.)
- 3 weapons: bullets, laser beam, area bomb
- Animated explosions, health bars, kill counter, HUD

<video src="https://raw.githubusercontent.com/HengVichet123/kosen-ar/main/EarthDefender/demo_vid.webm" controls width="720"></video>

---

## RoomNavigator

A multi-room AR walker. Move a character through a parking lot → lobby → room 1 / room 2 environment. Approaching a door transitions to the next room.

**Tech:** C++ · OpenGL · ARToolKit · OpenCV · GLMetaseq

**Features**
- Bidirectional room graph with automatic scene transitions
- Cylinder-based collision for doors (transition) and furniture (solid obstacle)
- Object interactions: push, pull, jump-on
- Built-in edit mode to reposition and resize objects, with save/load

<video src="https://raw.githubusercontent.com/HengVichet123/kosen-ar/main/RoomNavigator/roomNavi_demo.webm" controls width="720"></video>

---

## EnvDesigner

A 3D scene layout tool for designing the room environments used by RoomNavigator. Spawn models from a library of 92 `.mqo` files, assign types (door, furniture, init position), tune collision cylinders, and save in RoomNavigator-compatible format.

**Tech:** C++ · OpenGL · ARToolKit · OpenCV · GLMetaseq

**Features**
- 92 spawnable models (type name with in-app text input or browse with picker)
- Per-model collision cylinder editor with live display
- Save / load layouts (appends on load, auto-fixes old path formats)
- Model type assignment for RoomNavigator integration

<video src="https://raw.githubusercontent.com/HengVichet123/kosen-ar/main/EnvDesigner/envDesign_demo.webm" controls width="720"></video>

---

## Build

```bash
# requirements: OpenCV 4, freeglut3-dev, libglew-dev, ARToolKit 2.72
make -C EarthDefender
make -C RoomNavigator
make -C EnvDesigner
```

Each binary must be run from its own directory with an active X display:

```bash
DISPLAY=:1 ./EarthDefender/EarthDefender
DISPLAY=:1 ./RoomNavigator/RoomNavigator
DISPLAY=:1 ./EnvDesigner/EnvDesigner
```

---

## Structure

```
kosen-ar/
├── EarthDefender/   # AR space shooter
├── RoomNavigator/   # Multi-room AR navigation
├── EnvDesigner/     # Scene layout designer
├── shared/          # Shared headers and ARToolKit stubs
└── Makefile.common  # Common build rules
```

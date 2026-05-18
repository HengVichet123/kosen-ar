# RoomNavigator

An AR room navigation system. The player walks through a multi-room environment — parking lot → lobby → room 1 / room 2 — by approaching doors. Each room is a separate AR scene loaded from a static JPEG background with a Hiro marker.

---

## Overview

RoomNavigator renders 3D furniture, doors, and other objects anchored to a Hiro AR marker. When the player's character walks close enough to a door object, the scene transitions to the connected room automatically. Each room's layout is loaded from saved data files.

The player can also enter **Control Mode** to reposition, resize, push, pull, or delete individual objects, then save the modified layout back to disk.

---

## Requirements

- Linux with X display
- OpenCV 4
- OpenGL / GLUT (`freeglut3-dev`)
- GLEW (`libglew-dev`)
- ARToolKit 2.72 (path set in `../Makefile.common`)
- GLMetaseq (in `../shared/GL/`)

---

## Build & Run

```bash
make
DISPLAY=:1 ./RoomNavigator
```

Must be run from the `RoomNavigator/` directory.

---

## Environment Graph

```
Parking Lot  ←→  Lobby  ←→  Room 1
                        ←→  Room 2
```

Door transitions are bidirectional. Approaching a door in any room takes you to the connected room and places the character at that room's init position.

---

## Controls

### Navigation (Play Mode)

| Key | Action |
|-----|--------|
| Arrow keys | Move character |
| `1`–`8` | Switch animation (idle, walk, jump, run, push, pull, …) |
| `H` | Toggle HUD (key reference overlay) |

### Edit (Control Mode — toggle with `E`)

| Key | Action |
|-----|--------|
| `E` | Enter / exit Control Mode |
| `>` / `<` | Cycle selected model |
| Arrow keys | Move selected model (XY) |
| `P` / `O` | Move selected model up / down (Z) |
| `C` / `X` | Scale selected model up / down |
| `Shift+D` / `Shift+A` | Increase / decrease collision radius |
| `Shift+W` / `Shift+S` | Increase / decrease collision height |
| `t` | Toggle collision cylinder display (all models) |
| `0` | Save current layout to file |
| `Delete` | Delete selected model |
| `H` | Toggle HUD |

### Interactions (Play Mode)

| Key | Action |
|-----|--------|
| `N` | Push nearest object away |
| `P` | Pull nearest object toward you |
| `Space` | Jump onto nearest object |

---

## Save Data Format

Each room layout is stored as two files in `Data/SavedData/`:

- `<name>_positions.txt` — one model per line: `idx x y z rot vrot height ori_height radius ori_radius k mType`
- `<name>_stringData.txt` — one model file path per line: `Data/mqoFile/<name>.mqo`

Model types (`mType`): `0`=null, `1`=domain, `2`=door, `3`=furniture, `4`=initPos

---

## Project Structure

```
RoomNavigator/
├── RoomNavigator.cpp       # All navigation and rendering logic
├── Makefile
├── Data/
│   ├── camera_para.dat
│   ├── patt/patt.hiro
│   ├── images/             # Room background images
│   │   ├── parkinglot.jpg
│   │   ├── frontDesk.jpg
│   │   ├── room1.jpg
│   │   └── room2.jpg
│   ├── mqoFile/            # 3D model files (.mqo)
│   └── SavedData/          # Saved room layouts
└── Sequence/
    └── myAnimation/        # Character animation frames
        ├── idle/, walk/, jump/, run/, push/, pull/, …
```

---

## Technical Notes

- **Static image AR**: `cv::imread()` loads the background; the same frame is used every tick.
- **Collision detection**: cylinder-vs-cylinder test between the character and each model. Door cylinders trigger room transitions; furniture cylinders block movement.
- **Edit mode**: all per-model transforms (position, rotation, scale, cylinder dimensions) are live-editable and immediately visible.
- **HUD**: semi-transparent centered overlay drawn with `cv::addWeighted`; toggled with `H`.

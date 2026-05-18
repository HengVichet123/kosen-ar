# EnvDesigner

A 3D environment designer for creating and editing room layouts used by RoomNavigator. Place models from a library of 92 `.mqo` files, assign model types (domain, door, furniture, init position), adjust collision cylinders, and save the result in RoomNavigator-compatible format.

---

## Overview

EnvDesigner renders a static AR scene (Hiro marker on a background JPEG) and lets you spawn, move, rotate, scale, and delete 3D objects interactively. Saved layouts can be loaded directly into RoomNavigator.

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
DISPLAY=:1 ./EnvDesigner
```

Must be run from the `EnvDesigner/` directory.

---

## Controls

### Model Selection & Spawning

| Key | Action |
|-----|--------|
| `i` | Open text input — type a model name, press Enter to spawn |
| `[` / `]` | Cycle the quick-spawn picker |
| `u` | Spawn the currently highlighted picker model |
| `m` | Toggle model list overlay (all 92 available models) |
| `>` / `<` | Cycle selected model among placed models |
| `f` | Delete selected model |

### Moving & Transforming

| Key | Action |
|-----|--------|
| Arrow keys | Move selected model (XY) |
| `P` / `O` | Move selected model up / down (Z) |
| `C` / `X` | Scale up / down |
| `Z` | Reset position and rotation to zero |
| Left / Right arrows | Rotate around Z axis |
| Up / Down arrows | Tilt around X axis |

### Collision Cylinder

| Key | Action |
|-----|--------|
| `t` | Toggle collision cylinder and axes display |
| `Shift+D` / `Shift+A` | Increase / decrease radius |
| `Shift+W` / `Shift+S` | Increase / decrease height |

### Model Type & Door Wiring

| Key | Action |
|-----|--------|
| `y` | Cycle model type: null → domain → door → furniture → initPos |
| `1` / `2` / `3` | Set door destination (room index 0 / 1 / 2) — door models only |

### Save & Load

| Key | Action |
|-----|--------|
| `0` | Open save prompt — type a filename, Enter to save |
| `9` | Open load prompt — shows available save files, type a filename, Enter to load (appends to scene) |
| `Esc` | Cancel current input |

### Display

| Key | Action |
|-----|--------|
| `h` | Toggle help / key reference overlay |
| `m` | Toggle model list overlay |

---

## Model Types

| Type | `mType` value | Purpose in RoomNavigator |
|------|---------------|--------------------------|
| `null` | 0 | Decorative only, no collision |
| `domain` | 1 | Room floor / domain boundary |
| `door` | 2 | Triggers room transition when player approaches |
| `furniture` | 3 | Solid obstacle — blocks player movement |
| `initPos` | 4 | Player spawn point for this room |

---

## Save Data Format

Saves two files to `Data/SavedData/`:

- `<name>_positions.txt` — `idx x y z rot vrot height ori_height radius ori_radius k mType` per line
- `<name>_stringData.txt` — `Data/mqoFile/<model>.mqo` per line

Old saves that used the shorter `Data/<model>.mqo` path are automatically corrected on load.

---

## Available Models (92)

Typed with the `i` input box or browsed with `[`/`]` + `m`:

```
Bishop        BlackBishop   BlackChessKing  BlackChessPawn  BlackChessQueen
BlackChessRook BlackHair    BlackKnight     ChessKing       ChessPawn
ChessQueen    ChessRook     GoldKnight      Knight          MaskKnight
MaskKnight1-4 Robot         TableClock      Tele            ball
balls         barricade     bed             bench           bicycle
bl_ninja      books         car             chair           character
character1-7  clock         cone            cube            cubes
cylinder      desk          domain          door            drawer
falling       floor         goalNet         human           iamRun
invisible     jeep          jeep2           jeep3           keyboard
mario         monitor       mouse           mug             myHuman
mymotion0     mystickman    newSticker      ninja           r8
raptor        raptor1       roundTable      rug             runner0
runner1       sacchi        sofa            stickMan        sticker
stickerMan    street        table           tachikoma       theCone
theRed        theStick      tree            tree2           tunnel
tv            wall          window
```

---

## Project Structure

```
EnvDesigner/
├── EnvDesigner.cpp         # All designer logic
├── Makefile
├── Data/
│   ├── camera_para.dat
│   ├── patt/patt.hiro
│   ├── images/             # Background image for the AR scene
│   ├── mqoFile/            # 3D model files (.mqo) — 92 models
│   └── SavedData/          # Saved layouts (loaded by RoomNavigator)
└── obj/                    # Compiled object files
```

---

## Technical Notes

- **In-app text input**: GLUT has no native text widget; keystroke capture is handled manually in `KeyDown` with an `inputMode` flag. The status bar at the top always shows the current mode and last feedback message.
- **Path compatibility**: loader auto-upgrades old `Data/<name>.mqo` paths to `Data/mqoFile/<name>.mqo`.
- **Non-destructive load**: loading a saved file appends its models to the current scene rather than replacing it, so multiple layouts can be merged.
- **Collision cylinder**: rendered as two circles + 4 vertical lines with `GL_LIGHTING` and `GL_DEPTH_TEST` disabled so it is always visible over models.

# EarthDefender

An AR space shooter built as a kosen (高専) graduation research project. 3D enemies and a player ship are overlaid on a static background image using ARToolKit marker detection. Defend Earth by shooting down 100 enemies before they reach it.

---

## Overview

EarthDefender uses a Hiro AR marker detected in a static JPEG image to anchor a 3D coordinate system. All game objects — the player ship, Earth, portals, enemies, projectiles, bombs, and lasers — are rendered in OpenGL on top of the background image using the marker's transform matrix.

---

## Requirements

- Linux (tested on Ubuntu with X display)
- OpenCV 4 (`pkg-config --libs opencv4`)
- OpenGL / GLUT (`freeglut3-dev`)
- GLEW (`libglew-dev`)
- ARToolKit 2.72 source (path configured in `Makefile`)
- GLMetaseq (included in `GL/`)

---

## Build

Edit `AR_SRC` in `Makefile` to point to your ARToolKit 2.72 source directory, then:

```bash
make
```

Output binary: `./EarthDefender`

---

## Run

Must be run from within the `EarthDefender/` directory with an active X display:

```bash
DISPLAY=:1 ./EarthDefender
```

---

## Controls

| Key | Action |
|-----|--------|
| Arrow keys | Move ship |
| `Space` | Fire active weapon |
| `N` | Drop bomb (area damage) |
| `1` | Switch to bullets |
| `2` | Switch to laser |
| `H` | Toggle hitbox display |
| `Shift+D` | Toggle debug HUD |
| `R` | Restart after game over / win |

---

## Gameplay

- Enemies spawn from one of three portals on the AR board and move toward Earth.
- Shoot enemies before they reach Earth. Earth has a health pool — if it reaches zero, game over.
- Kill 100 enemies to win.
- The current weapon and kill count are always shown on screen.

### Enemy Types

| Model | Name | Speed | Health | Notes |
|-------|------|-------|--------|-------|
| alien0 | Scout | Fast | Low | 2-shot kill |
| alien1 | Soldier | Medium | Medium | — |
| alien2 | Splitter | Medium | Medium | — |
| alien3 | Teleporter | Slow | High | Teleports randomly |
| alien4 | Swarmer Queen | Very slow | Very high | Spawns child swarmers |

Swarmer children have 1 HP, move at medium speed, and have no health bar.

### Weapons

- **Bullets** — rapid fire, low damage per shot
- **Laser** — continuous beam, high DPS
- **Bomb** — area-of-effect damage around the ship, does not move enemies

---

## Project Structure

```
EarthDefender/
├── EarthDefender.cpp       # All game logic
├── Makefile
├── arVideo_stub.c          # No-op video functions (static image mode)
├── AR/                     # ARToolKit 2.72 headers
│   ├── config.h
│   ├── sys/                # Platform video headers (required by video.h)
│   └── ...
├── GL/
│   ├── GLMetaseq.c/h       # Metasequoia .mqo model loader
│   └── controller.h        # Key definitions and input struct
├── Data/
│   ├── camera_para.dat     # Camera calibration parameters
│   ├── patt/patt.hiro      # Hiro marker pattern file
│   ├── images/             # Background images (myhall.jpg, test2.jpg)
│   └── mqoFile/            # 3D models (jet, earth, portal, aliens, weapons)
└── Sequence/
    └── earthDefender/
        └── explosion/      # Bomb explosion animation frames (bomb0-5.mqo)
```

---

## Technical Notes

- **Static image AR**: `cv::imread()` loads the background JPEG; raw pixel data is passed to `arDetectMarker()` each frame instead of a live camera feed.
- **Pixel format**: `AR_PIXEL_FORMAT_BGRA` (Linux default, 4 bytes/pixel).
- **Draw mode**: `AR_DRAW_BY_GL_DRAW_PIXELS` — avoids crashes with certain NVIDIA drivers.
- **Draw order**: models are drawn back-to-front (Earth before ship) so the ship always appears on top.
- **Coordinate system**: game world units, origin at marker center. Portal positions: `(-300, 0)`, `(300, 300)`, `(100, 300)`.

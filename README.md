# kosen-ar

AR projects built as kosen (高専) graduation research using ARToolKit 2.72, OpenGL/GLUT, and C++ on Linux.

## Projects

| Directory | Description |
|-----------|-------------|
| [`EarthDefender/`](EarthDefender/) | AR space shooter — defend Earth from alien enemies |
| [`RoomNavigator/`](RoomNavigator/) | Multi-room AR walker — explore and navigate between rooms |
| [`EnvDesigner/`](EnvDesigner/) | Environment designer tool — place and save 3D scene layouts |

All three share a common build system (`Makefile.common`) and a `shared/` library directory containing ARToolKit stubs, GLMetaseq, and controller headers.

## Shared Structure

```
kosen-ar/
├── Makefile.common       # Shared compiler flags and link rules
├── shared/               # Headers and compiled objects used by all projects
│   └── GL/
│       ├── GLMetaseq.c/h # Metasequoia .mqo loader
│       └── controller.h  # Key definitions and input struct
├── EarthDefender/
├── RoomNavigator/
└── EnvDesigner/
```

## Build Requirements

- Linux with X display
- OpenCV 4
- OpenGL / GLUT (`freeglut3-dev`)
- GLEW (`libglew-dev`)
- ARToolKit 2.72 source (set `AR_SRC` in `Makefile.common`)

Build any project individually:

```bash
make -C EarthDefender
make -C RoomNavigator
make -C EnvDesigner
```

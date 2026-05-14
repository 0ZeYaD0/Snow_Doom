# ❄️ Snow Doom

A modern C++ first-person action game built from scratch with a custom game engine. Survive the frozen wasteland, battle enemies, and uncover the mysteries of Snow Doom.

---

## 🎮 Features

- **Custom Game Engine** – Built in C++ with OpenGL for rendering
- **3D Graphics** – Real-time rendering with GLSL shaders
- **Physics System** – Collision detection and physics simulation
- **Audio System** – OpenAL-based sound effects and music
- **Dynamic UI** – Player HUD with weapons, health, and interactions
- **Map System** – Level loading and generation with collision data
- **Enemy AI** – Intelligent enemies to battle
- **Weapon System** – Multiple weapons with different mechanics
- **Player Movement** – Smooth camera and movement controls

---

## 📋 Requirements

### Runtime
- **Windows OS** (64-bit recommended)
- **OpenGL 4.0+** compatible GPU
- **Audio Device** for sound output

### Development
- **C++20** or later
- **GCC/MinGW** compiler
- **ymk** build system
- **GLFW3** (windowing)
- **OpenGL/GLAD** (graphics)
- **OpenAL** (audio)
- **GLM** (mathematics)

---

## 🚀 Quick Start

### 1. Clone the Repository
```bash
git clone https://github.com/0ZeYaD0/Snow_Doom.git
cd Snow_Doom
```

### 2. Build the Project
```bash
ymk build
```

Or use the VS Code task: **Build and Run Snow Doom**

### 3. Run the Game
```bash
.\bin\snow_doom.exe
```

---

## 🎛️ Controls

| Action | Key |
|--------|-----|
| Move Forward | `W` |
| Move Backward | `S` |
| Strafe Left | `A` |
| Strafe Right | `D` |
| Jump | `Space` |
| Dash | `Shift` |
| Look Around | `Mouse` |
| Fire/Attack | `Left Click` |
| Switch Weapon | `1-9` or `Scroll` |
| Interact | `E` |
| Menu | `ESC` |

---

## 📁 Project Structure

```
Snow_Doom/
├── src/                    # Source code
│   ├── main.cpp           # Entry point
│   ├── engine/            # Core engine systems
│   │   ├── graphics/      # OpenGL rendering
│   │   ├── audio/         # OpenAL sound system
│   │   ├── physics/       # Collision & physics
│   │   ├── map/           # Level loading
│   │   ├── window/        # GLFW window management
│   │   ├── ui/            # UI system
│   │   └── core/          # Input & utilities
│   └── game/              # Game-specific code
│       ├── core/          # Game logic & entities
│       ├── player/        # Player & camera
│       ├── enemies/       # Enemy AI
│       └── ui/            # Game HUD
├── include/               # Header files (mirrors src/)
├── res/                   # Game resources
│   ├── shaders/           # GLSL shader files
│   ├── textures/          # Texture assets
│   ├── models/            # 3D models
│   ├── maps/              # Level files
│   └── audio/             # Sound effects & music
├── vendor/                # Third-party libraries
│   ├── glm/               # Math library
│   ├── GLFW/              # Window management
│   ├── OpenAL/            # Audio
│   ├── glad/              # OpenGL loader
│   ├── stb/               # Image loading
│   └── tiny_obj_loader/   # Model loading
├── lib/                   # Compiled libraries
├── build/                 # Build artifacts (generated)
├── bin/                   # Executable output
├── build.ymk              # Build configuration
└── tools/                 # Development utilities
```

---

## 🔧 Build Configurations

The project supports multiple build configurations:

- **Debug** – Includes debug symbols and no optimization (`-g -O0`)
- **Release** – Optimized build (`-O3`)

Build in release mode with:
```bash
ymk build release
```

---

## 🚧 Roadmap & TODO

### UI Improvements
- [ ] Dynamic crosshair
- [ ] Better dash counter display
- [ ] Gun information HUD
- [ ] Screen effects (blood splatter, damage feedback)

### Gameplay Features
- [ ] Sound effects
- [ ] Main menu & scene management
- [ ] Cutscene & boss fight system
- [ ] Door interaction system with keys
- [ ] Weapon pickup mechanics

### Enemy System
- [ ] Multiple enemy types with unique behaviors
- [ ] "Truck-kun" boss fight
- [ ] Enemy spawn management

---

## 🛠️ Development

### Building from Source
```bash
# Debug build (default)
ymk build

# Release build
ymk build release

# Clean build
ymk clean
```

### Project Configuration
Edit `build.ymk` to modify:
- C++ standard (`cpp_std`)
- Compiler flags
- Linked libraries
- Include/library directories

---

## 📦 Dependencies

| Library | Purpose | Version |
|---------|---------|---------|
| **GLFW3** | Window management | 3.x |
| **OpenGL/GLAD** | Graphics rendering | 4.0+ |
| **OpenAL** | Audio system | 1.x |
| **GLM** | Math operations | Latest |
| **stb_image** | Texture loading | - |
| **tiny_obj_loader** | Model loading | - |

---

## 🎨 Graphics & Assets

- **Shaders** – Located in `res/shaders/`, includes vertex and fragment shaders for 3D rendering and UI
- **Textures** – Organized by category (Brick, Stone, Wood, Tiles, Doors, etc.)
- **Models** – OBJ format models with MTL materials
- **Maps** – Custom map definitions with collision data

---

## 🔊 Audio

The game uses **OpenAL** for spatial audio. Audio files are organized in `res/audio/` by event type (death, impact, etc.).

---

## 🐛 Troubleshooting

### Game won't start
- Ensure your GPU supports OpenGL 4.0+
- Check that all DLLs (glfw3dll, OpenAL32dll) are in the `lib/` directory
- Verify the executable is in `bin/snow_doom.exe`

### No sound
- Check that your audio device is connected
- Verify OpenAL is properly installed

### Build errors
- Ensure C++20 compiler support (`g++ --version`)
- Verify `ymk` is installed and in your PATH
- Check that vendor libraries are up to date

---

## 📄 License

[Add license information here]

---

## 👤 Author

**0ZeYaD0** – Game Developer <br>
**DeltaY0** - Game Developer

---

## 🤝 Contributing

Contributions, bug reports, and feature requests are welcome! Feel free to open issues or submit pull requests.

---

**Enjoy Snow Doom! ❄️🎮**

# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a **single-player remake of Helbreath** built with C++20 and Raylib, targeting both Windows desktop and web (Emscripten) platforms with a unified codebase. The original Helbreath was an MMORPG; this version reimagines it as a single-player experience.

## Build Commands

### Desktop Build (Windows)
Open `HelbreathRaylib.sln` in Visual Studio 2022 and build (Debug or Release, x64).

**Command line (MSBuild):**
```powershell
# Release build
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" HelbreathRaylib.sln -p:Configuration=Release -p:Platform=x64 -m:1

# Debug build
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" HelbreathRaylib.sln -p:Configuration=Debug -p:Platform=x64 -m:1
```

Output: `Build/x64/{Debug|Release}/HelbreathRaylib.exe`

Note: Use `-m:1` (single-threaded) to avoid race condition with the GenerateItemIDHeader prebuild tool.

### Web Build (Emscripten)
```powershell
# Release build
powershell -ExecutionPolicy Bypass -File build_web.ps1 -Configuration Release

# Debug build
powershell -ExecutionPolicy Bypass -File build_web.ps1 -Configuration Debug
```

Output: `Build/Web/{Debug|Release}/helbreath_web.html`

### Serve Web Build
```powershell
.\serve_web.ps1 -Configuration Release
# or
python serve_web.py
```

## Architecture Overview

### Core Systems

**Application** (`Application.h/cpp`) - Singleton managing window lifecycle, layer stack, platform services, and frame timing.

**Game** (`Game.h/cpp`) - Main game layer owning SceneManager, sprite collections, and SoundPlayer.

**SceneManager** (`SceneManager.h`) - FSM for scene transitions with configurable fade effects.

### Platform Abstraction Layer (`Platform/`)

All platform-specific code is abstracted through interfaces. **Game code NEVER uses `#ifdef __EMSCRIPTEN__`**.

- `ICursorManager` - Cursor hide/show/lock
- `IInputTranslator` - Input handling
- `ITimerService` - Repeating/one-shot timers (thread-based on desktop, frame-based on web)
- `IPlatformInfo` - Platform capabilities

Access via: `Application::get_platform()->getCursorManager().hide()`

## Scene System

### Base Scene Class

All scenes inherit from `Scene` and must implement:
```cpp
class MyScene : public Scene {
public:
    SCENE_TYPE(MyScene)  // Generates type ID and inherits constructor

    void on_initialize() override;    // Called when scene becomes active
    void on_uninitialize() override;  // Called when scene is deactivated
    void on_update() override;        // Called each frame (not during transitions)
    void on_render() override;        // Called each frame (always)
};
```

### Protected Resources (Available to All Scenes)
```cpp
Game& game;                              // Main game object
SceneManager& scene_manager;             // For scene transitions
CSpriteCollection& sprites;              // UI/screen sprites
CSpriteCollection& model_sprites;        // Character models
CSpriteCollection& map_tiles;            // Map/world sprites
std::vector<ItemMetadataEntry>& item_metadata;
StaticEntityManager& static_entity_manager;
SoundPlayer& sound_player;
```

### Scene Transitions

```cpp
// Transition with fade effect
scene_manager.set_scene<MainMenuScene>();

// Check previous scene
if (scene_manager.previous_scene_is<CreateCharacterScene>()) { }

// Transition timing (in SceneManager)
constexpr float FAST_FADE_DURATION = 0.15f;
constexpr float DEFAULT_FADE_DURATION = 0.25f;
constexpr float SLOW_FADE_DURATION = 0.5f;
```

### Current Scenes
- **LoadingScene** - Progressive asset loading (sprites, sounds, maps, items)
- **MainMenuScene** - Main menu with Login, New Account, Exit buttons
- **CharacterSelectionScene** - Displays saved characters, loads from `.jsave` JSON files
- **CreateCharacterScene** - Character creation with class selection, stat allocation, appearance customization
- **MainGameScene** - Main gameplay with map rendering, camera, entities
- **ExitGameScene** - Exit confirmation with timer

## UI Component System (`UI/`)

### Base Control Class

All UI components inherit from `Control`:
```cpp
// Properties
control.SetBounds(x, y, width, height);
control.SetEnabled(true);
control.SetVisible(true);

// Event callbacks (std::function)
control.OnClick = [](Control* sender) { };
control.OnMouseEnter = [](Control* sender) { };
control.OnMouseLeave = [](Control* sender) { };
control.OnMouseHover = [](Control* sender) { };

// Lifecycle
control.Update();  // Call each frame
control.Render();  // Call each frame
```

### Button

```cpp
auto button = std::make_unique<UI::Button>(sprites);

// Sprite configuration
UI::Button::SpriteConfig sprite_cfg;
sprite_cfg.sprite_id = SPRID_BUTTONS;
sprite_cfg.normal_frame = 0;
sprite_cfg.hover_frame = 1;
sprite_cfg.pressed_frame = 2;
sprite_cfg.disabled_frame = 3;
button->SetSprite(sprite_cfg);

// Text configuration
UI::Button::TextConfig text_cfg;
text_cfg.text = "Click Me";
text_cfg.font_index = FontFamily::Default;
text_cfg.font_size = 14;
text_cfg.h_align = HorizontalAlign::Center;
text_cfg.v_align = VerticalAlign::Middle;
button->SetText(text_cfg);
```

### ToggleButton

Extends Button with toggle state:
```cpp
toggle_button.SetToggled(true);
toggle_button.IsToggled();
toggle_button.OnToggleChanged = [](ToggleButton* btn, bool is_toggled) { };
```

### InputBox

```cpp
auto input = std::make_unique<UI::InputBox>(x, y, width, height);

UI::InputBox::Config cfg;
cfg.font_index = FontFamily::Default;
cfg.font_size = 14;
cfg.max_length = 16;
cfg.placeholder_text = "Enter name...";
cfg.password_mode = false;
input->SetConfig(cfg);

// Events
input->OnTextChanged = [](InputBox* box) { };
input->OnEnterPressed = [](InputBox* box) { };
input->OnFocused = [](InputBox* box) { };

// Methods
input->GetText();
input->SetText("value");
input->Focus();
input->Blur();
input->SetAllowedCharacters("abcdefghijklmnopqrstuvwxyz");
```

### Label

```cpp
UI::Label::Config cfg;
cfg.text = "Hello World";
cfg.font_index = FontFamily::Default;
cfg.font_size = 12;
cfg.color = raylib::WHITE;
cfg.h_align = HorizontalAlign::Left;
cfg.auto_size = true;       // Bounds = text size
cfg.word_wrap = false;      // Requires auto_size = false
label.SetConfig(cfg);
```

### ScissorStack (Clipping)

```cpp
// Manual push/pop
ScissorStack::Push(x, y, width, height);
// ... drawing code (clipped to region)
ScissorStack::Pop();

// RAII helper
{
    ScissorStack::ScopedScissor scissor(x, y, width, height);
    // ... drawing code
} // Auto-pops
```

## Entity System

**Entity** (`entity.h/cpp`) - Base class with:
- `GamePosition` - Dual tile/pixel coordinate tracking
- 8-directional movement with pathfinding
- Animation state management

**Player** (`player.h/cpp`) - Player character with equipment, stats, appearance

**EntityManager** - Per-map entity container

**StaticEntityManager** - For display-only entities (character selection preview)

## Key Constants (`global_constants.h`)

```cpp
namespace constant {
    inline constexpr int BASE_WIDTH = 640;
    inline constexpr int BASE_HEIGHT = 480;
    inline constexpr int UPSCALE_FACTOR = 8;
    inline constexpr uint32_t TILE_SIZE = 32;
}
```

## Coordinate System

- **Tile size:** 32 pixels
- **GamePosition:** Tracks both tile coordinates and pixel position
- **Literals:** `10_t` (tiles), `64_p` (pixels)

## Rendering

- **Base resolution:** 640x480, upscaled 8x
- **Depth sorting:** Shadows → Map objects → Entity shadows → Entities (Y-sorted)
- **Sprite loading:** Lazy-loaded from PAK files, auto-unloaded after 60 seconds

## Adding New Components

### New Scene
1. Create `MyScene.h/cpp` inheriting from `Scene`
2. Add `SCENE_TYPE(MyScene)` macro in class
3. Implement `on_initialize()`, `on_uninitialize()`, `on_update()`, `on_render()`
4. Transition via `scene_manager.set_scene<MyScene>()`

### New UI Control
1. Create class inheriting from `UI::Control`
2. Implement `Render()` method
3. Optionally override `Update()` (call base first)
4. Use `FontSystem::draw_text_aligned()` for text rendering

## PAK File Format (`PAK.h`)

PAK files are custom sprite archives containing multiple sprites with frame rectangles and image data.

### File Structure
```
[File Header]        20 bytes: "<Pak file header>" + 3 bytes padding
[Sprite Count]        4 bytes: uint32_t (little-endian)
[Sprite Entries]      8 bytes each × sprite_count
  - offset: uint32_t  (byte position in file)
  - size: uint32_t    (total sprite data size)

[Sprite Data] (for each sprite):
  [Sprite Header]   100 bytes: "<Sprite File Header>" + 80 bytes padding
  [Rect Count]        4 bytes: uint32_t
  [Sprite Rects]     12 bytes each × rect_count
    - x, y, width, height: uint16_t
    - pivotX, pivotY: int16_t
  [Padding]           4 bytes
  [Image Data]        remaining bytes (PNG/JPEG)
```

### Loading Functions
```cpp
// Load entire PAK
PAKLib::pak pak = PAKLib::loadpak("path/to/file.pak");
PAKLib::pak pak = PAKLib::loadpak_fast("path/to/file.pak");  // Skip validation

// Load single sprite by index
PAKLib::sprite sprite = PAKLib::get_sprite("path/to/file.pak", sprite_index);
PAKLib::sprite sprite = PAKLib::get_sprite_fast("path/to/file.pak", sprite_index);

// Encrypted variants (with XOR + rotation cipher)
PAKLib::pak pak = PAKLib::loadpak_encrypted("path/to/file.pak", key);
PAKLib::sprite sprite = PAKLib::get_sprite_encrypted("path/to/file.pak", index, key);
```

### Sprite Rectangle Structure
Each `sprite_rect` defines a frame within the sprite atlas:
- `x, y` - Position in the atlas image
- `width, height` - Frame dimensions
- `pivotX, pivotY` - Anchor point offset for rendering

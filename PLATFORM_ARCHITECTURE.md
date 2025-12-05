# Platform Architecture Design Document

**Date:** 2025-12-05
**Status:** Design Phase - Ready for Implementation
**Goal:** Create true platform abstraction where game logic is completely platform-agnostic

---

## CONTEXT FOR CLAUDE (Internal Notes)

**What happened:** User asked about managing web vs desktop branches. I explored the codebase and discovered the architecture is ALREADY well-separated - game logic is clean, platform-specific code is minimal and concentrated in a few files (Application.cpp, Window.cpp, Game.cpp, main.cpp). The user had the KEY INSIGHT: game should never care about platform, only react to state changes through consistent interfaces.

**User's vision:** Game calls methods like `cursor.hide()` and doesn't care if that's implemented via raylib native calls or CSS manipulation. Translation layer handles platform differences transparently. Game only asks "what platform?" in rare edge cases.

**Critical understanding:** This is NOT about creating separate branches. This is about creating a clean platform abstraction layer so everything lives in ONE branch with platform-specific code isolated in dedicated modules that implement common interfaces.

**Current platform-specific touchpoints:**
- `Application.cpp:206-216` - main loop (emscripten_set_main_loop vs while)
- `Window.cpp:35-36` - FPS limiting (disabled on web)
- `main.cpp` - window dimensions (1024x768 vs 1920x1080)
- `Game.cpp:44-53` - cursor + periodic timer thread
- `raylib_include.h` - header selection (keep as-is)

**What needs to happen:**
1. Create Platform/ directory with interface definitions
2. Implement Desktop/Web concrete implementations
3. Refactor Game.cpp to remove ALL #ifdef, use injected IPlatformServices
4. Factory pattern in main.cpp creates correct platform at compile-time
5. Application holds platform services, passes to Game layer

**Key files to modify:**
- NEW: Platform/{interfaces}, Platform/Desktop/{impls}, Platform/Web/{impls}
- MODIFY: Game.cpp (remove #ifdef, inject platform)
- MODIFY: Application.h/cpp (hold IPlatformServices, may need some refactoring)
- MODIFY: main.cpp (use PlatformFactory)

**Don't touch:** All scenes, entities, rendering systems - they're already pure game logic

**Testing strategy:** After each interface implementation, verify BOTH desktop and web builds still work identically to current behavior. Incremental migration is key.

---

## Vision

The game layer should **never know or care** about what platform it's running on. Whether web or desktop, the game should just work because all platform-specific behavior is hidden behind consistent interfaces. The game only explicitly asks "what platform am I on?" in rare edge cases.

## Core Principles

1. **Dependency Inversion** - Game depends on abstractions, not concrete platforms
2. **Behavioral Equivalence** - Same method calls work identically across platforms
3. **Zero Platform Leakage** - No `#ifdef __EMSCRIPTEN__` in game logic
4. **Translation Layer** - Platform handles input/output differences transparently
5. **Single Branch** - All platforms coexist in one codebase

## Current State Analysis

### Existing Separation (Good)
- Game logic (scenes, entities, systems) is already isolated
- Application/Window layer handles platform initialization
- Main loop abstraction already exists (`emscripten_set_main_loop` vs `while`)

### Platform-Specific Code Locations
Currently scattered with `#ifdef` guards:

1. **Application.cpp** - Main loop mechanism (web callback vs native loop)
2. **Window.cpp** - FPS limiting (disabled on web)
3. **main.cpp** - Window dimensions (1024x768 web vs 1920x1080 desktop)
4. **Game.cpp:44-53** - Cursor hiding/locking and periodic timer thread
5. **raylib_include.h** - Platform-specific raylib headers

### Game Logic (Platform-Agnostic)
All scenes, entities, rendering systems, and game logic are **already clean** and platform-independent.

## Proposed Architecture

```
┌─────────────────────────────────────┐
│         Game Layer                  │
│  (Scenes, Entities, Game Logic)     │
│                                     │
│  - Never knows about platform       │
│  - Works with abstractions only     │
│  - Zero #ifdef statements           │
└──────────────┬──────────────────────┘
               │ depends on
               ▼
┌─────────────────────────────────────┐
│    Platform Abstraction Layer       │
│         (Interfaces)                │
│                                     │
│  IPlatformServices                  │
│  ├── ICursorManager                 │
│  ├── IInputTranslator               │
│  ├── ITimerService                  │
│  └── IPlatformInfo                  │
└──────────────┬──────────────────────┘
               │ implemented by
         ┌─────┴─────┐
         ▼           ▼
┌─────────────┐ ┌─────────────┐
│  Desktop    │ │    Web      │
│ Platform    │ │  Platform   │
│             │ │             │
│ - Real      │ │ - Browser   │
│   cursor    │ │   cursor    │
│   lock      │ │   (CSS)     │
│ - Threads   │ │ - No threads│
│ - Native    │ │ - Emscripten│
│   timers    │ │   timers    │
└─────────────┘ └─────────────┘
```

## Interface Definitions

### IPlatformServices (Main Interface)
```cpp
enum class PlatformType {
    Desktop,
    Web,
    Unknown
};

class IPlatformServices {
public:
    virtual ~IPlatformServices() = default;

    virtual ICursorManager& getCursorManager() = 0;
    virtual IInputTranslator& getInputTranslator() = 0;
    virtual ITimerService& getTimerService() = 0;

    // Only call if absolutely necessary
    virtual PlatformType getPlatformType() const = 0;
};
```

### ICursorManager (Cursor Abstraction)
```cpp
class ICursorManager {
public:
    virtual ~ICursorManager() = default;

    virtual void hide() = 0;
    virtual void show() = 0;
    virtual void lock() = 0;    // Desktop: actual lock, Web: no-op or CSS
    virtual void unlock() = 0;
    virtual bool isLocked() const = 0;
    virtual bool isVisible() const = 0;
};
```

**Desktop Implementation:**
- Uses `HideCursor()`, `ShowCursor()`, `rlx::LockCursor()`
- Real cursor confinement to window

**Web Implementation:**
- Uses CSS cursor properties
- No actual pointer lock (removed for point-and-click gameplay)
- Methods are no-ops or update CSS only

### IInputTranslator (Input Translation)
```cpp
struct MousePosition {
    float x, y;
};

enum class KeyCode {
    // Platform-agnostic key codes
    // Maps to raylib keys internally
};

class IInputTranslator {
public:
    virtual ~IInputTranslator() = default;

    virtual MousePosition getMousePosition() = 0;
    virtual bool isKeyPressed(KeyCode key) = 0;
    virtual bool isKeyDown(KeyCode key) = 0;
    virtual bool isMouseButtonPressed(int button) = 0;

    // Translate platform-specific input quirks
    virtual void translateInput(InputEvent& event) = 0;
};
```

**Desktop Implementation:**
- Direct raylib input pass-through
- Native keyboard shortcuts work

**Web Implementation:**
- Blocks browser shortcuts (Ctrl+T, Ctrl+N, Ctrl+W, function keys)
- Handles canvas-specific mouse coordinate translation
- Prevents accidental navigation

### ITimerService (Periodic Events)
```cpp
using TimerID = uint32_t;
using TimerCallback = std::function<void()>;

class ITimerService {
public:
    virtual ~ITimerService() = default;

    virtual TimerID scheduleRepeating(TimerCallback callback, float intervalSeconds) = 0;
    virtual void scheduleOnce(TimerCallback callback, float delaySeconds) = 0;
    virtual void cancel(TimerID id) = 0;
    virtual void cancelAll() = 0;
};
```

**Desktop Implementation:**
- Uses `std::thread` with periodic sleep
- Background thread executes callbacks
- Example: Sprite unloading every 1 second

**Web Implementation:**
- Frame-based timing (no threads in browser)
- Accumulates delta time, fires when interval reached
- Alternative: `emscripten_async_call()` for simple cases

### IPlatformInfo (Platform Metadata)
```cpp
struct PlatformCapabilities {
    bool supportsThreading;
    bool supportsPointerLock;
    bool supportsFullscreen;
    int maxTextureSize;
    // etc.
};

class IPlatformInfo {
public:
    virtual ~IPlatformInfo() = default;

    virtual PlatformType getType() const = 0;
    virtual const char* getName() const = 0;
    virtual PlatformCapabilities getCapabilities() const = 0;
};
```

## File Structure

```
Sources/HelbreathRaylib/
├── Platform/
│   ├── IPlatformServices.h          - Main platform interface
│   ├── ICursorManager.h             - Cursor abstraction
│   ├── IInputTranslator.h           - Input translation
│   ├── ITimerService.h              - Timer/periodic events
│   ├── IPlatformInfo.h              - Platform metadata
│   │
│   ├── Desktop/
│   │   ├── DesktopPlatform.h/cpp    - Desktop implementation
│   │   ├── DesktopCursorManager.h/cpp
│   │   ├── DesktopInputTranslator.h/cpp
│   │   ├── DesktopTimerService.h/cpp
│   │   └── DesktopPlatformInfo.h/cpp
│   │
│   ├── Web/
│   │   ├── WebPlatform.h/cpp        - Web implementation
│   │   ├── WebCursorManager.h/cpp
│   │   ├── WebInputTranslator.h/cpp
│   │   ├── WebTimerService.h/cpp
│   │   └── WebPlatformInfo.h/cpp
│   │
│   └── PlatformFactory.h/cpp        - Creates correct platform
│
├── Application.h/cpp                 - Uses IPlatformServices
├── Game.h/cpp                        - Uses IPlatformServices (no #ifdef!)
├── main.cpp                          - Creates platform via factory
└── [All game logic unchanged]
```

## Usage Example

### Before (Current):
```cpp
// Game.cpp - Platform knowledge leaked in!
void Game::on_attach() {
#ifndef __EMSCRIPTEN__
    HideCursor();
    rlx::LockCursor();

    // Periodic timer thread
    std::thread periodic_timer([this]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            this->unloadUnusedSprites();
        }
    });
    periodic_timer.detach();
#endif
}
```

### After (Proposed):
```cpp
// Game.cpp - Completely platform-agnostic!
void Game::on_attach() {
    // Platform services injected via constructor or Application
    auto& cursor = _platform->getCursorManager();
    cursor.hide();
    cursor.lock();

    // Timer service works identically on all platforms
    auto& timer = _platform->getTimerService();
    _spriteUnloadTimer = timer.scheduleRepeating([this]() {
        this->unloadUnusedSprites();
    }, 1.0f);

    // Only if truly needed (rare):
    if (_platform->getPlatformType() == PlatformType::Web) {
        // Edge case handling
    }
}
```

### Initialization (main.cpp):
```cpp
#include "Platform/PlatformFactory.h"

int main() {
    // Factory creates the right platform based on compile-time detection
    auto platform = PlatformFactory::create();

    WindowSpec spec;
    spec.width = platform->getDefaultWidth();   // 1024 web, 1920 desktop
    spec.height = platform->getDefaultHeight(); // 768 web, 1080 desktop

    Application::create_app_window(spec, std::move(platform));
    Application::push_layer<Game>();
    Application::run();

    return 0;
}
```

## Implementation Plan

### Phase 1: Create Interface Layer
1. Define all platform interfaces (`IPlatformServices`, `ICursorManager`, etc.)
2. Create `PlatformFactory` for platform creation
3. Update `Application` to accept and store `IPlatformServices`

### Phase 2: Implement Desktop Platform
1. `DesktopCursorManager` - Wrap existing raylib cursor functions
2. `DesktopInputTranslator` - Pass-through to raylib input
3. `DesktopTimerService` - Thread-based periodic execution
4. `DesktopPlatform` - Aggregate all desktop services

### Phase 3: Implement Web Platform
1. `WebCursorManager` - CSS-based cursor (no-op for lock)
2. `WebInputTranslator` - Browser shortcut blocking, coordinate translation
3. `WebTimerService` - Frame-based accumulation timing
4. `WebPlatform` - Aggregate all web services

### Phase 4: Refactor Game Layer
1. Remove all `#ifdef __EMSCRIPTEN__` from `Game.cpp`
2. Inject `IPlatformServices` into `Game` constructor
3. Replace direct platform calls with interface calls
4. Test both platforms

### Phase 5: Cleanup
1. Remove platform-specific code from `Game.cpp`
2. Update `Window.cpp` to use platform services if needed
3. Ensure `raylib_include.h` is only include-level abstraction
4. Update documentation

## Benefits

1. **Testability** - Mock platform implementations for unit tests
2. **Maintainability** - Platform code is isolated and easy to find
3. **Extensibility** - Adding mobile/console is just new implementations
4. **Clarity** - Game logic is pure, no platform noise
5. **Single Branch** - No code duplication across branches
6. **Future-Proof** - New platforms require zero game logic changes

## Edge Cases & Considerations

### When Game Needs Platform Info
- **Rare:** Only when behavior truly differs (not just implementation)
- **Example:** Web-specific asset loading optimizations
- **Solution:** Query `getPlatformType()` explicitly, document why

### Input Translation Layer
- **Desktop:** Native shortcuts work normally
- **Web:** Block Ctrl+T, Ctrl+N, Ctrl+W, F-keys to prevent navigation
- **Both:** Game receives consistent input events

### Window Configuration
- **Desktop:** 1920x1080, fullscreen, borderless
- **Web:** 1024x768, canvas-contained
- **Solution:** Platform provides default `WindowSpec`

### FPS Limiting
- **Desktop:** `SetTargetFPS(60)` or similar
- **Web:** Browser controls frame rate, no explicit limit
- **Solution:** Platform controls FPS policy in `Application::run()`

## Migration Strategy

1. **Incremental Migration** - Start with one interface (e.g., `ICursorManager`)
2. **Keep Existing Code Working** - Run both old and new paths in parallel initially
3. **Test Each Platform** - Validate desktop and web after each interface
4. **Remove Old Code** - Once all interfaces implemented, remove `#ifdef` blocks
5. **Single Branch** - Everything stays in one codebase

## Success Criteria

- [ ] Zero `#ifdef __EMSCRIPTEN__` in game logic files
- [ ] Game layer only depends on platform interfaces
- [ ] Desktop build works identically to current
- [ ] Web build works identically to current
- [ ] New platform can be added without touching game code
- [ ] All platform-specific code isolated in `Platform/` directory

## Notes

- Keep raylib abstraction minimal - most raylib APIs are already cross-platform
- Platform layer should be thin - just handle true platform differences
- Don't over-abstract - only create interfaces for actual platform variance
- Game logic should never include `<emscripten.h>` or Windows-specific headers

## References

- Current codebase state documented in exploration (2025-12-05)
- Emscripten documentation: https://emscripten.org/
- Raylib cross-platform guide: https://www.raylib.com/

#pragma once
#include <cstdint>
#include <string>
#include <memory>
#include "Animation.h"
#include "SpriteIDs.h"
#include "global_constants.h"
#include "Colors.h"
#include "FontIDs.h"
#include "Sprite.h"
#include <unordered_set>
#include "Application.h"
#include "EntityGUID.h"
#include "EntityStats.h"

// Entity behavior types - determines how the entity reacts to the player
enum class EntityBehavior : uint8_t {
    Friendly,   // Ignores player, usually for NPCs like shopkeepers
    Neutral,    // Ignores player unless attacked, then becomes hostile
    Hostile     // Actively follows and attacks the player
};

// Entity movement patterns - determines how the entity moves in the world
enum class EntityMovementType : uint8_t {
    Stationary, // Does not move on its own
    Random,     // Moves randomly within an area
    Waypoint    // Follows predefined waypoints
};

enum Dir : uint8_t {
    NORTH, NORTH_EAST, EAST, SOUTH_EAST, SOUTH, SOUTH_WEST, WEST, NORTH_WEST, DIRECTION_COUNT
};

enum Stance : uint8_t {
    PEACE, COMBAT, STANCE_COUNT
};

enum AnimationType : uint8_t {
    STAND, WALK, RUN, ATTACK, PICKUP, TAKEDAMAGE, DYING
};

enum WeaponUsed : uint8_t {
    HAND, BOW, CAST
};

// Item classification - used for sound selection, damage types, and item behavior
enum class ItemClass : uint8_t {
    NONE,           // No item / default
    WEAPON,         // Weapons (swords, bows, wands, etc.)
    ARMOR,          // Protective gear (chest, legs, helm, etc.)
    SHIELD,         // Shields and off-hand defensive items
    ACCESSORY,      // Rings, amulets, capes
    CONSUMABLE,     // Potions, scrolls, food
    MATERIAL,       // Crafting materials
    QUEST           // Quest items
};

// Item subclass - more specific categorization for sound/animation selection
enum class ItemSubclass : uint8_t {
    NONE,           // Default / not applicable

    // Weapon subclasses (determines attack sounds)
    SWORD,          // Standard swords - sharp weapon sounds
    LARGE_SWORD,    // Two-handed/heavy swords - heavy swing sounds
    AXE,            // Axes - sharp weapon sounds
    LARGE_AXE,      // Two-handed axes - heavy swing sounds
    MACE,           // Maces/hammers - blunt weapon sounds
    LARGE_MACE,     // Two-handed maces - heavy blunt sounds
    DAGGER,         // Daggers - quick slash sounds
    SPEAR,          // Spears/polearms
    BOW,            // Bows - uses BOW animation
    CROSSBOW,       // Crossbows - uses BOW animation
    WAND,           // Wands - uses CAST animation
    STAFF,          // Staves - uses CAST animation

    // Armor subclasses
    CLOTH,          // Cloth/robes - light armor
    LEATHER,        // Leather armor - light armor
    CHAIN,          // Chainmail - medium armor
    PLATE,          // Plate armor - heavy armor

    // Consumable subclasses
    POTION,         // Healing/mana potions
    SCROLL,         // Spell scrolls
    FOOD            // Consumable food items
};

// Helper to determine WeaponUsed from ItemSubclass (for animation selection)
inline WeaponUsed get_weapon_used_from_subclass(ItemSubclass subclass) {
    switch (subclass) {
    case ItemSubclass::BOW:
    case ItemSubclass::CROSSBOW:
        return WeaponUsed::BOW;
    case ItemSubclass::WAND:
    case ItemSubclass::STAFF:
        return WeaponUsed::CAST;
    default:
        return WeaponUsed::HAND;  // All melee weapons use HAND animation
    }
}

// Special attack result values (negative = special, positive = damage dealt)
namespace AttackResult {
    constexpr int MISS = -1;        // Attack missed
    // Future: DODGE = -2, BLOCK = -3, etc.
}

class Game;
class CMapData;
struct ItemMetadataEntry;

// Floating damage indicator
struct DamageIndicator {
    int damage = 0;
    float time_remaining = 1.5f;    // seconds until removed
    float offset_y = 0.0f;          // current vertical offset
    float velocity_y = 0.0f;        // current velocity for physics-based movement
    float alpha = 1.0f;             // opacity (1.0 = fully visible)
    bool is_bumped = false;         // true if pushed up by newer indicator
    bool is_settling = true;        // true while new indicator is sliding into place
    float bounce_time = 0.0f;       // remaining bounce/rubberband duration

    static constexpr float MAX_DURATION = 1.5f;
    static constexpr float BUMP_FADE_SPEED = 3.0f;   // fade out speed when bumped
    static constexpr float BUMP_INITIAL_VELOCITY = -150.0f; // initial upward velocity when bumped (negative = up)
    static constexpr float BUMP_DECELERATION = 250.0f;  // how fast velocity decreases
    static constexpr float SETTLE_START_OFFSET = 8.0f;  // new indicators start this many pixels below
    static constexpr float SETTLE_SPEED = 60.0f;        // pixels per second to settle into place
    static constexpr float BOUNCE_DURATION = 0.12f;     // how long the rubberband bounce lasts
    static constexpr float BOUNCE_INTENSITY = 2.0f;     // max pixels of vertical bounce
};

// Helper types for distinguishing between tile and pixel coordinates
struct TileCoord {
    uint16_t value;
    explicit constexpr TileCoord(uint16_t v) : value(v) {}
};

struct PixelCoord {
    uint32_t value;
    explicit constexpr PixelCoord(uint32_t v) : value(v) {}
};

// User-defined literals for convenient coordinate specification
namespace position_literals {
    constexpr TileCoord operator""_t(unsigned long long v) {
        return TileCoord(static_cast<uint16_t>(v));
    }

    constexpr PixelCoord operator""_p(unsigned long long v) {
        return PixelCoord(static_cast<uint32_t>(v));
    }
}

struct GamePosition {
public:
    GamePosition() = default;

    // Original constructor - assumes tile coordinates
    GamePosition(uint16_t t_x, uint16_t t_y)
        : tile_x(t_x), tile_y(t_y),
        pixel_x(t_x * constant::TILE_SIZE), pixel_y(t_y * constant::TILE_SIZE) {
    }

    // Tile-based constructor using TileCoord
    GamePosition(TileCoord t_x, TileCoord t_y)
        : tile_x(t_x.value), tile_y(t_y.value),
        pixel_x(t_x.value * constant::TILE_SIZE), pixel_y(t_y.value * constant::TILE_SIZE) {
    }

    // Pixel-based constructor using PixelCoord
    GamePosition(PixelCoord p_x, PixelCoord p_y)
        : tile_x(static_cast<uint16_t>(p_x.value / constant::TILE_SIZE)),
          tile_y(static_cast<uint16_t>(p_y.value / constant::TILE_SIZE)),
          pixel_x(p_x.value), pixel_y(p_y.value) {
    }

    // Mixed constructor - tile X, pixel Y
    GamePosition(TileCoord t_x, PixelCoord p_y)
        : tile_x(t_x.value),
          tile_y(static_cast<uint16_t>(p_y.value / constant::TILE_SIZE)),
          pixel_x(t_x.value * constant::TILE_SIZE), pixel_y(p_y.value) {
    }

    // Mixed constructor - pixel X, tile Y
    GamePosition(PixelCoord p_x, TileCoord t_y)
        : tile_x(static_cast<uint16_t>(p_x.value / constant::TILE_SIZE)),
          tile_y(t_y.value),
          pixel_x(p_x.value), pixel_y(t_y.value * constant::TILE_SIZE) {
    }

    uint16_t get_tile_x() const { return tile_x; }
    uint16_t get_tile_y() const { return tile_y; }
    uint32_t get_pixel_x() const { return pixel_x; }
    uint32_t get_pixel_y() const { return pixel_y; }

    void set_pixel_position(uint32_t px, uint32_t py) {
        pixel_x = px;
        pixel_y = py;
    }

    bool operator==(const GamePosition& other) const {
        return tile_x == other.tile_x && tile_y == other.tile_y;
    }

    bool operator!=(const GamePosition& other) const {
        return !(*this == other);
    }

protected:
    uint16_t tile_x = 0;
    uint16_t tile_y = 0;
    uint32_t pixel_x = 0;
    uint32_t pixel_y = 0;
};

inline GamePosition get_tile_world_mouse_position(raylib::Camera2D camera)
{
    raylib::Vector2 mposWorld = GetScreenToWorld2D(rlx::get_mouse_position(), camera);
    int tx = (int)floorf((mposWorld.x + constant::TILE_HALF) / constant::TILE_SIZE);
    int ty = (int)floorf((mposWorld.y + constant::TILE_HALF) / constant::TILE_SIZE);
    tx = std::max(0, tx);
    ty = std::max(0, ty);
    return GamePosition(tx, ty);
}

inline Dir get_direction_to_point(const GamePosition& position, const GamePosition& desired_position)
{
    int dx = desired_position.get_tile_x() - position.get_tile_x();
    int dy = desired_position.get_tile_y() - position.get_tile_y();

    int abs_dx = std::abs(dx);
    int abs_dy = std::abs(dy);

    // Determine primary axis
    if (abs_dx > abs_dy)
    {
        // Horizontal dominant
        if (dx > 0)
        {
            // East quadrants
            if (dy > abs_dx / 2) return Dir::SOUTH_EAST;
            if (dy < -abs_dx / 2) return Dir::NORTH_EAST;
            return Dir::EAST;
        }
        else
        {
            // West quadrants
            if (dy > abs_dx / 2) return Dir::SOUTH_WEST;
            if (dy < -abs_dx / 2) return Dir::NORTH_WEST;
            return Dir::WEST;
        }
    }
    else
    {
        // Vertical dominant
        if (dy > 0)
        {
            // South quadrants
            if (dx > abs_dy / 2) return Dir::SOUTH_EAST;
            if (dx < -abs_dy / 2) return Dir::SOUTH_WEST;
            return Dir::SOUTH;
        }
        else
        {
            // North quadrants
            if (dx > abs_dy / 2) return Dir::NORTH_EAST;
            if (dx < -abs_dy / 2) return Dir::NORTH_WEST;
            return Dir::NORTH;
        }
    }
}

class Entity {
public:
    Entity();
    virtual ~Entity() = default;

    // Disable copy and move semantics
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;
    Entity(Entity&&) = delete;
    Entity& operator=(Entity&&) = delete;

    void update();
    virtual void on_update() = 0;
    virtual void on_render() const = 0;
	virtual void on_render_shadow() const = 0;
    virtual void set_animation(AnimationType new_type, WeaponUsed new_weapon = WeaponUsed::HAND) = 0;

    void set_position(const GamePosition& new_position) {
        position = new_position;
    }

    const GamePosition& get_position() const {
        return position;
    }

    void attach_camera(raylib::Camera2D& camera)
    {
        _attached_camera = &camera;
    }

    void detach_camera()
    {
        _attached_camera = nullptr;
    }

    void set_active_map(CMapData* map);
    std::string get_active_map_identifier() const;

    void move_to(const GamePosition& target, bool run = false);
    void dash_to(const GamePosition& target);  // Fast single-tile movement
    GamePosition find_valid_target(const GamePosition& target);
    bool is_tile_occupied(int tile_x, int tile_y) const;
    bool is_moving() const { return _is_moving; }
    bool is_dashing() const { return _is_dashing; }
    void stop_movement();
    void render_debug_movement();

    Dir get_current_direction() const { return current_direction; }
    void set_direction(Dir direction) { current_direction = direction; }
	AnimationType get_current_animation_type() const { return current_animation_type; }

    // Get the entity's unique GUID
    EntityGUID get_guid() const { return _guid; }

    // Display name
    void set_display_name(const std::string& name) { _display_name = name; }
    const std::string& get_display_name() const { return _display_name; }

    // Behavior and movement
    void set_behavior(EntityBehavior behavior) { _behavior = behavior; }
    EntityBehavior get_behavior() const { return _behavior; }
    void set_movement_type(EntityMovementType type) { _movement_type = type; }
    EntityMovementType get_movement_type() const { return _movement_type; }
    void set_spawn_point(const GamePosition& pos) { _spawn_point = pos; }
    const GamePosition& get_spawn_point() const { return _spawn_point; }
    void set_movement_radius(int radius) { _movement_radius = radius; }
    int get_movement_radius() const { return _movement_radius; }

    // Targeting - check if a world position is within entity bounds
    bool is_point_over_entity(int world_x, int world_y) const;

    // Check if entity can be targeted (not dead/dying)
    bool is_targetable() const { return !_is_dying && !_is_dead; }

    // Render the entity's nameplate (display name + GUID)
    void render_nameplate() const;

    // Damage indicators
    void spawn_damage_indicator(int damage);
    void update_damage_indicators(float delta_time);
    void render_damage_indicators() const;

    // Combat system - attack flow
    // Called when this entity attacks another entity
    // Override on_before_attack() to modify damage or add effects before attacking
    // Override on_after_attack() to trigger effects after the attack completes
    void attack(Entity* target);

    // Called when this entity is attacked by another
    // Override on_receive_attack() to customize damage taken, trigger effects, etc.
    void receive_attack(Entity* attacker, int damage);

    // Check if this entity is in attack range of target (adjacent tile)
    bool is_in_attack_range_of(const Entity* target) const;

    // Death system
    bool is_dying() const { return _is_dying; }
    bool is_dead() const { return _is_dead; }
    bool should_remove() const { return _should_remove; }
    float get_dead_opacity() const { return _dead_opacity; }

    // Get entity stats
    EntityStats& get_stats() { return *_stats; }
    const EntityStats& get_stats() const { return *_stats; }

    // Get the entity that killed this one (if any)
    Entity* get_killed_by() const { return _killed_by_entity; }

protected:
    // Override these in derived classes to customize attack behavior

    // Called before this entity attacks - return the damage to deal
    // Base implementation returns a default damage value
    virtual int on_before_attack(Entity* target) { return 10; }

    // Called after this entity completes an attack
    virtual void on_after_attack(Entity* target) {}

    // Called when this entity receives an attack - return true if the attack was handled
    // Base implementation just logs the damage
    virtual bool on_receive_attack(Entity* attacker, int damage) { return true; }

    // Movement hooks - override to customize movement behavior
    // Called before movement is updated each frame (useful for capturing position before it changes)
    virtual void on_before_movement_update() {}

    // Called when entity reaches a tile during movement. Use set_tile_pause() to delay next step.
    virtual void on_tile_reached() {}

    // Set a pause duration before moving to the next tile (call from on_tile_reached)
    void set_tile_pause(float seconds) { _tile_pause_remaining = seconds; _is_paused_at_tile = true; }

    // Dash movement hooks - override to customize dash behavior
    // Called when dash movement completes (entity arrived at dash destination)
    virtual void on_dash_complete() {}

    // Death hooks - override to customize death behavior
    // Return false from on_before_* to cancel the death sequence
    virtual bool on_before_dying(Entity* attacker) { return true; }
    virtual void on_after_dying(Entity* attacker) {}
    virtual bool on_before_death() { return true; }
    virtual void on_after_death() {}

    // Internal death state methods
    void _on_hit(Entity* attacker, int damage);
    void _on_dying(Entity* attacker);
    void _on_death();
    void _update_death();

    Game& _game;
    CSpriteCollection& _model_sprites;
    CSpriteCollection& _entity_sprites;
	std::vector<ItemMetadataEntry>& _item_metadata;

    // Unique identifier for this entity
    EntityGUID _guid;

    // Display name shown when targeted
    std::string _display_name = "Unknown";

    // AI behavior and movement
    EntityBehavior _behavior = EntityBehavior::Neutral;
    EntityMovementType _movement_type = EntityMovementType::Stationary;
    GamePosition _spawn_point{};
    int _movement_radius = 5;  // Tiles from spawn point for random movement
    double _next_random_move_time = 0.0;  // When to pick next random destination

    // Damage indicators
    std::vector<DamageIndicator> _damage_indicators{};

    // Stats
    std::unique_ptr<EntityStats> _stats;

    // Death state
    bool _is_dying = false;
    bool _is_dead = false;
    bool _should_remove = false;
    float _dead_opacity = 1.0f;
    Entity* _killed_by_entity = nullptr;

    // Death timing
    float _death_opacity_timer = 0.0f;
    float _death_opacity_interval = 0.025f;  // How fast opacity decreases
    float _dying_wait_timer = 0.0f;
    float _dying_wait_interval = 2.0f;  // Seconds to wait after dying animation before fade

    Animation current_animation{};
    Dir current_direction = NORTH;
    AnimationType current_animation_type = STAND;

    GamePosition position{};

    raylib::Camera2D* _attached_camera = nullptr;

    // Movement system
    bool _is_moving = false;
    std::vector<GamePosition> _movement_path{};
    std::vector<GamePosition> _next_movement_path{};
    size_t _current_path_index = 0;
    GamePosition _current_step_target{};
    float _move_progress = 0.0f;
    int32_t _move_start_pixel_x = 0;
    int32_t _move_start_pixel_y = 0;
    float _base_speed = 1.0f;
    float _internal_speed_multiplier = 100.0f;
    bool _is_running = false;
    bool _is_dashing = false;  // Fast single-tile movement for dash-attacks
    //bool _can_cancel_movement = false;
	bool _stop_requested = false;
    GamePosition _final_target{};
    float _current_step_distance = constant::TILE_SIZE;
    float _tile_pause_remaining = 0.0f;  // Time remaining before next step
    bool _is_paused_at_tile = false;     // Currently pausing between tiles

    std::unordered_set<uint64_t>* _reserved_tiles = nullptr; // Shared across all entities
    uint64_t _reserved_tile_key = 0; // Current reservation

    static uint64_t _make_tile_key(int x, int y) {
        return ((uint64_t)x << 32) | (uint64_t)y;
    }

    // Entity Bounds
	virtual PAKLib::sprite_rect get_entity_bounds() const = 0;

    // Map bounds
    CMapData* _active_map;
    int16_t _map_width = 0;
    int16_t _map_height = 0;

    void _update_movement();
    void _request_stop_movement();
    void _build_path(std::vector<GamePosition>& path, const GamePosition& start, const GamePosition& end);
    bool _build_path_direct(std::vector<GamePosition>& path, const GamePosition& start, const GamePosition& end);
    void _start_next_step();
};
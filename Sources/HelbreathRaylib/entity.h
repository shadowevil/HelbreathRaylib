#pragma once
#include <cstdint>
#include "Animation.h"
#include "SpriteIDs.h"
#include "global_constants.h"
#include "Colors.h"
#include "FontIDs.h"
#include "Sprite.h"
#include <unordered_set>
#include "Application.h"

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

class Game;
class CMapData;
struct ItemMetadataEntry;

struct GamePosition {
public:
    GamePosition() = default;
    GamePosition(uint16_t t_x, uint16_t t_y)
        : tile_x(t_x), tile_y(t_y),
        pixel_x(t_x* constant::TILE_SIZE), pixel_y(t_y* constant::TILE_SIZE) {
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

inline GamePosition get_tile_world_mouse_position(Camera2D camera)
{
    Vector2 mposWorld = GetScreenToWorld2D(rlx::get_mouse_position(), camera);
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

    void attach_camera(Camera2D& camera)
    {
        _attached_camera = &camera;
    }

    void detach_camera()
    {
        _attached_camera = nullptr;
    }

    void set_active_map(CMapData* map);

    void move_to(const GamePosition& target, bool run = false);
    GamePosition find_valid_target(const GamePosition& target);
    bool is_tile_occupied(int tile_x, int tile_y) const;
    bool is_moving() const { return _is_moving; }
    void stop_movement();
    void render_debug_movement();

    Dir get_current_direction() const { return current_direction; }
    void set_direction(Dir direction) { current_direction = direction; }

protected:
    Game& _game;
    CSpriteCollection& _model_sprites;
	std::vector<ItemMetadataEntry>& _item_metadata;
    std::vector<std::unique_ptr<Entity>>& _entities;

    Animation current_animation{};
    Dir current_direction = NORTH;
    AnimationType current_animation_type = STAND;

    GamePosition position{};

    Camera2D* _attached_camera = nullptr;

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
    //bool _can_cancel_movement = false;
	bool _stop_requested = false;
    GamePosition _final_target{};
    float _current_step_distance = constant::TILE_SIZE;

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
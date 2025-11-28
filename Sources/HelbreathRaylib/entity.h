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

inline GamePosition GetTileWorldMousePosition(Camera2D camera)
{
    Vector2 mposWorld = GetScreenToWorld2D(rlx::GetMousePosition(), camera);
    int tx = (int)floorf((mposWorld.x + constant::TILE_HALF) / constant::TILE_SIZE);
    int ty = (int)floorf((mposWorld.y + constant::TILE_HALF) / constant::TILE_SIZE);
    tx = std::max(0, tx);
    ty = std::max(0, ty);
    return GamePosition(tx, ty);
}

inline Dir GetDirectionToPoint(const GamePosition& position, const GamePosition& desired_position)
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

    void Update();
    virtual void OnUpdate() = 0;
    virtual void OnRender() const = 0;
	virtual void OnRenderShadow() const = 0;
    virtual void SetAnimation(AnimationType newType, WeaponUsed newWeapon = WeaponUsed::HAND) = 0;

    void SetPosition(const GamePosition& newPosition) {
        position = newPosition;
    }

    const GamePosition& GetPosition() const {
        return position;
    }

    void AttachCamera(Camera2D& camera)
    {
        m_attachedCamera = &camera;
    }

    void DetachCamera()
    {
        m_attachedCamera = nullptr;
    }

    void SetActiveMap(CMapData* map);

    void MoveTo(const GamePosition& target, bool run = false);
    GamePosition FindValidTarget(const GamePosition& target);
    bool IsTileOccupied(int tile_x, int tile_y) const;
    bool IsMoving() const { return m_isMoving; }
    void StopMovement();
    void RenderDebugMovement();

    Dir GetCurrentDirection() const { return current_direction; }
    void SetDirection(Dir direction) { current_direction = direction; }

protected:
    Game& m_game;
    CSpriteCollection& m_modelSprites;
	std::vector<ItemMetadataEntry>& m_itemMetadata;
    std::vector<std::unique_ptr<Entity>>& m_entities;

    Animation current_animation{};
    Dir current_direction = NORTH;
    AnimationType current_animation_type = STAND;

    GamePosition position{};

    Camera2D* m_attachedCamera = nullptr;

    // Movement system
    bool m_isMoving = false;
    std::vector<GamePosition> m_movementPath{};
    std::vector<GamePosition> m_nextMovementPath{};
    size_t m_currentPathIndex = 0;
    GamePosition m_currentStepTarget{};
    float m_moveProgress = 0.0f;
    int32_t m_moveStartPixelX = 0;
    int32_t m_moveStartPixelY = 0;
    float m_baseSpeed = 1.0f;
    float m_internalSpeedMultiplier = 100.0f;
    bool m_isRunning = false;
    //bool m_canCancelMovement = false;
	bool m_stopRequested = false;
    GamePosition m_finalTarget{};
    float m_currentStepDistance = constant::TILE_SIZE;

    std::unordered_set<uint64_t>* m_reservedTiles = nullptr; // Shared across all entities
    uint64_t m_reservedTileKey = 0; // Current reservation

    static uint64_t MakeTileKey(int x, int y) {
        return ((uint64_t)x << 32) | (uint64_t)y;
    }

    // Entity Bounds
	virtual PAKLib::sprite_rect GetEntityBounds() const = 0;

    // Map bounds
    CMapData* m_activeMap;
    int16_t m_mapWidth = 0;
    int16_t m_mapHeight = 0;

    void UpdateMovement();
    void RequestStopMovement();
    void BuildPath(std::vector<GamePosition>& path, const GamePosition& start, const GamePosition& end);
    bool BuildPathDirect(std::vector<GamePosition>& path, const GamePosition& start, const GamePosition& end);
    void StartNextStep();
};
#pragma once
#include "entities.h"
#include "raylib_include.h"
#include "Colors.h"
#include <memory>
#include <optional>
#include "entity.h"

// Manages static (non-moving) entities that periodically change animation and direction
// Useful for character preview screens, NPCs standing in place, etc.
class StaticEntityManager {
public:
    struct EntityConfig {
        AnimationType animation_type;
        WeaponUsed weapon_used;
        double direction_change_min_interval;  // Minimum seconds between direction changes
        double direction_change_max_interval; // Maximum seconds between direction changes
        bool randomize_direction;             // Whether to randomly change direction

        // Default constructor with default values
        EntityConfig()
            : animation_type(AnimationType::WALK)
            , weapon_used(WeaponUsed::HAND)
            , direction_change_min_interval(5.0)
            , direction_change_max_interval(10.0)
            , randomize_direction(true)
        {}
    };

    StaticEntityManager() = default;
    ~StaticEntityManager() = default;

    // Disable copy and move semantics
    StaticEntityManager(const StaticEntityManager&) = delete;
    StaticEntityManager& operator=(const StaticEntityManager&) = delete;
    StaticEntityManager(StaticEntityManager&&) = delete;
    StaticEntityManager& operator=(StaticEntityManager&&) = delete;

    // Add an entity to manage with optional config
    void Add(Entity* entity, const EntityConfig& config = EntityConfig()) {
        ManagedEntity managed;
        managed.entity = entity;
        managed.config = config;
        managed.next_direction_change = raylib::GetTime() + _get_random_interval(config);

        // Set initial animation
        entity->set_animation(config.animation_type, config.weapon_used);

        _entities.push_back(managed);
    }

    // Remove an entity from management
    void Remove(Entity* entity) {
        _entities.erase(
            std::remove_if(_entities.begin(), _entities.end(),
                [entity](const ManagedEntity& managed) {
                    return managed.entity == entity;
                }),
            _entities.end()
        );
    }

    // Update all managed entities
    void Update() {
        double current_time = raylib::GetTime();

        for (auto& managed : _entities) {
            Entity* entity = managed.entity;
            const EntityConfig& config = managed.config;

            // Ensure animation is set
            if (entity->get_current_animation_type() != config.animation_type) {
                entity->set_animation(config.animation_type, config.weapon_used);
            }

            // Handle direction changes
            if (config.randomize_direction && current_time >= managed.next_direction_change) {
                Dir new_direction = static_cast<Dir>(raylib::GetRandomValue(0, Dir::DIRECTION_COUNT - 1));
                entity->set_direction(new_direction);
                managed.next_direction_change = current_time + _get_random_interval(config);
            }

            // Update the entity (advances animation frames)
            entity->update();
        }
    }

    // Render all managed entities
    void Render() const {
        for (const auto& managed : _entities) {
            managed.entity->on_render();
        }
    }

    // Clear all managed entities
    void Clear() {
        _entities.clear();
    }

    // Get count of managed entities
    size_t Count() const {
        return _entities.size();
    }

    // Add a static player with optional or randomized appearance
    // If appearance is not provided, a random appearance will be generated
    Player* AddPlayer(Player* player, const GamePosition& position,
                      std::optional<PlayerAppearance> appearance = std::nullopt,
                      const EntityConfig& config = EntityConfig()) {
        // Generate or use provided appearance
        PlayerAppearance final_appearance;
        if (appearance.has_value()) {
            final_appearance = appearance.value();
        } else {
            // Randomize appearance
            final_appearance.gender = (raylib::GetRandomValue(0, 1) == 0 ? GENDER_MALE : GENDER_FEMALE);
            final_appearance.hair_style = static_cast<HairStyle>(raylib::GetRandomValue(0, HairStyle::HAIR_STYLE_COUNT - 1));
            final_appearance.hair_color_index = static_cast<HairColorIndex>(raylib::GetRandomValue(0, HairColorCount - 1));
            final_appearance.skin_color_index = static_cast<SkinColorIndex>(raylib::GetRandomValue(0, SkinColorCount - 1));
            final_appearance.underwear_color_index = static_cast<UnderwearColorIndex>(raylib::GetRandomValue(0, SkinColorCount - 1));
        }

        // Set random initial direction
        player->set_direction(static_cast<Dir>(raylib::GetRandomValue(0, Dir::DIRECTION_COUNT - 1)));

        // Update appearance and position
        player->update_appearance(final_appearance);
        player->set_position(position);

        // Add to manager
        Add(player, config);

        return player;
    }

private:
    struct ManagedEntity {
        Entity* entity = nullptr;
        EntityConfig config;
        double next_direction_change = 0.0;
    };

    std::vector<ManagedEntity> _entities;

    double _get_random_interval(const EntityConfig& config) const {
        double min_interval = config.direction_change_min_interval;
        double max_interval = config.direction_change_max_interval;

        if (min_interval >= max_interval) {
            return min_interval;
        }

        int min_ms = static_cast<int>(min_interval * 1000.0);
        int max_ms = static_cast<int>(max_interval * 1000.0);
        int random_ms = raylib::GetRandomValue(min_ms, max_ms);

        return random_ms / 1000.0;
    }
};

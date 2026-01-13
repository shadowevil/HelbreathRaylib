#pragma once
#include "entity.h"
#include "EntityIDs.h"

// Dummy entity - simple NPC/monster entity for testing
class EntityDummy : public Entity {
public:
	EntityDummy();
	~EntityDummy() override = default;

	void on_update() override;
	void on_render() const override;
	void on_render_shadow() const override;
	PAKLib::sprite_rect get_entity_bounds() const override;

	void set_animation(AnimationType new_type, WeaponUsed new_weapon = WeaponUsed::HAND) override;

	// Get the entity type
	static constexpr EntityID GetEntityType() { return EntityID::DUMMY; }

protected:
	// Combat overrides - customize attack behavior for this entity type
	int on_before_attack(Entity* target) override;
	void on_after_attack(Entity* target) override;
	bool on_receive_attack(Entity* attacker, int damage) override;

	// Movement overrides - pause briefly at each tile
	void on_tile_reached() override;

private:
	// Get the sprite index based on animation type and direction
	size_t _get_sprite_index() const;

	// Calculate the frame number within the sprite (0-3)
	size_t _get_frame_index() const;

	// Animation definitions for this entity (4 frames per sprite)
	const Animation _stand_animation = Animation(0, 4, 0.15);
	const Animation _walk_animation = Animation(0, 4, 0.1);
	const Animation _attack_animation = Animation(0, 4, 0.12);
	const Animation _takedamage_animation = Animation(0, 4, 0.1);
	const Animation _dying_animation = Animation(0, 4, 0.15, false);  // Don't loop - play once

	// Sprite offsets for each animation type (8 sprites per animation, one per direction)
	// Total: 5 animation types × 8 directions = 40 sprites
	static constexpr size_t SPRITE_OFFSET_STAND = 0;       // Sprites 0-7
	static constexpr size_t SPRITE_OFFSET_WALK = 8;        // Sprites 8-15
	static constexpr size_t SPRITE_OFFSET_ATTACK = 16;     // Sprites 16-23
	static constexpr size_t SPRITE_OFFSET_TAKEDAMAGE = 24; // Sprites 24-31
	static constexpr size_t SPRITE_OFFSET_DYING = 32;      // Sprites 32-39
};

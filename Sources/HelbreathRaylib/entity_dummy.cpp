#include "entity_dummy.h"
#include "Colors.h"
#include <random>

EntityDummy::EntityDummy()
	: Entity()
{
	// Initialize with default stand animation
	current_animation = _stand_animation;
	current_animation_type = AnimationType::STAND;

	// Set default display name and behavior
	_display_name = "Dummy";
	_behavior = EntityBehavior::Neutral;
	_movement_type = EntityMovementType::Stationary;
}

void EntityDummy::on_update()
{
	current_animation.update(raylib::GetTime());

	// Handle random movement
	if (_movement_type == EntityMovementType::Random && !is_moving())
	{
		double current_time = raylib::GetTime();
		if (current_time >= _next_random_move_time)
		{
			// Random delay between 1-4 seconds before next move
			static std::random_device rd;
			static std::mt19937 gen(rd());
			std::uniform_real_distribution<> delay_dist(1.0, 4.0);
			std::uniform_int_distribution<> offset_dist(-_movement_radius, _movement_radius);

			// Pick a random tile within radius of spawn point
			int target_x = _spawn_point.get_tile_x() + offset_dist(gen);
			int target_y = _spawn_point.get_tile_y() + offset_dist(gen);

			// Move to the random position
			GamePosition target_pos(target_x, target_y);
			move_to(target_pos, false);

			// Schedule next random move
			_next_random_move_time = current_time + delay_dist(gen);
		}
	}
}

void EntityDummy::on_render() const
{
	size_t sprite_index = _get_sprite_index();
	size_t frame_index = _get_frame_index();

	if (_entity_sprites[sprite_index])
	{
		_entity_sprites[sprite_index]->draw(
			position.get_pixel_x(),
			position.get_pixel_y(),
			frame_index,
			raylib::WHITE
		);
	}
}

void EntityDummy::on_render_shadow() const
{
	size_t sprite_index = _get_sprite_index();
	size_t frame_index = _get_frame_index();

	if (_entity_sprites[sprite_index])
	{
		_entity_sprites[sprite_index]->draw(
			position.get_pixel_x(),
			position.get_pixel_y(),
			frame_index,
			rlx::RGBA(0, 0, 0, 64)
		);
	}
}

PAKLib::sprite_rect EntityDummy::get_entity_bounds() const
{
	size_t sprite_index = _get_sprite_index();
	size_t frame_index = _get_frame_index();

	if (_entity_sprites[sprite_index])
	{
		return _entity_sprites[sprite_index]->get_frame_rectangle(frame_index);
	}

	// Fallback bounds if sprite not loaded
	return PAKLib::sprite_rect{ 0, 0, 32, 32 };
}

void EntityDummy::set_animation(AnimationType new_type, WeaponUsed new_weapon)
{
	if (current_animation_type == new_type) return;

	current_animation_type = new_type;

	switch (new_type) {
	case AnimationType::STAND:
		current_animation = _stand_animation;
		break;
	case AnimationType::WALK:
	case AnimationType::RUN:  // Entities use walk animation for running
		current_animation = _walk_animation;
		break;
	case AnimationType::ATTACK:
		current_animation = _attack_animation;
		break;
	case AnimationType::TAKEDAMAGE:
		current_animation = _takedamage_animation;
		break;
	case AnimationType::DYING:
		current_animation = _dying_animation;
		break;
	default:
		current_animation = _stand_animation;
		break;
	}

	current_animation.reset();
}

size_t EntityDummy::_get_sprite_index() const
{
	// Base index for this entity type
	size_t base = EntitySpriteBase(EntityID::DUMMY);

	// Get sprite offset based on animation type
	size_t sprite_offset = 0;
	switch (current_animation_type) {
	case AnimationType::STAND:
		sprite_offset = SPRITE_OFFSET_STAND;
		break;
	case AnimationType::WALK:
	case AnimationType::RUN:
		sprite_offset = SPRITE_OFFSET_WALK;
		break;
	case AnimationType::ATTACK:
		sprite_offset = SPRITE_OFFSET_ATTACK;
		break;
	case AnimationType::TAKEDAMAGE:
		sprite_offset = SPRITE_OFFSET_TAKEDAMAGE;
		break;
	case AnimationType::DYING:
		sprite_offset = SPRITE_OFFSET_DYING;
		break;
	default:
		sprite_offset = SPRITE_OFFSET_STAND;
		break;
	}

	// Sprite index = base + animation_offset + direction
	return base + sprite_offset + current_direction;
}

size_t EntityDummy::_get_frame_index() const
{
	// Each sprite has 4 frames (0-3)
	return current_animation.current_frame;
}

int EntityDummy::on_before_attack(Entity* target)
{
	// Dummy deals 5 damage
	return 5;
}

void EntityDummy::on_after_attack(Entity* target)
{
	// Could trigger effects, sounds, etc. after attacking
}

bool EntityDummy::on_receive_attack(Entity* attacker, int damage)
{
	// Dummy receives attack - could reduce HP, play hit animation, etc.
	// For now, just log it (no HP system yet)
	return true;
}

void EntityDummy::on_tile_reached()
{
	// Pause for half a second before moving to the next tile
	set_tile_pause(0.5f);
}

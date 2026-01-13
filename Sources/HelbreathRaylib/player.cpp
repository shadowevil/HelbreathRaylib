// player.cpp
#include "raylib_include.h"
#include "entities.h"
#include "ItemMetadata.h"
#include "Game.h"
#include "SoundIDs.h"
#include <optional>

Player::Player(const PlayerAppearance& appear, const PlayerEntityStats& stats)
	: appearance(appear), _player_stats(stats)
{
    current_animation = _stand_animation;
}

void Player::on_before_movement_update()
{
    // Capture dash ghost trail BEFORE movement updates position
    _update_dash_ghosts();
}

void Player::on_update()
{
    current_animation.update(raylib::GetTime());

    // Update dash ghost fade-out
    _fade_dash_ghosts();

    // Handle combat (attacking, pursuing targets)
    // Returns true if input should be blocked (e.g., during attack animation)
    if (_handle_combat())
        return;

    // Play footstep sounds during walk/run animations
    if (is_moving())
    {
        if (current_animation_type == WALK)
        {
            // Walking: play at frames 1 and 5
            if (current_animation.just_changed_to(1) || current_animation.just_changed_to(5))
            {
                _game.sound_player->play_character_sound(Sound::CharacterSoundID::PLAYER_WALK);
            }
        }
        else if (current_animation_type == RUN)
        {
            // Running: play at frames 1 and 5 (same as walking, but faster cycle)
            if (current_animation.just_changed_to(1) || current_animation.just_changed_to(5))
            {
                _game.sound_player->play_character_sound(Sound::CharacterSoundID::PLAYER_RUN);
            }
        }
    }

    if (_attached_camera)
    {
        // Click movement handling (only if not targeting an entity)
        if (raylib::IsMouseButtonDown(raylib::MOUSE_BUTTON_LEFT) && !_attack_target)
        {
            GamePosition Target = get_tile_world_mouse_position(*_attached_camera);

            // Skip movement if clicking on current tile (prevents accidental movement)
            if (Target.get_tile_x() != position.get_tile_x() || Target.get_tile_y() != position.get_tile_y())
            {
                bool Run = raylib::IsKeyDown(raylib::KEY_LEFT_SHIFT) || raylib::IsKeyDown(raylib::KEY_RIGHT_SHIFT);
                move_to(Target, Run);
            }
        }

		// Toggle combat/peace stance
        if (raylib::IsKeyPressed(raylib::KEY_TAB))
        {
            _current_stance = (_current_stance == PEACE ? COMBAT : PEACE);
            set_animation(current_animation_type, _current_weapon_type);
        }

		// Running / Walking handling
        if(raylib::IsKeyDown(raylib::KEY_LEFT_SHIFT))
        {
            if (current_animation_type != RUN && is_moving())
            {
				set_animation(RUN, _current_weapon_type);
                _is_running = true;
            }
        }
        else
        {   // Force walking if moving and not holding shift
            if(current_animation_type != WALK && is_moving())
            {
                set_animation(WALK, _current_weapon_type);
                _is_running = false;
            }
		}

		// Stop movement if right mouse button is down.
        // If we're not moving, change direction to face mouse.
        // Also clears attack target
        if (raylib::IsMouseButtonDown(raylib::MOUSE_BUTTON_RIGHT))
        {
            clear_attack_target();
            if(!is_moving()) {
                current_direction = get_direction_to_point(position, get_tile_world_mouse_position(*_attached_camera));
            }
            else {
                _request_stop_movement();
            }
        }
    }
}

void Player::on_render() const
{
    // Render dash ghost trail (behind player)
    _render_dash_ghosts();

    // Calculate display frame - applies offset for dash attack finish phase
    // Attack animations always have 8 frames per direction in the sprite sheet,
    // even if we're only playing a subset (like dash-attack's 4-frame phases)
    size_t frame = current_animation.current_frame + _animation_frame_offset;
    size_t dir_mult = (current_animation_type == ATTACK) ? 8 : current_animation.max_frame;
    size_t display_frame = frame + (current_direction * dir_mult);

    // Model rendering
    _model_sprites[get_player_animation_index(_current_stance, current_animation_type, _current_weapon_type, appearance.gender)]
        ->draw(position.get_pixel_x(), position.get_pixel_y(), display_frame, SkinColorFromIndex(appearance.skin_color_index));

    // Hair rendering
	if (appearance.hair_style != HAIR_STYLE_0)
        _model_sprites[get_player_hair_animation_index(_current_stance, current_animation_type, _current_weapon_type, appearance.gender, appearance.hair_style)]
            ->draw(position.get_pixel_x(), position.get_pixel_y(), display_frame, HairColorFromIndex(appearance.hair_color_index));

    // Underwear rendering
    _model_sprites[get_player_underwear_animation_index(_current_stance, current_animation_type, _current_weapon_type, appearance.gender)]
        ->draw(position.get_pixel_x(), position.get_pixel_y(), display_frame, UnderwearColorFromIndex(appearance.underwear_color_index));

    appearance.equipment.foreach([&](auto& slot, int idx) {
        _draw_model_item(slot.get_id(), raylib::WHITE);
        });
}

void Player::on_render_shadow() const
{
    // Calculate display frame - applies offset for dash attack finish phase
    // Attack animations always have 8 frames per direction in the sprite sheet
    size_t frame = current_animation.current_frame + _animation_frame_offset;
    size_t dir_mult = (current_animation_type == ATTACK) ? 8 : current_animation.max_frame;
    size_t display_frame = frame + (current_direction * dir_mult);

    // Model rendering
    _model_sprites[get_player_animation_index(_current_stance, current_animation_type, _current_weapon_type, appearance.gender)]
        ->draw(position.get_pixel_x(), position.get_pixel_y(), display_frame, rlx::RGBA(0, 0, 0, 64));

    // Hair rendering
    if (appearance.hair_style != HAIR_STYLE_0)
        _model_sprites[get_player_hair_animation_index(_current_stance, current_animation_type, _current_weapon_type, appearance.gender, appearance.hair_style)]
            ->draw(position.get_pixel_x(), position.get_pixel_y(), display_frame, rlx::RGBA(0, 0, 0, 32));

    // Underwear rendering
    _model_sprites[get_player_underwear_animation_index(_current_stance, current_animation_type, _current_weapon_type, appearance.gender)]
        ->draw(position.get_pixel_x(), position.get_pixel_y(), display_frame, rlx::RGBA(0, 0, 0, 32));

    appearance.equipment.foreach([&](auto& slot, int idx) {
        _draw_model_item(slot.get_id(), rlx::RGBA(0, 0, 0, 32), true);
        });
}

PAKLib::sprite_rect Player::get_entity_bounds() const
{
    // Calculate display frame - applies offset for dash attack finish phase
    // Attack animations always have 8 frames per direction in the sprite sheet
    size_t frame = current_animation.current_frame + _animation_frame_offset;
    size_t dir_mult = (current_animation_type == ATTACK) ? 8 : current_animation.max_frame;
    size_t display_frame = frame + (current_direction * dir_mult);

    return _model_sprites[get_player_animation_index(_current_stance, current_animation_type, _current_weapon_type, appearance.gender)]
        ->get_frame_rectangle(display_frame);
}

void Player::set_animation(AnimationType new_type, WeaponUsed new_weapon)
{
    current_animation_type = new_type;
    _current_weapon_type = new_weapon;

    switch (new_type)
    {
    case STAND:      current_animation = _stand_animation;      break;
    case WALK:       current_animation = _walk_animation;       break;
    case RUN:        current_animation = _run_animation;        break;
    case ATTACK:
        // During dash, use stage 1 animation (frames 0-3 at 2x speed)
        if (is_dashing())
        {
            current_animation = _dash_attack_stage1;
        }
        else if (new_weapon == CAST)
        {
            current_animation = _cast_animation_def;
        }
        else
        {
            current_animation = _attack_animation_def;
        }
        break;
    case PICKUP:     current_animation = _pickup_animation;     break;
    case TAKEDAMAGE: current_animation = _takedamage_animation; break;
    case DYING:      current_animation = _dying_animation;      break;
    }

    current_animation.reset();
}

void Player::_draw_model_item(int16_t item_id, raylib::Color item_color, bool is_shadow) const
{
    if (item_id == -1)
        return;

    auto Sprite = _model_sprites[get_player_item_animation_index(item_id, _current_stance, current_animation_type, _current_weapon_type, appearance.gender)];
    if (!Sprite)    // sprite not found, return safely
        return;

    // Calculate display frame - applies offset for dash attack finish phase
    // Attack animations always have 8 frames per direction in the sprite sheet
    size_t frame = current_animation.current_frame + _animation_frame_offset;
    size_t dir_mult = (current_animation_type == ATTACK) ? 8 : current_animation.max_frame;
    size_t display_frame = frame + (current_direction * dir_mult);

    Sprite->draw(position.get_pixel_x(), position.get_pixel_y(), display_frame, item_color);
}

bool Player::_handle_combat()
{
    // Handle dash-attack cooldown (allows movement, blocks new attacks)
    if (_dash_attack_cooldown > 0.0f)
    {
        _dash_attack_cooldown -= raylib::GetFrameTime();
        if (_dash_attack_cooldown <= 0.0f)
        {
            _dash_attack_cooldown = 0.0f;
        }
        // Don't block input - allow movement during cooldown
        // New attacks will be blocked by checking cooldown below
    }

    // Handle ongoing pickup animation
    if (_is_picking_up)
    {
        if (current_animation.is_finished())
        {
            _is_picking_up = false;
            set_animation(STAND, _current_weapon_type);
            // TODO: Call map's item pickup logic here
            // _active_map->try_pickup_item_at(position);
        }
        // Block all other input during pickup
        return true;
    }

    // Handle ongoing dash (fast movement before attack)
    if (is_dashing())
    {
        // Dash movement is in progress - block input and let movement system handle it
        // Attack will start automatically when dash completes via on_dash_complete()
        return true;
    }

    // Handle ongoing attack animation
    if (_is_attacking)
    {
        // Allow stance toggle during attack
        if (raylib::IsKeyPressed(raylib::KEY_TAB))
        {
            _current_stance = (_current_stance == PEACE ? COMBAT : PEACE);
        }

        // Play hit or miss sound at sprite frame 5 (middle/end of 8-frame animation) - only in combat stance
        // For dash-attack finish phase: frame offset is 4, so animation frame 1 = sprite frame 5
        // For normal attack: no offset, so animation frame 5 = sprite frame 5
        // TODO: Sound selection should be based on equipped item's class/subclass, not WeaponUsed
        size_t sound_trigger_frame = (_animation_frame_offset > 0) ? 1 : 5;
        if (_attack_target && _current_stance == COMBAT && current_animation.just_changed_to(sound_trigger_frame))
        {
            if (_attack_will_hit)
            {
                // Hit sounds - for now, HAND uses blunt hit sound
                // Future: use item class/subclass for SHARP_WEAPON_HIT etc.
                switch (_current_weapon_type)
                {
                case HAND:
                    _game.sound_player->play_character_sound(Sound::CharacterSoundID::BLUNT_WEAPON_HIT, 2.0f);
                    break;
                case BOW:
                case CAST:
                    // Bow and cast don't have separate hit sounds
                    break;
                }
            }
            else
            {
                // Miss sounds - for now, HAND uses normal miss
                // Future: use item class/subclass for SWORD_SWING_MISS, HEAVY_SWORD_SWING_MISS etc.
                switch (_current_weapon_type)
                {
                case HAND:
                    _game.sound_player->play_character_sound(Sound::CharacterSoundID::NORMAL_MISS, 2.0f);
                    break;
                case BOW:
                case CAST:
                    // Bow and cast don't have separate miss sounds
                    break;
                }
            }
        }

        // Handle dash-attack hold time (holding frame 5)
        if (_dash_attack_hold_time > 0.0f)
        {
            _dash_attack_hold_time -= raylib::GetFrameTime();
            if (_dash_attack_hold_time <= 0.0f)
            {
                // Hold finished - end attack state
                _dash_attack_hold_time = 0.0f;

                _is_attacking = false;
                _is_dash_attack = false;
                _animation_frame_offset = 0;
                _dash_attack_cooldown = 0.15f;
                set_animation(STAND, _current_weapon_type);

                if (!raylib::IsMouseButtonDown(raylib::MOUSE_BUTTON_LEFT))
                {
                    clear_attack_target();
                }
                return false;  // Allow movement during cooldown
            }
            // Block input while holding
            return true;
        }

        // Attack animation completed
        if (current_animation.is_finished())
        {
            // Handle dash-attack stage 2 completion - deliver attack and start hold on frame 5
            if (_is_dash_attack && _animation_frame_offset == 5)
            {
                // Play hit or miss sound
                if (_attack_target && _current_stance == COMBAT)
                {
                    if (_attack_will_hit)
                    {
                        switch (_current_weapon_type)
                        {
                        case HAND:
                            _game.sound_player->play_character_sound(Sound::CharacterSoundID::BLUNT_WEAPON_HIT, 2.0f);
                            break;
                        case BOW:
                        case CAST:
                            break;
                        }
                    }
                    else
                    {
                        switch (_current_weapon_type)
                        {
                        case HAND:
                            _game.sound_player->play_character_sound(Sound::CharacterSoundID::NORMAL_MISS, 2.0f);
                            break;
                        case BOW:
                        case CAST:
                            break;
                        }
                    }

                    // Deliver attack
                    attack(_attack_target);
                }

                _dash_attack_hold_time = 0.15f;
                return true;  // Block input during hold
            }

            // Normal attack finished - deliver the attack to target (only in combat stance)
            if (_attack_target && _current_stance == COMBAT)
            {
                attack(_attack_target);
            }

            _is_attacking = false;
            _is_dash_attack = false;
            _animation_frame_offset = 0;
            set_animation(STAND, _current_weapon_type);

            // Non-sticky targeting: only continue attacking if mouse is still held
            if (!raylib::IsMouseButtonDown(raylib::MOUSE_BUTTON_LEFT))
            {
                clear_attack_target();
            }
        }

        // Block all other input during attack
        return true;
    }

    // Handle pursuing attack target
    if (_attack_target)
    {
        // Block new attacks during dash-attack cooldown (but allow movement)
        if (_dash_attack_cooldown > 0.0f)
        {
            return false;  // Allow normal input processing (movement)
        }

        int distance = _get_tile_distance_to(_attack_target);
        int critical_range = _get_critical_range();

        // Check for critical attack (CTRL + at critical range + has charges)
        if (_can_critical_attack(_attack_target))
        {
            // Stop if moving
            if (is_moving())
            {
                _request_stop_movement();
                return true;
            }

            // Consume critical charge (DEBUG: disabled for unlimited criticals)
            // _player_stats.use_critical();

            // Face the target
            current_direction = get_direction_to_point(position, _attack_target->get_position());

            // Critical attacks always hit
            _attack_will_hit = true;
            _is_critical_attack = true;
            _is_dash_attack = false;

            // Play gender-based critical attack sound
            if (appearance.gender == GENDER_MALE)
                _game.sound_player->play_character_sound(Sound::CharacterSoundID::MALE_CRIT_ATTACK, 2.0f);
            else
                _game.sound_player->play_character_sound(Sound::CharacterSoundID::FEMALE_CRIT_ATTACK, 2.0f);

            // Start attack animation
            _is_attacking = true;
            set_animation(ATTACK, _current_weapon_type);
            return true;
        }

        // Check for dash-attack (CTRL + SHIFT, within critical range but not adjacent)
        // Dash must be in a direct line (straight or diagonal) - no L-shaped movements
        // Can trigger while stationary OR when entering dash range while pursuing
        bool ctrl_held = raylib::IsKeyDown(raylib::KEY_LEFT_CONTROL) || raylib::IsKeyDown(raylib::KEY_RIGHT_CONTROL);
        bool shift_held = raylib::IsKeyDown(raylib::KEY_LEFT_SHIFT) || raylib::IsKeyDown(raylib::KEY_RIGHT_SHIFT);

        int dir_dx = 0, dir_dy = 0;
        bool is_direct_line = _is_direct_line_to(_attack_target, dir_dx, dir_dy);

        if (ctrl_held && shift_held && distance > 1 && distance <= critical_range && is_direct_line)
        {
            // If moving, stop first - dash will trigger on next frame when stationary
            if (is_moving())
            {
                _request_stop_movement();
                return true;
            }

            // Calculate dash destination - one tile step toward target in direct line
            int my_tx = position.get_tile_x();
            int my_ty = position.get_tile_y();
            int dash_x = my_tx + dir_dx;
            int dash_y = my_ty + dir_dy;

            // Check if dash destination is valid
            bool can_dash = true;
            if (_active_map && !_active_map->get_tile(dash_x, dash_y)->is_move_allowed)
                can_dash = false;
            if (is_tile_occupied(dash_x, dash_y))
                can_dash = false;

            if (can_dash)
            {
                GamePosition dash_dest(dash_x, dash_y);

                // Pre-roll hit with dash bonus: +20% hit ratio
                float base_hit = PlayerEntityStats::hit_ratio_to_percentage(_player_stats.get_hit_ratio());
                float dash_bonus = base_hit + 20.0f;
                if (dash_bonus > 95.0f) dash_bonus = 95.0f;
                float roll = static_cast<float>(rand() % 100);
                _attack_will_hit = roll < dash_bonus;

                _is_dash_attack = true;
                _is_critical_attack = false;
                _dash_attack_pending = true;
                _is_attacking = true;

                // Store dash start/end positions for ghost trail interpolation
                _dash_start_px = static_cast<int32_t>(position.get_pixel_x());
                _dash_start_py = static_cast<int32_t>(position.get_pixel_y());
                _dash_end_px = static_cast<int32_t>(dash_dest.get_pixel_x());
                _dash_end_py = static_cast<int32_t>(dash_dest.get_pixel_y());
                _dash_ghost_count = 0;  // Clear old ghosts
                _dash_captured_frames = 0;  // Reset captured frames bitmask

                // Start fast dash movement with ATTACK animation (not RUN)
                dash_to(dash_dest);
                // Face the target AFTER dash_to (dash_to overwrites direction in _start_next_step)
                current_direction = get_direction_to_point(position, _attack_target->get_position());
                set_animation(ATTACK, _current_weapon_type);  // Override to use attack frames during dash
                return true;
            }
        }

        if (is_in_attack_range_of(_attack_target))
        {
            // In range - request stop if moving, wait until movement completes
            if (is_moving())
            {
                _request_stop_movement();
                return true;  // Wait for movement to complete before attacking
            }

            // Not moving - we're on a tile and ready to attack
            // Face the target
            current_direction = get_direction_to_point(position, _attack_target->get_position());

            // Pre-roll hit check for this attack (used for sound selection and damage)
            // Note: If this is a dash-attack, _attack_will_hit was already pre-rolled with bonus
            if (!_is_dash_attack)
            {
                _attack_will_hit = _player_stats.roll_hit();
            }
            _is_critical_attack = false;

            // Play weapon swing/attack sound (only in combat stance, and only with weapon equipped)
            // TODO: Melee swing sounds should be based on item class/subclass
            if (_current_stance == COMBAT && _current_weapon_type != HAND)
            {
                switch (_current_weapon_type)
                {
                case BOW:
                    _game.sound_player->play_character_sound(Sound::CharacterSoundID::BOW_ATTACK, 2.0f);
                    break;
                case CAST:
                    _game.sound_player->play_character_sound(Sound::CharacterSoundID::CASTING_SOUND, 2.0f);
                    break;
                default:
                    break;
                }
            }

            // Start attack animation (stance affects how it looks)
            _is_attacking = true;
            set_animation(ATTACK, _current_weapon_type);
        }
        else if (!is_moving())
        {
            // Not in range and not already moving - find adjacent tile to attack from
            const GamePosition& target_pos = _attack_target->get_position();
            int target_tx = target_pos.get_tile_x();
            int target_ty = target_pos.get_tile_y();
            int my_tx = position.get_tile_x();
            int my_ty = position.get_tile_y();

            // Direction from target to player (approach direction)
            int approach_dx = my_tx - target_tx;
            int approach_dy = my_ty - target_ty;

            // Find the best adjacent tile to move to (closest to our current position)
            GamePosition best_adjacent = target_pos;
            int best_dist = INT_MAX;
            int best_alignment = INT_MIN;  // Higher = more aligned with approach direction

            // Check all 8 adjacent tiles around the target
            const int offsets[8][2] = {
                {0, -1}, {0, 1}, {-1, 0}, {1, 0},  // Cardinal
                {-1, -1}, {1, -1}, {-1, 1}, {1, 1}  // Diagonal
            };

            for (const auto& offset : offsets)
            {
                int adj_x = target_tx + offset[0];
                int adj_y = target_ty + offset[1];

                // Skip if this tile is occupied or not walkable
                if (is_tile_occupied(adj_x, adj_y))
                    continue;

                if (_active_map && !_active_map->get_tile(adj_x, adj_y)->is_move_allowed)
                    continue;

                // Calculate distance from our position to this adjacent tile
                int dx = adj_x - my_tx;
                int dy = adj_y - my_ty;
                int dist = dx * dx + dy * dy;

                // Calculate alignment with approach direction (dot product of offset and approach)
                // Higher value = tile is more in the direction player is approaching from
                int alignment = offset[0] * approach_dx + offset[1] * approach_dy;

                // Prefer closer tiles, use alignment as tiebreaker
                if (dist < best_dist || (dist == best_dist && alignment > best_alignment))
                {
                    best_dist = dist;
                    best_alignment = alignment;
                    best_adjacent = GamePosition(adj_x, adj_y);
                }
            }

            // Move to the best adjacent tile
            if (best_dist < INT_MAX)
            {
                bool Run = raylib::IsKeyDown(raylib::KEY_LEFT_SHIFT) || raylib::IsKeyDown(raylib::KEY_RIGHT_SHIFT);
                move_to(best_adjacent, Run);
            }
        }
        // If already moving toward target, let movement continue naturally
    }

    return false;
}

void Player::set_attack_target(Entity* target)
{
    _attack_target = target;
}

void Player::clear_attack_target()
{
    _attack_target = nullptr;
}

int Player::on_before_attack(Entity* target)
{
    // Use pre-rolled hit check (rolled when attack started)
    if (!_attack_will_hit)
    {
        _is_critical_attack = false;  // Reset flag
        return AttackResult::MISS;
    }

    // Get base and max damage from stats
    int base_damage = static_cast<int>(_player_stats.get_stat(StatType::BASE_DAMAGE));
    int max_damage = static_cast<int>(_player_stats.get_max_stat(StatType::BASE_DAMAGE));

    // Add additional damage if any
    int additional = static_cast<int>(_player_stats.get_stat(StatType::ADDITIONAL_DAMAGE));
    base_damage += additional;
    max_damage += additional;

    // Ensure valid range
    if (max_damage < base_damage) max_damage = base_damage;
    if (base_damage < 1) base_damage = 1;
    if (max_damage < 1) max_damage = 1;

    int damage;

    // Critical attack: roll 3 times, pick highest
    if (_is_critical_attack)
    {
        damage = PlayerEntityStats::roll_critical_damage(base_damage, max_damage);
        _is_critical_attack = false;  // Reset flag
    }
    else
    {
        // Normal damage roll
        if (max_damage > base_damage)
        {
            damage = base_damage + (rand() % (max_damage - base_damage + 1));
        }
        else
        {
            damage = base_damage;
        }
    }

    return damage;
}

void Player::on_after_attack(Entity* target)
{
    // TODO: Trigger effects, gain experience, etc.
}

bool Player::on_receive_attack(Entity* attacker, int damage)
{
    // TODO: Reduce HP, play hurt animation, etc.
    return true;
}

bool Player::try_pickup()
{
    // Can't pickup if already doing something
    if (_is_attacking || _is_picking_up || is_moving())
        return false;

    // Start pickup animation
    _is_picking_up = true;
    set_animation(PICKUP, _current_weapon_type);
    return true;
}

int Player::_get_tile_distance_to(const Entity* target) const
{
    if (!target) return INT_MAX;

    int dx = std::abs(static_cast<int>(target->get_position().get_tile_x()) - static_cast<int>(position.get_tile_x()));
    int dy = std::abs(static_cast<int>(target->get_position().get_tile_y()) - static_cast<int>(position.get_tile_y()));

    // Chebyshev distance (allows diagonal movement)
    return std::max(dx, dy);
}

int Player::_get_critical_range() const
{
    // Check equipped weapon for LARGE_* subclass
    int16_t weapon_id = appearance.equipment.weapon.get_id();
    if (weapon_id == -1) {
        return 2;  // Unarmed - default range
    }

    // TODO: When ItemMetadataEntry has subclass field, check for LARGE_* weapons
    // and return 3 for LARGE_SWORD, LARGE_AXE, LARGE_MACE
    // For now, default to 2 tiles for all weapons

    return 2;  // Default critical range for normal weapons
}

bool Player::_can_critical_attack(const Entity* target) const
{
    if (!target) return false;

    // Must have critical charges (DEBUG: unlimited criticals for testing)
    // if (!_player_stats.has_critical()) return false;

    // Must be holding CTRL (without SHIFT - SHIFT triggers dash instead)
    bool ctrl_held = raylib::IsKeyDown(raylib::KEY_LEFT_CONTROL) || raylib::IsKeyDown(raylib::KEY_RIGHT_CONTROL);
    bool shift_held = raylib::IsKeyDown(raylib::KEY_LEFT_SHIFT) || raylib::IsKeyDown(raylib::KEY_RIGHT_SHIFT);

    if (!ctrl_held || shift_held) return false;  // CTRL only, not CTRL+SHIFT

    // Check distance - must be within critical range but not adjacent (melee range)
    int distance = _get_tile_distance_to(target);
    int critical_range = _get_critical_range();

    // Critical attack is valid at exactly critical_range tiles (not closer)
    return distance == critical_range;
}

bool Player::_is_direct_line_to(const Entity* target, int& out_dx, int& out_dy) const
{
    if (!target) return false;

    int dx = static_cast<int>(target->get_position().get_tile_x()) - static_cast<int>(position.get_tile_x());
    int dy = static_cast<int>(target->get_position().get_tile_y()) - static_cast<int>(position.get_tile_y());

    int abs_dx = std::abs(dx);
    int abs_dy = std::abs(dy);

    // Direct line = either cardinal (one axis is 0) or diagonal (both axes equal)
    // Cardinal: dx=0 or dy=0
    // Diagonal: |dx| == |dy|
    bool is_direct = (abs_dx == 0 || abs_dy == 0 || abs_dx == abs_dy);

    if (is_direct)
    {
        // Normalize to unit step direction
        out_dx = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
        out_dy = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;
    }

    return is_direct;
}

void Player::on_dash_complete()
{
    // Don't clear ghosts here - let them persist and fade out naturally

    // Dash finished - start the attack finish phase if we have a pending dash-attack
    if (_dash_attack_pending && _attack_target)
    {
        _dash_attack_pending = false;

        // Face the target
        current_direction = get_direction_to_point(position, _attack_target->get_position());

        // Start stage 2 animation (frames 5-7, ending on frame 7)
        // The offset makes animation frames 0-2 render as attack sprite frames 5-7
        _is_attacking = true;
        _animation_frame_offset = 5;  // Offset so frame 0 displays as frame 5
        current_animation_type = ATTACK;
        current_animation = _dash_attack_stage2;
        current_animation.reset();
    }
    else
    {
        // No pending attack - just stand
        _dash_attack_pending = false;
        _animation_frame_offset = 0;
        set_animation(STAND, _current_weapon_type);
    }
}

void Player::_update_dash_ghosts()
{
    // Use dash attack flags instead of is_dashing() for more reliable detection
    if (!_is_dash_attack || !_dash_attack_pending)
        return;

    // Don't add more ghosts if we've reached the limit
    if (_dash_ghost_count >= MAX_DASH_GHOSTS)
        return;

    // Get the current animation frame
    size_t anim_frame = current_animation.current_frame;

    // Only capture at animation frames 1, 3, and 4 (meaningful poses in the attack animation)
    // Frame 0 is the starting pose, frames 1-4 are the swing, frame 5+ is follow-through
    bool is_target_frame = (anim_frame % 2 != 0);
    if (!is_target_frame)
        return;

    // Check if we've already captured this animation frame (using bitmask)
    uint32_t frame_bit = 1u << anim_frame;
    if (_dash_captured_frames & frame_bit)
        return;  // Already captured this frame

    // Mark this frame as captured
    _dash_captured_frames |= frame_bit;

    // Use current pixel position (this is called BEFORE movement update, so position is valid)
    int32_t ghost_x = static_cast<int32_t>(position.get_pixel_x());
    int32_t ghost_y = static_cast<int32_t>(position.get_pixel_y());

    // Compute display frame at capture time (fully detached from player state)
    size_t frame = anim_frame + _animation_frame_offset;
    size_t display_frame = frame + (current_direction * 8);

    // Add ghost at the END of the array - capture complete snapshot
    _dash_ghosts[_dash_ghost_count] = DashGhost{
        ghost_x,
        ghost_y,
        display_frame,
        current_direction,
        GHOST_START_OPACITY,
        GHOST_FADE_DURATION,
        _current_stance,
        current_animation_type,
        _current_weapon_type
    };
    _dash_ghost_count++;
}

void Player::_fade_dash_ghosts()
{
    if (_dash_ghost_count == 0)
        return;

    float dt = raylib::GetFrameTime();

    for (size_t i = 0; i < _dash_ghost_count; ++i)
    {
        auto& ghost = _dash_ghosts[i];
        if (ghost.fade_timer > 0.0f)
        {
            ghost.fade_timer -= dt;
            if (ghost.fade_timer < 0.0f)
                ghost.fade_timer = 0.0f;

            // Calculate opacity based on remaining fade time
            ghost.opacity = GHOST_START_OPACITY * (ghost.fade_timer / GHOST_FADE_DURATION);
        }
    }

    // Remove fully faded ghosts by compacting the array
    size_t write_idx = 0;
    for (size_t read_idx = 0; read_idx < _dash_ghost_count; ++read_idx)
    {
        if (_dash_ghosts[read_idx].opacity > 0.01f)
        {
            if (write_idx != read_idx)
                _dash_ghosts[write_idx] = _dash_ghosts[read_idx];
            ++write_idx;
        }
    }
    _dash_ghost_count = write_idx;
}

void Player::_render_dash_ghosts() const
{
    if (_dash_ghost_count == 0)
        return;

    // Render ghosts - each uses its captured animation state (fully static)
    for (size_t i = 0; i < _dash_ghost_count; ++i)
    {
        const auto& ghost = _dash_ghosts[i];
        if (ghost.opacity < 0.01f)
            continue;

        // Convert float opacity (0-1) to uint8_t (0-255)
        uint8_t alpha = static_cast<uint8_t>(ghost.opacity * 255.0f);

        // Get base colors and apply ghost opacity
        auto skin_color = SkinColorFromIndex(appearance.skin_color_index);
        auto hair_color = HairColorFromIndex(appearance.hair_color_index);
        auto underwear_color = UnderwearColorFromIndex(appearance.underwear_color_index);

        rlx::RGBA ghost_skin(skin_color.GetR(), skin_color.GetG(), skin_color.GetB(), alpha);
        rlx::RGBA ghost_hair(hair_color.GetR(), hair_color.GetG(), hair_color.GetB(), alpha);
        rlx::RGBA ghost_underwear(underwear_color.GetR(), underwear_color.GetG(), underwear_color.GetB(), alpha);
        rlx::RGBA ghost_white(255, 255, 255, alpha);

        // Use ghost's captured animation state (stance, anim_type, weapon) - NOT current player state
        _model_sprites[get_player_animation_index(ghost.stance, ghost.anim_type, ghost.weapon, appearance.gender)]
            ->draw(ghost.pixel_x, ghost.pixel_y, ghost.display_frame, ghost_skin);

        if (appearance.hair_style != HAIR_STYLE_0)
            _model_sprites[get_player_hair_animation_index(ghost.stance, ghost.anim_type, ghost.weapon, appearance.gender, appearance.hair_style)]
                ->draw(ghost.pixel_x, ghost.pixel_y, ghost.display_frame, ghost_hair);

        _model_sprites[get_player_underwear_animation_index(ghost.stance, ghost.anim_type, ghost.weapon, appearance.gender)]
            ->draw(ghost.pixel_x, ghost.pixel_y, ghost.display_frame, ghost_underwear);

        // Equipment - also uses captured animation state
        appearance.equipment.foreach([&](auto& slot, int idx) {
            if (slot.get_id() == -1)
                return;
            auto Sprite = _model_sprites[get_player_item_animation_index(slot.get_id(), ghost.stance, ghost.anim_type, ghost.weapon, appearance.gender)];
            if (Sprite)
                Sprite->draw(ghost.pixel_x, ghost.pixel_y, ghost.display_frame, ghost_white);
        });
    }
}

void Player::_clear_dash_ghosts()
{
    _dash_ghost_count = 0;
    _dash_captured_frames = 0;
}
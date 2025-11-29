// player.cpp
#include "raylib_include.h"
#include "entities.h"
#include "ItemMetadata.h"
#include <optional>

Player::Player(const PlayerAppearance& appear)
	: appearance(appear)
{
    current_animation = _stand_animation;
}

void Player::on_update()
{
    current_animation.update(GetTime());

    if (_attached_camera)
    {
        // Click movement handling
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            GamePosition Target = get_tile_world_mouse_position(*_attached_camera);
            bool Run = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
            move_to(Target, Run);
        }

		// Toggle combat/peace stance
        if (IsKeyPressed(KEY_TAB))
        {
            _current_stance = (_current_stance == PEACE ? COMBAT : PEACE);
            set_animation(current_animation_type, _current_weapon_type);
        }

		// Running / Walking handling
        if(IsKeyDown(KEY_LEFT_SHIFT))
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
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            if(!is_moving()) {
                current_direction = get_direction_to_point(position, get_tile_world_mouse_position(*_attached_camera));
            }
            else {
                _request_stop_movement();
            }
        }

#ifdef _DEBUG
		// Debug animations with ALT + 1-6
        if (IsKeyDown(KEY_LEFT_ALT))
        {
            if (IsKeyPressed(KEY_ONE))
                set_animation(ATTACK, HAND);
            else if (IsKeyPressed(KEY_TWO) && _current_stance == COMBAT)
                set_animation(ATTACK, BOW);
            else if (IsKeyPressed(KEY_THREE) && _current_stance == COMBAT)
                set_animation(ATTACK, CAST);
            else if (IsKeyPressed(KEY_FOUR))
                set_animation(PICKUP, HAND);
            else if (IsKeyPressed(KEY_FIVE))
                set_animation(TAKEDAMAGE, HAND);
            else if (IsKeyPressed(KEY_SIX))
                set_animation(DYING, HAND);

            if (IsKeyPressed(KEY_G))
            {
                if (appearance.gender == GENDER_MALE)
                    appearance.gender = GENDER_FEMALE;
                else if (appearance.gender == GENDER_FEMALE)
                    appearance.gender = GENDER_MALE;
            }
        }
#endif
    }
}

void Player::on_render() const
{
    // Model rendering
    _model_sprites[get_player_animation_index(_current_stance, current_animation_type, _current_weapon_type, appearance.gender)]
        ->draw(position.get_pixel_x(), position.get_pixel_y(), current_animation.current_frame + (current_direction * current_animation.max_frame), SkinColorFromIndex(appearance.skin_color_index));

    // Hair rendering
	if (appearance.hair_style != HAIR_STYLE_0)
        _model_sprites[get_player_hair_animation_index(_current_stance, current_animation_type, _current_weapon_type, appearance.gender, appearance.hair_style)]
            ->draw(position.get_pixel_x(), position.get_pixel_y(), current_animation.current_frame + (current_direction * current_animation.max_frame), HairColorFromIndex(appearance.hair_color_index));

    // Underwear rendering
    _model_sprites[get_player_underwear_animation_index(_current_stance, current_animation_type, _current_weapon_type, appearance.gender)]
        ->draw(position.get_pixel_x(), position.get_pixel_y(), current_animation.current_frame + (current_direction * current_animation.max_frame), UnderwearColorFromIndex(appearance.underwear_color_index));

    appearance.equipment.foreach([&](auto& slot, int idx) {
        _draw_model_item(slot.get_id(), WHITE);
        });
}

void Player::on_render_shadow() const
{
    // Model rendering
    _model_sprites[get_player_animation_index(_current_stance, current_animation_type, _current_weapon_type, appearance.gender)]
        ->draw(position.get_pixel_x(), position.get_pixel_y(), current_animation.current_frame + (current_direction * current_animation.max_frame), rlx::RGBA(0, 0, 0, 64));

    // Hair rendering
    if (appearance.hair_style != HAIR_STYLE_0)
        _model_sprites[get_player_hair_animation_index(_current_stance, current_animation_type, _current_weapon_type, appearance.gender, appearance.hair_style)]
            ->draw(position.get_pixel_x(), position.get_pixel_y(), current_animation.current_frame + (current_direction * current_animation.max_frame), rlx::RGBA(0, 0, 0, 32));

    // Underwear rendering
    _model_sprites[get_player_underwear_animation_index(_current_stance, current_animation_type, _current_weapon_type, appearance.gender)]
        ->draw(position.get_pixel_x(), position.get_pixel_y(), current_animation.current_frame + (current_direction * current_animation.max_frame), rlx::RGBA(0, 0, 0, 32));

    appearance.equipment.foreach([&](auto& slot, int idx) {
        _draw_model_item(slot.get_id(), rlx::RGBA(0, 0, 0, 32), true);
        });
}

PAKLib::sprite_rect Player::get_entity_bounds() const
{
    return _model_sprites[get_player_animation_index(_current_stance, current_animation_type, _current_weapon_type, appearance.gender)]
        ->get_frame_rectangle(current_animation.current_frame + (current_direction * current_animation.max_frame));
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
        current_animation = (new_weapon == CAST)
            ? _cast_animation
            : _attack_animation;
        break;
    case PICKUP:     current_animation = _pickup_animation;     break;
    case TAKEDAMAGE: current_animation = _takedamage_animation; break;
    case DYING:      current_animation = _dying_animation;      break;
    }

    current_animation.reset();
}

void Player::_draw_model_item(int16_t item_id, Color item_color, bool is_shadow) const
{
    if (item_id == -1)
        return;

    auto Sprite = _model_sprites[get_player_item_animation_index(item_id, _current_stance, current_animation_type, _current_weapon_type, appearance.gender)];
    if (!Sprite)    // sprite not found, return safely
        return;

    if (is_shadow)
    {
        Sprite->draw(position.get_pixel_x(), position.get_pixel_y(), current_animation.current_frame + (current_direction * current_animation.max_frame), item_color);
    }
    else {
        Sprite->draw(position.get_pixel_x(), position.get_pixel_y(), current_animation.current_frame + (current_direction * current_animation.max_frame), item_color);
    }
}
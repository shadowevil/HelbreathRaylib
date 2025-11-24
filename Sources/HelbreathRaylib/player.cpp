// player.cpp
#include "raylib_include.h"
#include "entities.h"
#include "ItemMetadata.h"
#include <optional>

Player::Player(const PlayerAppearance& appear)
	: appearance(appear)
{
    current_animation = stand_animation;
}

void Player::OnUpdate()
{
    current_animation.update(GetTime());

    if (m_attachedCamera)
    {
        // Click movement handling
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            GamePosition target = GetTileWorldMousePosition(*m_attachedCamera);
            bool run = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
            MoveTo(target, run);
        }

		// Toggle combat/peace stance
        if (IsKeyPressed(KEY_TAB))
        {
            current_stance = (current_stance == PEACE ? COMBAT : PEACE);
            SetAnimation(current_animation_type, current_weapon_type);
        }

		// Running / Walking handling
        if(IsKeyDown(KEY_LEFT_SHIFT))
        {
            if (current_animation_type != RUN && IsMoving())
            {
				SetAnimation(RUN, current_weapon_type);
                m_isRunning = true;
            }
        }
        else
        {   // Force walking if moving and not holding shift
            if(current_animation_type != WALK && IsMoving())
            {
                SetAnimation(WALK, current_weapon_type);
                m_isRunning = false;
            }
		}

		// Stop movement if right mouse button is down.
        // If we're not moving, change direction to face mouse.
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            if(!IsMoving()) {
                current_direction = GetDirectionToPoint(position, GetTileWorldMousePosition(*m_attachedCamera));
            }
            else {
                RequestStopMovement();
            }
        }

#ifdef _DEBUG
		// Debug animations with ALT + 1-6
        if (IsKeyDown(KEY_LEFT_ALT))
        {
            if (IsKeyPressed(KEY_ONE))
                SetAnimation(ATTACK, HAND);
            else if (IsKeyPressed(KEY_TWO) && current_stance == COMBAT)
                SetAnimation(ATTACK, BOW);
            else if (IsKeyPressed(KEY_THREE) && current_stance == COMBAT)
                SetAnimation(ATTACK, CAST);
            else if (IsKeyPressed(KEY_FOUR))
                SetAnimation(PICKUP, HAND);
            else if (IsKeyPressed(KEY_FIVE))
                SetAnimation(TAKEDAMAGE, HAND);
            else if (IsKeyPressed(KEY_SIX))
                SetAnimation(DYING, HAND);

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

void Player::OnRender() const
{
    // Model rendering
    m_modelSprites[GetPlayerAnimationIndex(current_stance, current_animation_type, current_weapon_type, appearance.gender)]
        ->Draw(position.get_pixel_x(), position.get_pixel_y(), current_animation.current_frame + (current_direction * current_animation.max_frame), appearance.skin_color);

    // Hair rendering
	if (appearance.hair_style != HAIR_STYLE_0)
        m_modelSprites[GetPlayerHairAnimationIndex(current_stance, current_animation_type, current_weapon_type, appearance.gender, appearance.hair_style)]
            ->Draw(position.get_pixel_x(), position.get_pixel_y(), current_animation.current_frame + (current_direction * current_animation.max_frame), appearance.hair_color);

    // Underwear rendering
    m_modelSprites[GetPlayerUnderwearAnimationIndex(current_stance, current_animation_type, current_weapon_type, appearance.gender)]
        ->Draw(position.get_pixel_x(), position.get_pixel_y(), current_animation.current_frame + (current_direction * current_animation.max_frame), appearance.underwear_color);

    appearance.equipment.foreach([&](auto& slot, int idx) {
        DrawModelItem(slot.Id(), WHITE);
        });
}

void Player::OnRenderShadow() const
{
    // Model rendering
    m_modelSprites[GetPlayerAnimationIndex(current_stance, current_animation_type, current_weapon_type, appearance.gender)]
        ->Draw(position.get_pixel_x(), position.get_pixel_y(), current_animation.current_frame + (current_direction * current_animation.max_frame), rlx::RGBA(0, 0, 0, 64));

    // Hair rendering
    if (appearance.hair_style != HAIR_STYLE_0)
        m_modelSprites[GetPlayerHairAnimationIndex(current_stance, current_animation_type, current_weapon_type, appearance.gender, appearance.hair_style)]
            ->Draw(position.get_pixel_x(), position.get_pixel_y(), current_animation.current_frame + (current_direction * current_animation.max_frame), rlx::RGBA(0, 0, 0, 32));

    // Underwear rendering
    m_modelSprites[GetPlayerUnderwearAnimationIndex(current_stance, current_animation_type, current_weapon_type, appearance.gender)]
        ->Draw(position.get_pixel_x(), position.get_pixel_y(), current_animation.current_frame + (current_direction * current_animation.max_frame), rlx::RGBA(0, 0, 0, 32));

    appearance.equipment.foreach([&](auto& slot, int idx) {
        DrawModelItem(slot.Id(), rlx::RGBA(0, 0, 0, 32), true);
        });
}

PAKLib::sprite_rect Player::GetEntityBounds() const
{
    return m_modelSprites[GetPlayerAnimationIndex(current_stance, current_animation_type, current_weapon_type, appearance.gender)]
        ->GetFrameRectangle(current_animation.current_frame + (current_direction * current_animation.max_frame));
}

void Player::SetAnimation(AnimationType newType, WeaponUsed newWeapon)
{
    current_animation_type = newType;
    current_weapon_type = newWeapon;

    switch (newType)
    {
    case STAND:      current_animation = stand_animation;      break;
    case WALK:       current_animation = walk_animation;       break;
    case RUN:        current_animation = run_animation;        break;
    case ATTACK:
        current_animation = (newWeapon == CAST)
            ? cast_animation
            : attack_animation;
        break;
    case PICKUP:     current_animation = pickup_animation;     break;
    case TAKEDAMAGE: current_animation = takedamage_animation; break;
    case DYING:      current_animation = dying_animation;      break;
    }

    current_animation.reset();
}

void Player::DrawModelItem(int16_t item_id, Color item_color, bool is_shadow) const
{
    if (item_id == -1)
        return;

    auto sprite = m_modelSprites[GetPlayerItemAnimationIndex(item_id, current_stance, current_animation_type, current_weapon_type, appearance.gender)];
    if (!sprite)    // sprite not found, return safely
        return;

    if (is_shadow)
    {
        sprite->Draw(position.get_pixel_x(), position.get_pixel_y(), current_animation.current_frame + (current_direction * current_animation.max_frame), item_color);
    }
    else {
        sprite->Draw(position.get_pixel_x(), position.get_pixel_y(), current_animation.current_frame + (current_direction * current_animation.max_frame), item_color);
    }
}
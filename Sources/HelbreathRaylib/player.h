#pragma once
#include "entity.h"
#include <array>
#include <typeinfo>
#include <functional>
#include "ItemMetadata.h"

enum HairStyle : int8_t {
    HAIR_STYLE_0 = -1,       // Bald
	HAIR_STYLE_1,
	HAIR_STYLE_2,
	HAIR_STYLE_3,
	HAIR_STYLE_4,
	HAIR_STYLE_5,
	HAIR_STYLE_6,
	HAIR_STYLE_7,
	HAIR_STYLE_COUNT
};

enum Gender : uint8_t {
    GENDER_MALE,
    GENDER_FEMALE
};

template<EquipType Type>
struct Equip {
public:
    // Item metadata would be replaced with actual item data,
    // since we aren't implementing items yet, we just have metadata
    // to get them to draw on the screen
    bool TryEquip(int16_t item_id, const std::vector<ItemMetadataEntry>& _metadata)
    {
        const auto it = std::find_if(_metadata.begin(), _metadata.end(),
			[item_id](const ItemMetadataEntry& entry) { return entry.id == static_cast<uint32_t>(item_id); });
        if (it == _metadata.end())
			return false;
		const ItemMetadataEntry& metadata = *it;
        if (metadata.equip_type != Type)
            return false;
        id = item_id;
        return true;
	}

	void Unequip() { id = -1; }

	const int16_t& Id() const { return id; }
    static constexpr EquipType GetType() { return Type; }
private:
    int16_t id = -1;
};

struct Equipment {
    Equip<EquipType::Head>                      head{};
    Equip<EquipType::Back>                      back{};
    Equip<EquipType::Shirt>                     shirt{};
    Equip<EquipType::Chest>                     chest{};
    Equip<EquipType::Legs>                      legs{};
    Equip<EquipType::Feet>                      feet{};
    Equip<EquipType::Weapon>                    weapon{};
    Equip<EquipType::Shield>                    shield{};
    std::array<Equip<EquipType::Accessory>, 3>  accessories{};

    template<typename Func>
    void foreach(Func&& func) const {
        func(head, -1);
        func(back, -1);
        func(shirt, -1);
        func(chest, -1);
        func(legs, -1);
        func(feet, -1);
        func(weapon, -1);
        func(shield, -1);
        for (int i = 0; i < 3; ++i) {
            func(accessories[i], i);
        }
    }

    template<typename Func>
    void foreach(Func&& func) {
        func(head, -1);
        func(back, -1);
        func(shirt, -1);
        func(chest, -1);
        func(legs, -1);
        func(feet, -1);
        func(weapon, -1);
        func(shield, -1);
        for (int i = 0; i < 3; ++i) {
            func(accessories[i], i);
        }
    }
};

struct PlayerAppearance {
    Gender gender = GENDER_MALE;
	SkinColorIndex skin_color_index = 0;
	HairStyle hair_style = HAIR_STYLE_1;
	HairColorIndex hair_color_index = 0;
	UnderwearColorIndex underwear_color_index = 0;

	Equipment equipment{};
};

class Player : public Entity {
public:
	Player(const PlayerAppearance& appear);
	void OnUpdate() override;
	void OnRender() const override;
	void OnRenderShadow() const override;
	PAKLib::sprite_rect GetEntityBounds() const override;

    PlayerAppearance& GetAppearance() {
        return appearance;
    }
    void UpdateAppearance(const PlayerAppearance& appear) {
        appearance = appear;
	}
    AnimationType GetCurrentAnimationType() const {
        return current_animation_type;
	}
	void SetAnimation(AnimationType newType, WeaponUsed newWeapon) override;

protected:
	void DrawModelItem(int16_t item_id, Color item_color, bool is_shadow = false) const;

	PlayerAppearance appearance{};

private:
    Stance current_stance = PEACE;
    WeaponUsed current_weapon_type = HAND;
	const Animation stand_animation		    = Animation(0, 8, 0.1);
	const Animation walk_animation		    = Animation(0, 8, 0.085);
	const Animation run_animation		    = Animation(0, 8, 0.070);
	const Animation attack_animation		= Animation(0, 8, 0.1);
	const Animation cast_animation		    = Animation(0, 16, 0.1);
	const Animation pickup_animation		= Animation(0, 4, 0.125);
	const Animation takedamage_animation    = Animation(0, 4, 0.115);
	const Animation dying_animation		    = Animation(0, 8, 0.125);
};

inline size_t _GetPlayerAnimation(size_t baseIndex, Stance stance, AnimationType type, WeaponUsed weapon)
{
    switch (type)
    {
    case STAND:
        return baseIndex + (stance == PEACE ? SPR_GAME_ANIMATIONS::PAK_INDEX_STANDING_PEACE : SPR_GAME_ANIMATIONS::PAK_INDEX_STANDING_COMBAT);

    case WALK:
        return baseIndex + (stance == PEACE ? SPR_GAME_ANIMATIONS::PAK_INDEX_WALKING_PEACE : SPR_GAME_ANIMATIONS::PAK_INDEX_WALKING_COMBAT);

    case RUN:
        return baseIndex + SPR_GAME_ANIMATIONS::PAK_INDEX_RUNNING;

    case ATTACK:
        if (stance == PEACE)
            return baseIndex + SPR_GAME_ANIMATIONS::PAK_INDEX_ATTACKING_PEACE;

        switch (weapon)
        {
        case HAND: return baseIndex + SPR_GAME_ANIMATIONS::PAK_INDEX_ATTACKING_HAND;
        case BOW:  return baseIndex + SPR_GAME_ANIMATIONS::PAK_INDEX_ATTACKING_BOW;
        case CAST: return baseIndex + SPR_GAME_ANIMATIONS::PAK_INDEX_ATTACKING_CAST;
        }
        return baseIndex;

    case PICKUP:
        return baseIndex + SPR_GAME_ANIMATIONS::PAK_INDEX_PICKUP;

    case TAKEDAMAGE:
        return baseIndex + SPR_GAME_ANIMATIONS::PAK_INDEX_TAKEDAMAGE;

    case DYING:
        return baseIndex + SPR_GAME_ANIMATIONS::PAK_INDEX_DYING;

    default:
        return baseIndex;
    }
}

inline size_t GetPlayerUnderwearAnimationIndex(Stance stance, AnimationType type, WeaponUsed weapon, Gender gender)
{
    size_t baseIndex = 0;
    if (gender == GENDER_MALE) baseIndex = MDLID_MALE_UNDERWEAR;
	else if(gender == GENDER_FEMALE) baseIndex = MDLID_FEMALE_UNDERWEAR;

    return _GetPlayerAnimation(baseIndex, stance, type, weapon);
}

inline size_t GetPlayerHairAnimationIndex(Stance stance, AnimationType type, WeaponUsed weapon, Gender gender, HairStyle style)
{
    size_t baseIndex = 0;

    if (gender == GENDER_MALE)
    {
        switch (style)
        {
        case HAIR_STYLE_1: baseIndex = MDLID_MALE_HAIRSTYLE_1; break;
        case HAIR_STYLE_2: baseIndex = MDLID_MALE_HAIRSTYLE_2; break;
        case HAIR_STYLE_3: baseIndex = MDLID_MALE_HAIRSTYLE_3; break;
        case HAIR_STYLE_4: baseIndex = MDLID_MALE_HAIRSTYLE_4; break;
        case HAIR_STYLE_5: baseIndex = MDLID_MALE_HAIRSTYLE_5; break;
        case HAIR_STYLE_6: baseIndex = MDLID_MALE_HAIRSTYLE_6; break;
        case HAIR_STYLE_7: baseIndex = MDLID_MALE_HAIRSTYLE_7; break;
        default: return 0;
        }
    }
    else if(gender == GENDER_FEMALE) {
        switch (style)
        {
        case HAIR_STYLE_1: baseIndex = MDLID_FEMALE_HAIRSTYLE_1; break;
        case HAIR_STYLE_2: baseIndex = MDLID_FEMALE_HAIRSTYLE_2; break;
        case HAIR_STYLE_3: baseIndex = MDLID_FEMALE_HAIRSTYLE_3; break;
        case HAIR_STYLE_4: baseIndex = MDLID_FEMALE_HAIRSTYLE_4; break;
        case HAIR_STYLE_5: baseIndex = MDLID_FEMALE_HAIRSTYLE_5; break;
        case HAIR_STYLE_6: baseIndex = MDLID_FEMALE_HAIRSTYLE_6; break;
        case HAIR_STYLE_7: baseIndex = MDLID_FEMALE_HAIRSTYLE_7; break;
        default: return 0;
        }
    }

    return _GetPlayerAnimation(baseIndex, stance, type, weapon);
}

inline size_t GetPlayerAnimationIndex(Stance stance, AnimationType type, WeaponUsed weapon, Gender gender)
{
	size_t baseIndex = 0;
    if (gender == GENDER_MALE) baseIndex = MDLID_MALE_GAME_MODEL;
    else if (gender == GENDER_FEMALE) baseIndex = MDLID_FEMALE_GAME_MODEL;

    return _GetPlayerAnimation(baseIndex, stance, type, weapon);
}

inline size_t GetPlayerItemAnimationIndex(int16_t itemId, Stance stance, AnimationType type, WeaponUsed weapon, Gender gender)
{
    size_t baseIndex = 0;
    // ItemID * 24 because each item has 24 sprite sheets, 12 are for male aned 12 are for female.
    if (gender == GENDER_MALE) baseIndex = MDLID_MALE_ITEM_START + (itemId * 24);
    else if (gender == GENDER_FEMALE) baseIndex = MDLID_FEMALE_ITEM_START + (itemId * 24);

    return _GetPlayerAnimation(baseIndex, stance, type, weapon);
}
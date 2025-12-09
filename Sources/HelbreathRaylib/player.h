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
    bool try_equip(int16_t item_id, const std::vector<ItemMetadataEntry>& _metadata)
    {
        const auto It = std::find_if(_metadata.begin(), _metadata.end(),
			[item_id](const ItemMetadataEntry& entry) { return entry.id == static_cast<uint32_t>(item_id); });
        if (It == _metadata.end())
			return false;
		const ItemMetadataEntry& Metadata = *It;
        if (Metadata.equip_type != Type)
            return false;
        id = item_id;
        return true;
	}

	void unequip() { id = -1; }

	const int16_t& get_id() const { return id; }
    static constexpr EquipType get_type() { return Type; }
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
	void on_update() override;
	void on_render() const override;
	void on_render_shadow() const override;
	PAKLib::sprite_rect get_entity_bounds() const override;

    PlayerAppearance& get_appearance() {
        return appearance;
    }
    void update_appearance(const PlayerAppearance& appear) {
        appearance = appear;
	}
    AnimationType get_current_animation_type() const {
        return current_animation_type;
	}
	void set_animation(AnimationType new_type, WeaponUsed new_weapon) override;

protected:
	void _draw_model_item(int16_t item_id, raylib::Color item_color, bool is_shadow = false) const;

	PlayerAppearance appearance{};

private:
    Stance _current_stance = PEACE;
    WeaponUsed _current_weapon_type = HAND;
	const Animation _stand_animation		    = Animation(0, 8, 0.1);
	const Animation _walk_animation		    = Animation(0, 8, 0.085);
	const Animation _run_animation		    = Animation(0, 8, 0.070);
	const Animation _attack_animation		= Animation(0, 8, 0.1);
	const Animation _cast_animation		    = Animation(0, 16, 0.1);
	const Animation _pickup_animation		= Animation(0, 4, 0.125);
	const Animation _takedamage_animation    = Animation(0, 4, 0.115);
	const Animation _dying_animation		    = Animation(0, 8, 0.125);
};

inline size_t _get_player_animation(size_t base_index, Stance stance, AnimationType type, WeaponUsed weapon)
{
    switch (type)
    {
    case STAND:
        return base_index + (stance == PEACE ? SPR_GAME_ANIMATIONS::PAK_INDEX_STANDING_PEACE : SPR_GAME_ANIMATIONS::PAK_INDEX_STANDING_COMBAT);

    case WALK:
        return base_index + (stance == PEACE ? SPR_GAME_ANIMATIONS::PAK_INDEX_WALKING_PEACE : SPR_GAME_ANIMATIONS::PAK_INDEX_WALKING_COMBAT);

    case RUN:
        return base_index + SPR_GAME_ANIMATIONS::PAK_INDEX_RUNNING;

    case ATTACK:
        if (stance == PEACE)
            return base_index + SPR_GAME_ANIMATIONS::PAK_INDEX_ATTACKING_PEACE;

        switch (weapon)
        {
        case HAND: return base_index + SPR_GAME_ANIMATIONS::PAK_INDEX_ATTACKING_HAND;
        case BOW:  return base_index + SPR_GAME_ANIMATIONS::PAK_INDEX_ATTACKING_BOW;
        case CAST: return base_index + SPR_GAME_ANIMATIONS::PAK_INDEX_ATTACKING_CAST;
        }
        return base_index;

    case PICKUP:
        return base_index + SPR_GAME_ANIMATIONS::PAK_INDEX_PICKUP;

    case TAKEDAMAGE:
        return base_index + SPR_GAME_ANIMATIONS::PAK_INDEX_TAKEDAMAGE;

    case DYING:
        return base_index + SPR_GAME_ANIMATIONS::PAK_INDEX_DYING;

    default:
        return base_index;
    }
}

inline size_t get_player_underwear_animation_index(Stance stance, AnimationType type, WeaponUsed weapon, Gender gender)
{
    size_t BaseIndex = 0;
    if (gender == GENDER_MALE) BaseIndex = MDLID_MALE_UNDERWEAR;
	else if(gender == GENDER_FEMALE) BaseIndex = MDLID_FEMALE_UNDERWEAR;

    return _get_player_animation(BaseIndex, stance, type, weapon);
}

inline size_t get_player_hair_animation_index(Stance stance, AnimationType type, WeaponUsed weapon, Gender gender, HairStyle style)
{
    size_t BaseIndex = 0;

    if (gender == GENDER_MALE)
    {
        switch (style)
        {
        case HAIR_STYLE_1: BaseIndex = MDLID_MALE_HAIRSTYLE_1; break;
        case HAIR_STYLE_2: BaseIndex = MDLID_MALE_HAIRSTYLE_2; break;
        case HAIR_STYLE_3: BaseIndex = MDLID_MALE_HAIRSTYLE_3; break;
        case HAIR_STYLE_4: BaseIndex = MDLID_MALE_HAIRSTYLE_4; break;
        case HAIR_STYLE_5: BaseIndex = MDLID_MALE_HAIRSTYLE_5; break;
        case HAIR_STYLE_6: BaseIndex = MDLID_MALE_HAIRSTYLE_6; break;
        case HAIR_STYLE_7: BaseIndex = MDLID_MALE_HAIRSTYLE_7; break;
        default: return 0;
        }
    }
    else if(gender == GENDER_FEMALE) {
        switch (style)
        {
        case HAIR_STYLE_1: BaseIndex = MDLID_FEMALE_HAIRSTYLE_1; break;
        case HAIR_STYLE_2: BaseIndex = MDLID_FEMALE_HAIRSTYLE_2; break;
        case HAIR_STYLE_3: BaseIndex = MDLID_FEMALE_HAIRSTYLE_3; break;
        case HAIR_STYLE_4: BaseIndex = MDLID_FEMALE_HAIRSTYLE_4; break;
        case HAIR_STYLE_5: BaseIndex = MDLID_FEMALE_HAIRSTYLE_5; break;
        case HAIR_STYLE_6: BaseIndex = MDLID_FEMALE_HAIRSTYLE_6; break;
        case HAIR_STYLE_7: BaseIndex = MDLID_FEMALE_HAIRSTYLE_7; break;
        default: return 0;
        }
    }

    return _get_player_animation(BaseIndex, stance, type, weapon);
}

inline size_t get_player_animation_index(Stance stance, AnimationType type, WeaponUsed weapon, Gender gender)
{
	size_t BaseIndex = 0;
    if (gender == GENDER_MALE) BaseIndex = MDLID_MALE_GAME_MODEL;
    else if (gender == GENDER_FEMALE) BaseIndex = MDLID_FEMALE_GAME_MODEL;

    return _get_player_animation(BaseIndex, stance, type, weapon);
}

inline size_t get_player_item_animation_index(int16_t item_id, Stance stance, AnimationType type, WeaponUsed weapon, Gender gender)
{
    size_t BaseIndex = 0;
    // ItemID * 24 because each item has 24 sprite sheets, 12 are for male aned 12 are for female.
    if (gender == GENDER_MALE) BaseIndex = MDLID_MALE_ITEM_START + (item_id * 24);
    else if (gender == GENDER_FEMALE) BaseIndex = MDLID_FEMALE_ITEM_START + (item_id * 24);

    return _get_player_animation(BaseIndex, stance, type, weapon);
}
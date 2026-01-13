#pragma once
#include "entity.h"
#include <array>
#include <typeinfo>
#include <functional>
#include <algorithm>
#include "ItemMetadata.h"
#include "json.hpp"

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

	// Allow JSON deserialization to set the id
	void set_id_from_save(int16_t saved_id) { id = saved_id; }

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
	Player(const PlayerAppearance& appear, const PlayerEntityStats& stats);
	void on_update() override;
	void on_render() const override;
	void on_render_shadow() const override;
	PAKLib::sprite_rect get_entity_bounds() const override;

    PlayerAppearance& get_appearance() {
        return appearance;
    }

    const PlayerAppearance& get_appearance() const {
        return appearance;
	}

    void update_appearance(const PlayerAppearance& appear) {
        appearance = appear;
	}
    AnimationType get_current_animation_type() const {
        return current_animation_type;
	}
	void set_animation(AnimationType new_type, WeaponUsed new_weapon) override;

    const PlayerEntityStats& get_player_stats() const {
        return _player_stats;
    }

	PlayerEntityStats& get_player_stats() {
		return _player_stats;
	}

    void commit_new_player_stats(const PlayerEntityStats& new_stats) {
        _player_stats = new_stats;
	}

	// Attack targeting
	void set_attack_target(Entity* target);
	void clear_attack_target();
	Entity* get_attack_target() const { return _attack_target; }
	bool is_attacking() const { return _is_attacking; }

	// Pickup action - triggers when player clicks on themselves
	// Returns true if pickup animation started, false if busy with other action
	bool try_pickup();
	bool is_picking_up() const { return _is_picking_up; }

protected:
	// Combat overrides
	int on_before_attack(Entity* target) override;
	void on_after_attack(Entity* target) override;
	bool on_receive_attack(Entity* attacker, int damage) override;

	// Dash movement overrides
	void on_before_movement_update() override;
	void on_dash_complete() override;

	void _draw_model_item(int16_t item_id, raylib::Color item_color, bool is_shadow = false) const;

	// Combat handling - returns true if input should be blocked (e.g., during attack animation)
	bool _handle_combat();

	PlayerAppearance appearance{};
    PlayerEntityStats _player_stats{};

private:
    Stance _current_stance = PEACE;
    WeaponUsed _current_weapon_type = HAND;
	const Animation _stand_animation		    = Animation(0, 8, 0.1);
	const Animation _walk_animation		    = Animation(0, 8, 0.085);
	const Animation _run_animation		    = Animation(0, 8, 0.047);  // ~1.8x faster than walk (matching original ratio)
	const Animation _attack_animation_def	= Animation(0, 8, 0.1, false);  // non-looping for attack
	const Animation _cast_animation_def		= Animation(0, 16, 0.1, false); // non-looping for cast
	const Animation _pickup_animation		= Animation(0, 4, 0.125, false);  // non-looping for pickup
	const Animation _takedamage_animation    = Animation(0, 4, 0.115);
	const Animation _dying_animation		    = Animation(0, 8, 0.125, false);  // non-looping for dying
	const Animation _dash_attack_stage1		= Animation(0, 5, 0.05, false);   // dash-attack stage 1: frames 0-4 @ 2x speed
	const Animation _dash_attack_stage2		= Animation(0, 1, 0.1, false);    // dash-attack stage 2: just frame 5, then cooldown

	// Attack targeting
	Entity* _attack_target = nullptr;
	bool _is_attacking = false;
	bool _attack_will_hit = false;  // Pre-rolled hit check for current attack

	// Critical strike state
	bool _is_critical_attack = false;  // Current attack is a distance critical
	bool _is_dash_attack = false;      // Current attack is a dash-attack
	bool _dash_attack_pending = false; // Waiting for dash to complete before attacking
	size_t _animation_frame_offset = 0; // Offset added to animation frame (for dash-attack finish)
	float _dash_attack_hold_time = 0.15f; // Time to hold final frame of dash-attack
	float _dash_attack_cooldown = 0.0f; // Cooldown after dash-attack where new attacks are blocked

	// Dash ghost trail effect - stores complete snapshot of player state at capture time
	struct DashGhost {
		int pixel_x;
		int pixel_y;
		size_t display_frame;  // Pre-computed: frame + (direction * 8)
		Dir direction;
		float opacity;         // 0.0 to 1.0, starts at 0.75 and fades to 0
		float fade_timer;      // Time remaining before fully faded
		// Store animation state at capture time for independent rendering
		Stance stance;
		AnimationType anim_type;
		WeaponUsed weapon;
	};
	static constexpr float GHOST_START_OPACITY = 0.9f;
	static constexpr float GHOST_FADE_DURATION = 0.5f;  // seconds to fully fade out
	static constexpr size_t MAX_DASH_GHOSTS = 10;
	std::array<DashGhost, MAX_DASH_GHOSTS> _dash_ghosts{};
	size_t _dash_ghost_count = 0;
	// Track which animation frames we've captured (bitmask - bit N = frame N captured)
	uint32_t _dash_captured_frames = 0;
	// Store dash start/end for manual interpolation
	int32_t _dash_start_px = 0;
	int32_t _dash_start_py = 0;
	int32_t _dash_end_px = 0;
	int32_t _dash_end_py = 0;
	void _update_dash_ghosts();
	void _fade_dash_ghosts();
	void _render_dash_ghosts() const;
	void _clear_dash_ghosts();

	// Pickup state
	bool _is_picking_up = false;

	// Helper methods
	int _get_tile_distance_to(const Entity* target) const;
	int _get_critical_range() const;  // Returns 2 for normal weapons, 3 for LARGE_* weapons
	bool _can_critical_attack(const Entity* target) const;
	bool _is_direct_line_to(const Entity* target, int& out_dx, int& out_dy) const;  // Check if target is on straight/diagonal line
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

// JSON serialization for enums
NLOHMANN_JSON_SERIALIZE_ENUM(Gender, {
    {GENDER_MALE, "male"},
    {GENDER_FEMALE, "female"}
})

NLOHMANN_JSON_SERIALIZE_ENUM(HairStyle, {
    {HAIR_STYLE_0, "bald"},
    {HAIR_STYLE_1, "style1"},
    {HAIR_STYLE_2, "style2"},
    {HAIR_STYLE_3, "style3"},
    {HAIR_STYLE_4, "style4"},
    {HAIR_STYLE_5, "style5"},
    {HAIR_STYLE_6, "style6"},
    {HAIR_STYLE_7, "style7"}
})

// JSON serialization for Equip template
template<EquipType Type>
inline void to_json(nlohmann::json& j, const Equip<Type>& e) {
    j = nlohmann::json{
        {"id", e.get_id()},
        {"type", static_cast<int>(Type)}
    };
}

template<EquipType Type>
inline void from_json(const nlohmann::json& j, Equip<Type>& e) {
    int16_t id = j.value("id", static_cast<int16_t>(-1));
    e.set_id_from_save(id);
}

// JSON serialization for Equipment
inline void to_json(nlohmann::json& j, const Equipment& eq) {
    j = nlohmann::json{
        {"head", eq.head},
        {"back", eq.back},
        {"shirt", eq.shirt},
        {"chest", eq.chest},
        {"legs", eq.legs},
        {"feet", eq.feet},
        {"weapon", eq.weapon},
        {"shield", eq.shield},
        {"accessories", nlohmann::json::array()}
    };
    for (const auto& acc : eq.accessories) {
        j["accessories"].push_back(acc);
    }
}

inline void from_json(const nlohmann::json& j, Equipment& eq) {
    if (j.contains("head")) eq.head = j["head"].get<Equip<EquipType::Head>>();
    if (j.contains("back")) eq.back = j["back"].get<Equip<EquipType::Back>>();
    if (j.contains("shirt")) eq.shirt = j["shirt"].get<Equip<EquipType::Shirt>>();
    if (j.contains("chest")) eq.chest = j["chest"].get<Equip<EquipType::Chest>>();
    if (j.contains("legs")) eq.legs = j["legs"].get<Equip<EquipType::Legs>>();
    if (j.contains("feet")) eq.feet = j["feet"].get<Equip<EquipType::Feet>>();
    if (j.contains("weapon")) eq.weapon = j["weapon"].get<Equip<EquipType::Weapon>>();
    if (j.contains("shield")) eq.shield = j["shield"].get<Equip<EquipType::Shield>>();
    if (j.contains("accessories") && j["accessories"].is_array()) {
        auto& acc_array = j["accessories"];
        for (size_t i = 0; i < std::min(acc_array.size(), eq.accessories.size()); ++i) {
            eq.accessories[i] = acc_array[i].get<Equip<EquipType::Accessory>>();
        }
    }
}

// JSON serialization for PlayerAppearance
inline void to_json(nlohmann::json& j, const PlayerAppearance& a) {
    j = nlohmann::json{
        {"gender", a.gender},
        {"skin_color_index", a.skin_color_index},
        {"hair_style", a.hair_style},
        {"hair_color_index", a.hair_color_index},
        {"underwear_color_index", a.underwear_color_index},
        {"equipment", a.equipment}
    };
}

inline void from_json(const nlohmann::json& j, PlayerAppearance& a) {
    if (j.contains("gender")) {
        a.gender = j["gender"].get<Gender>();
    }
    if (j.contains("skin_color_index")) {
        a.skin_color_index = j["skin_color_index"].get<SkinColorIndex>();
    }
    if (j.contains("hair_style")) {
        a.hair_style = j["hair_style"].get<HairStyle>();
    }
    if (j.contains("hair_color_index")) {
        a.hair_color_index = j["hair_color_index"].get<HairColorIndex>();
    }
    if (j.contains("underwear_color_index")) {
        a.underwear_color_index = j["underwear_color_index"].get<UnderwearColorIndex>();
    }
    if (j.contains("equipment")) {
        a.equipment = j["equipment"].get<Equipment>();
    }
}

// JSON serialization for Player
inline void to_json(nlohmann::json& j, const Player& p) {
    j = nlohmann::json{
        {"appearance", p.get_appearance()},
        {"stats", p.get_player_stats()}
    };
}

inline void from_json(const nlohmann::json& j, Player& p) {
    if (j.contains("appearance")) {
        PlayerAppearance appearance = j["appearance"].get<PlayerAppearance>();
        p.update_appearance(appearance);
    }
    if (j.contains("stats")) {
        PlayerEntityStats stats = j["stats"].get<PlayerEntityStats>();
        p.commit_new_player_stats(stats);
    }
}
#pragma once
#include <vector>
#include <algorithm>
#include <cstdint>
#include "json.hpp"

// Constants for stat bounds
constexpr uint16_t MAX_LEVEL = 140;
constexpr uint8_t LEVELUP_STAT_POINTS = 3;
constexpr uint8_t STARTING_STAT_POINTS = 10;
constexpr uint8_t STARTING_STAT_MAX_PER_STAT = 4;  // Max points allocatable to single stat at creation

// Theoretical max stat: base (10) + creation allocation (4) + level-up points (140 * 3)
// = 10 + 4 + 420 = 434
constexpr uint16_t MAX_STAT = 10 + STARTING_STAT_MAX_PER_STAT + (MAX_LEVEL * LEVELUP_STAT_POINTS);
constexpr uint16_t BASE_STAT_VALUE = 10;

// =============================================================================
// BaseStat - Used for player base stats (STR, VIT, INT, DEX, MAG, CHA)
// Has fixed min (10) and max (434) bounds with clamping
// =============================================================================
struct BaseStat {
	const uint16_t min_value = 10;
	const uint16_t max_value = MAX_STAT;
private:
	uint16_t current_value = min_value;

	void set(uint16_t value) {
		if (value > max_value)
			current_value = max_value;
		else if (value < min_value)
			current_value = min_value;
		else
			current_value = static_cast<uint16_t>(value);
	}

	void add(uint8_t amount) {
		int result = current_value + amount;
		if (result > max_value) current_value = max_value;
		else if (result < static_cast<int>(min_value)) current_value = min_value;
		else current_value = static_cast<uint16_t>(result);
	}

	void minus(uint8_t amount) {
		int result = current_value - amount;
		if (result > static_cast<int>(max_value)) current_value = max_value;
		else if (result < static_cast<int>(min_value)) current_value = min_value;
		else current_value = static_cast<uint16_t>(result);
	}

public:
	BaseStat() = default;
	BaseStat(const BaseStat&) = default;
	BaseStat(BaseStat&&) = default;

	template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	BaseStat(T value) { set(static_cast<uint16_t>(value)); }

	bool is_maxed() const { return current_value == max_value; }
	bool is_minimum() const { return current_value == min_value; }

	bool is_out_of_bounds() const {
		return static_cast<int>(current_value) < min_value || static_cast<int>(current_value) > max_value;
	}

	constexpr operator uint16_t() const { return current_value; }

	template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	uint16_t operator+(T amount) const {
		int result = current_value + static_cast<int>(amount);
		if (result > max_value) return max_value;
		if (result < min_value) return min_value;
		return static_cast<uint16_t>(result);
	}

	template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	uint16_t operator-(T amount) const {
		int result = current_value - static_cast<int>(amount);
		if (result > max_value) return max_value;
		if (result < min_value) return min_value;
		return static_cast<uint16_t>(result);
	}

	template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	uint16_t operator+=(T amount) {
		add(static_cast<uint8_t>(amount));
		return current_value;
	}

	template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	uint16_t operator-=(T amount) {
		minus(static_cast<uint8_t>(amount));
		return current_value;
	}

	template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	uint16_t operator=(T value) {
		set(static_cast<uint16_t>(value));
		return current_value;
	}

	// Prefix
	BaseStat& operator++() {
		add(1);
		return *this;
	}

	BaseStat& operator--() {
		minus(1);
		return *this;
	}

	// Postfix
	BaseStat operator++(int) {
		BaseStat temp = *this;
		add(1);
		return temp;
	}

	BaseStat operator--(int) {
		BaseStat temp = *this;
		minus(1);
		return temp;
	}

	BaseStat& operator=(const BaseStat& other) {
		set(static_cast<uint16_t>(other.current_value));
		return *this;
	}

	BaseStat& operator=(BaseStat&& other) noexcept {
		current_value = other.current_value;
		return *this;
	}
};

// JSON serialization for BaseStat
inline void to_json(nlohmann::json& j, const BaseStat& s) {
	j = static_cast<uint16_t>(s);
}

inline void from_json(const nlohmann::json& j, BaseStat& s) {
	s = j.get<uint16_t>();
}

// Alias for backwards compatibility
using Stat = BaseStat;

// =============================================================================
// StatType - Types of dynamic stats for entities (HEALTH, MANA, etc.)
// =============================================================================
enum class StatType : uint8_t {
	LEVEL,
	HEALTH,
	MANA,
	EXPERIENCE,
	STAMINA,
	WEIGHT,
	MAX_ITEMS,
	RUN_SPEED,
	WALK_SPEED,
	ATTACK_SPEED,
	EK_COUNT,
	EK_EARN,
	EXPERIENCE_EARN,
	AGRO_RANGE,
	WANDER_RANGE_X,
	WANDER_RANGE_Y,
	BASE_DAMAGE,
	ADDITIONAL_DAMAGE,
	AVAILABLE_STAT_POINTS,
	HIT_RATIO,           // Can be negative or positive, affects hit chance
	NEXT_LEVEL_EXPERIENCE,
	CRITICALS,           // Critical strike charges (Super Attacks)

	// Base stats marker - stats after this are "base stats" (STR, VIT, etc.)
	__BASE_BEGIN,
	STRENGTH,
	VITALITY,
	INTELLIGENCE,
	DEXTERITY,
	MAGIC,
	CHARISMA
};

// =============================================================================
// EntityStat - A single dynamic stat with value/maxValue and behavior flags
// =============================================================================
struct EntityStat {
	StatType type;
	uint32_t value;
	uint32_t max_value;
	bool is_percentage;        // Is this stat expressed as a percentage?
	bool can_max_be_modified;  // Can max_value be changed?
	bool can_exceed_maximum;   // Can value exceed max_value?

	EntityStat(StatType type, uint32_t value, uint32_t max_value,
		bool is_percentage = false, bool can_max_be_modified = false, bool can_exceed_maximum = false)
		: type(type), value(value), max_value(max_value),
		is_percentage(is_percentage), can_max_be_modified(can_max_be_modified),
		can_exceed_maximum(can_exceed_maximum) {}
};

// JSON serialization for EntityStat
inline void to_json(nlohmann::json& j, const EntityStat& s) {
	j = nlohmann::json{
		{"type", static_cast<int>(s.type)},
		{"value", s.value},
		{"max_value", s.max_value},
		{"is_percentage", s.is_percentage},
		{"can_max_be_modified", s.can_max_be_modified},
		{"can_exceed_maximum", s.can_exceed_maximum}
	};
}

inline void from_json(const nlohmann::json& j, EntityStat& s) {
	s.type = static_cast<StatType>(j.at("type").get<int>());
	s.value = j.at("value").get<uint32_t>();
	s.max_value = j.at("max_value").get<uint32_t>();
	s.is_percentage = j.value("is_percentage", false);
	s.can_max_be_modified = j.value("can_max_be_modified", false);
	s.can_exceed_maximum = j.value("can_exceed_maximum", false);
}

// =============================================================================
// EntityStats - Manages all stats for an entity
// =============================================================================
class EntityStats {
public:
	EntityStats() {
		// Default entity stats
		_stats.emplace_back(StatType::LEVEL, 1, MAX_LEVEL);
		_stats.emplace_back(StatType::HEALTH, 100, 100, false, true);
		_stats.emplace_back(StatType::MANA, 100, 100, false, true);
		_stats.emplace_back(StatType::RUN_SPEED, 200, 200, true, true);
		_stats.emplace_back(StatType::WALK_SPEED, 100, 100, true, true);
		_stats.emplace_back(StatType::ATTACK_SPEED, 1000, 1000, true, true);
		_stats.emplace_back(StatType::EXPERIENCE_EARN, 1, 1);
		_stats.emplace_back(StatType::AGRO_RANGE, 5, 5, false, true);
		_stats.emplace_back(StatType::WANDER_RANGE_X, 0, 5, false, true);
		_stats.emplace_back(StatType::WANDER_RANGE_Y, 0, 5, false, true);
		_stats.emplace_back(StatType::BASE_DAMAGE, 1, 5);
		_stats.emplace_back(StatType::ADDITIONAL_DAMAGE, 0, 0, false, true);
	}

	virtual ~EntityStats() = default;

	// Virtual methods for derived classes to override stat calculations
	virtual void calculate_weight() {}
	virtual void calculate_stamina() {}
	virtual void calculate_health() {}
	virtual void calculate_mana() {}
	virtual uint32_t get_total_base_stats() { return 0; }
	virtual void set_to_max_by_stat(StatType type) {}

	void add_experience(int value) {
		auto it = std::find_if(_stats.begin(), _stats.end(),
			[](const EntityStat& stat) { return stat.type == StatType::EXPERIENCE; });
		if (it == _stats.end()) return;

		set_stat(StatType::EXPERIENCE, get_stat(StatType::EXPERIENCE) + value);
	}

	void set_stat(StatType type, uint32_t value, int32_t max_value = -1) {
		if (max_value < -1) return;

		// Check if it's a base stat
		if (static_cast<int>(type) > static_cast<int>(StatType::__BASE_BEGIN)) {
			for (auto& stat : _base_stats) {
				if (stat.type == type) {
					if (max_value != -1)
						stat.max_value = static_cast<uint32_t>(max_value);
					if (!stat.can_exceed_maximum && stat.max_value < value)
						return;
					stat.value = value;
					return;
				}
			}
		}
		else {
			for (auto& stat : _stats) {
				if (stat.type == type) {
					if (max_value != -1)
						stat.max_value = static_cast<uint32_t>(max_value);
					if (!stat.can_exceed_maximum && stat.max_value < value)
						value = stat.max_value;
					stat.value = value;
					return;
				}
			}
		}
	}

	uint32_t get_stat(StatType type) const {
		if (static_cast<int>(type) > static_cast<int>(StatType::__BASE_BEGIN)) {
			for (const auto& stat : _base_stats) {
				if (stat.type == type) return stat.value;
			}
		}
		else {
			for (const auto& stat : _stats) {
				if (stat.type == type) return stat.value;
			}
		}
		return 0;
	}

	uint32_t get_max_stat(StatType type) const {
		if (static_cast<int>(type) > static_cast<int>(StatType::__BASE_BEGIN)) {
			for (const auto& stat : _base_stats) {
				if (stat.type == type) return stat.max_value;
			}
		}
		else {
			for (const auto& stat : _stats) {
				if (stat.type == type) return stat.max_value;
			}
		}
		return 0;
	}

	bool is_stat_percentage(StatType type) const {
		if (static_cast<int>(type) > static_cast<int>(StatType::__BASE_BEGIN)) {
			for (const auto& stat : _base_stats) {
				if (stat.type == type) return stat.is_percentage;
			}
		}
		else {
			for (const auto& stat : _stats) {
				if (stat.type == type) return stat.is_percentage;
			}
		}
		return false;
	}

	const std::vector<EntityStat>& get_stats() const { return _stats; }
	const std::vector<EntityStat>& get_base_stats() const { return _base_stats; }

	// Check if health is at zero or below
	bool is_health_depleted() const { return get_stat(StatType::HEALTH) == 0; }

	// Reduce health by damage amount, returns actual damage dealt
	uint32_t take_damage(uint32_t damage) {
		uint32_t current_hp = get_stat(StatType::HEALTH);
		if (damage >= current_hp) {
			set_stat(StatType::HEALTH, 0);
			return current_hp;
		}
		set_stat(StatType::HEALTH, current_hp - damage);
		return damage;
	}

protected:
	std::vector<EntityStat> _stats;
	std::vector<EntityStat> _base_stats;
};

// JSON serialization for EntityStats
inline void to_json(nlohmann::json& j, const EntityStats& es) {
	j["stats"] = es.get_stats();
	j["base_stats"] = es.get_base_stats();
}

// =============================================================================
// PlayerEntityStats - Extended entity stats with base stats and calculations
// Uses formulas from the original PlayerStats system
// =============================================================================
class PlayerEntityStats : public EntityStats {
public:
	PlayerEntityStats() : EntityStats() {
		_stats.clear();
		_base_stats.clear();

		// Player-specific stats
		_stats.emplace_back(StatType::LEVEL, 1, MAX_LEVEL);
		_stats.emplace_back(StatType::HEALTH, 100, 100, false, true);
		_stats.emplace_back(StatType::MANA, 100, 100, false, true);
		_stats.emplace_back(StatType::RUN_SPEED, 200, 200, true, true);
		_stats.emplace_back(StatType::WALK_SPEED, 100, 100, true, true);
		_stats.emplace_back(StatType::ATTACK_SPEED, 500, 500, true, true);
		_stats.emplace_back(StatType::BASE_DAMAGE, 1, 7);
		_stats.emplace_back(StatType::ADDITIONAL_DAMAGE, 0, 0, false, true);
		_stats.emplace_back(StatType::EXPERIENCE, 0, 100, false, true, true);
		_stats.emplace_back(StatType::NEXT_LEVEL_EXPERIENCE, 100, 100, false, true);
		_stats.emplace_back(StatType::STAMINA, 100, 100, false, true);
		_stats.emplace_back(StatType::EK_COUNT, 0, 65535, false, false);
		_stats.emplace_back(StatType::WEIGHT, 0, 100, false, true);
		_stats.emplace_back(StatType::MAX_ITEMS, 10, 50, false, false);
		_stats.emplace_back(StatType::EK_EARN, 1, 1, false, false);
		_stats.emplace_back(StatType::AVAILABLE_STAT_POINTS, STARTING_STAT_POINTS, 0, false, true, true);
		_stats.emplace_back(StatType::HIT_RATIO, 0, 0, false, true, true);  // Can be negative via signed interpretation
		_stats.emplace_back(StatType::CRITICALS, 0, 1, false, true);  // Max starts at level/10 + 1

		// Base stats (STR, VIT, etc.)
		_base_stats.emplace_back(StatType::STRENGTH, BASE_STAT_VALUE, MAX_STAT);
		_base_stats.emplace_back(StatType::VITALITY, BASE_STAT_VALUE, MAX_STAT);
		_base_stats.emplace_back(StatType::INTELLIGENCE, BASE_STAT_VALUE, MAX_STAT);
		_base_stats.emplace_back(StatType::DEXTERITY, BASE_STAT_VALUE, MAX_STAT);
		_base_stats.emplace_back(StatType::MAGIC, BASE_STAT_VALUE, MAX_STAT);
		_base_stats.emplace_back(StatType::CHARISMA, BASE_STAT_VALUE, MAX_STAT);

		// Calculate initial values
		calculate_all();
	}

	void set_to_max_by_stat(StatType type) override {
		switch (type) {
		case StatType::HEALTH:
			calculate_health();
			set_stat(StatType::HEALTH, get_max_stat(StatType::HEALTH));
			break;
		case StatType::MANA:
			calculate_mana();
			set_stat(StatType::MANA, get_max_stat(StatType::MANA));
			break;
		case StatType::STAMINA:
			calculate_stamina();
			set_stat(StatType::STAMINA, get_max_stat(StatType::STAMINA));
			break;
		default:
			break;
		}
	}

	// Health formula: (VIT * 3) + (level * 2) + (STR / 2)
	void calculate_health() override {
		uint32_t vit = get_stat(StatType::VITALITY);
		uint32_t str = get_stat(StatType::STRENGTH);
		uint32_t level = get_stat(StatType::LEVEL);
		uint32_t current_hp = get_stat(StatType::HEALTH);

		uint32_t new_max = (vit * 3) + (level * 2) + (str / 2);
		set_stat(StatType::HEALTH, current_hp, new_max);

		if (current_hp > get_max_stat(StatType::HEALTH)) {
			set_stat(StatType::HEALTH, get_max_stat(StatType::HEALTH));
		}
	}

	// Mana formula: (MAG * 2) + (level * 2) + (INT / 2)
	void calculate_mana() override {
		uint32_t mag = get_stat(StatType::MAGIC);
		uint32_t intel = get_stat(StatType::INTELLIGENCE);
		uint32_t level = get_stat(StatType::LEVEL);
		uint32_t current_mp = get_stat(StatType::MANA);

		uint32_t new_max = (mag * 2) + (level * 2) + (intel / 2);
		set_stat(StatType::MANA, current_mp, new_max);

		if (current_mp > get_max_stat(StatType::MANA)) {
			set_stat(StatType::MANA, get_max_stat(StatType::MANA));
		}
	}

	// Stamina formula: (STR * 2) + (level * 2)
	void calculate_stamina() override {
		uint32_t str = get_stat(StatType::STRENGTH);
		uint32_t level = get_stat(StatType::LEVEL);
		uint32_t current_stam = get_stat(StatType::STAMINA);

		uint32_t new_max = (str * 2) + (level * 2);
		set_stat(StatType::STAMINA, current_stam, new_max);

		if (current_stam > get_max_stat(StatType::STAMINA)) {
			set_stat(StatType::STAMINA, get_max_stat(StatType::STAMINA));
		}
	}

	// Weight formula: (STR * 5) + (level * 5)
	void calculate_weight() override {
		uint32_t str = get_stat(StatType::STRENGTH);
		uint32_t level = get_stat(StatType::LEVEL);
		uint32_t current_weight = get_stat(StatType::WEIGHT);

		uint32_t new_max = (str * 5) + (level * 5);
		set_stat(StatType::WEIGHT, current_weight, new_max);
	}

	// Next level experience formula: 100 + (LVL * (LVL * 1.5)) + ((LVL^4 * 1) / (MAXLVL^2 / 20))
	void calculate_next_level_experience() {
		constexpr float max_level_f = static_cast<float>(MAX_LEVEL);
		float level_f = static_cast<float>(get_stat(StatType::LEVEL));
		uint32_t next_exp = static_cast<uint32_t>(
			100.0f + (level_f * (level_f * 1.5f)) +
			((level_f * level_f * level_f * level_f * 1.0f) / (max_level_f * max_level_f / 20.0f))
		);
		set_stat(StatType::NEXT_LEVEL_EXPERIENCE, next_exp, next_exp);
	}

	// Hit ratio calculation based on DEX and level
	// Dex contribution: 0 to 60 (based on proportion of max dex)
	// Level contribution: 0 to 30 (based on proportion of max level)
	void calculate_hit_ratio() {
		uint32_t dex = get_stat(StatType::DEXTERITY);
		uint32_t level = get_stat(StatType::LEVEL);

		float dex_factor = static_cast<float>(dex - BASE_STAT_VALUE) / static_cast<float>(MAX_STAT - BASE_STAT_VALUE) * 60.0f;
		float level_factor = static_cast<float>(level - 1) / static_cast<float>(MAX_LEVEL - 1) * 30.0f;

		int16_t ratio = static_cast<int16_t>(dex_factor + level_factor);
		// Store as uint32_t but treat as signed when reading
		set_stat(StatType::HIT_RATIO, static_cast<uint32_t>(ratio), 0);
	}

	// Get hit ratio as signed value
	int16_t get_hit_ratio() const {
		return static_cast<int16_t>(get_stat(StatType::HIT_RATIO));
	}

	// Convert hit ratio to percentage (0-100)
	// Formula: Base 50% + (hit_ratio * 0.5), clamped to 5%-95%
	static float hit_ratio_to_percentage(int16_t ratio) {
		float percentage = 50.0f + (ratio * 0.5f);
		if (percentage < 5.0f) percentage = 5.0f;
		if (percentage > 95.0f) percentage = 95.0f;
		return percentage;
	}

	// Check if an attack hits based on hit ratio
	bool roll_hit() const {
		float hit_chance = hit_ratio_to_percentage(get_hit_ratio());
		float roll = static_cast<float>(rand() % 100);
		return roll < hit_chance;
	}

	// Calculate max criticals based on level (level/10 + 1)
	void calculate_max_criticals() {
		uint32_t level = get_stat(StatType::LEVEL);
		uint32_t max_crits = (level / 10) + 1;
		uint32_t current_crits = get_stat(StatType::CRITICALS);
		set_stat(StatType::CRITICALS, current_crits, max_crits);
		// Clamp current to max if over
		if (current_crits > max_crits) {
			set_stat(StatType::CRITICALS, max_crits);
		}
	}

	// Add a critical charge (capped at max)
	void add_critical() {
		uint32_t current = get_stat(StatType::CRITICALS);
		uint32_t max_crits = get_max_stat(StatType::CRITICALS);
		if (current < max_crits) {
			set_stat(StatType::CRITICALS, current + 1);
		}
	}

	// Use a critical charge if available
	bool use_critical() {
		uint32_t current = get_stat(StatType::CRITICALS);
		if (current > 0) {
			set_stat(StatType::CRITICALS, current - 1);
			return true;
		}
		return false;
	}

	// Check if player has critical charges
	bool has_critical() const {
		return get_stat(StatType::CRITICALS) > 0;
	}

	// Get current critical count
	uint32_t get_criticals() const {
		return get_stat(StatType::CRITICALS);
	}

	// Roll critical damage: roll damage 3 times, return highest
	// This is used for distance-based critical attacks
	static int roll_critical_damage(int base_damage, int max_damage) {
		int highest = 0;
		for (int i = 0; i < 3; ++i) {
			int roll = base_damage + (rand() % (max_damage - base_damage + 1));
			if (roll > highest) highest = roll;
		}
		return highest;
	}

	// Calculate maximum allocatable stat points based on level
	uint16_t calculate_current_max_stat_points() const {
		return static_cast<uint16_t>((get_stat(StatType::LEVEL) * LEVELUP_STAT_POINTS) + STARTING_STAT_POINTS);
	}

	// Validate stat integrity - clamps out-of-bounds stats and resets if too many points allocated
	void check_statpoint_integrity() {
		int total_allocated = 0;
		for (auto& stat : _base_stats) {
			// Clamp stats to valid range
			if (stat.value < BASE_STAT_VALUE) stat.value = BASE_STAT_VALUE;
			if (stat.value > MAX_STAT) stat.value = MAX_STAT;
			total_allocated += static_cast<int>(stat.value) - BASE_STAT_VALUE;
		}

		// Get maximum allocatable stat points based on level
		int max_allocatable = static_cast<int>(calculate_current_max_stat_points());
		if (total_allocated > max_allocatable) {
			// Too many points allocated, reset stats
			for (auto& stat : _base_stats) {
				stat.value = BASE_STAT_VALUE;
			}
			set_stat(StatType::AVAILABLE_STAT_POINTS, static_cast<uint32_t>(max_allocatable), 0);
		}
	}

	// Add points to a base stat (amount can be negative to remove points)
	bool stat_add(StatType stat_type, int amount) {
		if (static_cast<int>(stat_type) <= static_cast<int>(StatType::__BASE_BEGIN)) {
			return false;  // Not a base stat
		}

		uint32_t current_value = get_stat(stat_type);
		int new_value = static_cast<int>(current_value) + amount;

		// Clamp to stat bounds
		if (new_value > static_cast<int>(MAX_STAT) || new_value < static_cast<int>(BASE_STAT_VALUE)) {
			return false;
		}

		uint32_t available_points = get_stat(StatType::AVAILABLE_STAT_POINTS);

		// When increasing (amount > 0), check we have enough points
		if (amount > 0 && available_points < static_cast<uint32_t>(amount)) {
			return false;
		}

		// When decreasing (amount < 0), check stat isn't already at minimum
		if (amount < 0 && current_value <= BASE_STAT_VALUE) {
			return false;
		}

		set_stat(StatType::AVAILABLE_STAT_POINTS, available_points - amount, 0);
		set_stat(stat_type, static_cast<uint32_t>(new_value));
		return true;
	}

	// Level up - increments level and adds stat points
	void level_up() {
		uint32_t current_level = get_stat(StatType::LEVEL);
		if (current_level < MAX_LEVEL) {
			set_stat(StatType::LEVEL, current_level + 1);
			uint32_t current_points = get_stat(StatType::AVAILABLE_STAT_POINTS);
			set_stat(StatType::AVAILABLE_STAT_POINTS, current_points + LEVELUP_STAT_POINTS, 0);
			calculate_all();
		}
	}

	// Calculate all derived stats
	void calculate_all() {
		check_statpoint_integrity();
		calculate_health();
		calculate_mana();
		calculate_stamina();
		calculate_weight();
		calculate_next_level_experience();
		calculate_hit_ratio();
		calculate_max_criticals();
	}

	uint32_t get_total_base_stats() override {
		uint32_t total = 0;
		for (const auto& stat : _base_stats) {
			total += stat.value;
		}
		// Subtract the base value (10 per stat)
		int allocated = static_cast<int>(total) - static_cast<int>(_base_stats.size() * BASE_STAT_VALUE);
		return static_cast<uint32_t>(std::max(0, allocated));
	}

	// Get the total available stat points
	uint32_t get_available_stat_points() const {
		return get_stat(StatType::AVAILABLE_STAT_POINTS);
	}

	// Set initial stat points (for character creation)
	void set_initial_stat_points(uint16_t points) {
		set_stat(StatType::AVAILABLE_STAT_POINTS, points, 0);
	}
};

// JSON serialization for PlayerEntityStats
inline void to_json(nlohmann::json& j, const PlayerEntityStats& ps) {
	// Save base stats
	nlohmann::json base_json;
	base_json["strength"] = ps.get_stat(StatType::STRENGTH);
	base_json["vitality"] = ps.get_stat(StatType::VITALITY);
	base_json["intelligence"] = ps.get_stat(StatType::INTELLIGENCE);
	base_json["dexterity"] = ps.get_stat(StatType::DEXTERITY);
	base_json["magic"] = ps.get_stat(StatType::MAGIC);
	base_json["charisma"] = ps.get_stat(StatType::CHARISMA);

	// Save derived/dynamic stats
	nlohmann::json derived_json;
	derived_json["level"] = ps.get_stat(StatType::LEVEL);
	derived_json["experience"] = ps.get_stat(StatType::EXPERIENCE);
	derived_json["health"] = ps.get_stat(StatType::HEALTH);
	derived_json["mana"] = ps.get_stat(StatType::MANA);
	derived_json["stamina"] = ps.get_stat(StatType::STAMINA);
	derived_json["stat_points"] = ps.get_stat(StatType::AVAILABLE_STAT_POINTS);
	derived_json["ek_count"] = ps.get_stat(StatType::EK_COUNT);
	derived_json["criticals"] = ps.get_stat(StatType::CRITICALS);

	j = nlohmann::json{
		{"base", base_json},
		{"derived", derived_json}
	};
}

inline void from_json(const nlohmann::json& j, PlayerEntityStats& ps) {
	// Load base stats
	if (j.contains("base")) {
		const auto& base = j["base"];
		if (base.contains("strength")) ps.set_stat(StatType::STRENGTH, base["strength"].get<uint32_t>());
		if (base.contains("vitality")) ps.set_stat(StatType::VITALITY, base["vitality"].get<uint32_t>());
		if (base.contains("intelligence")) ps.set_stat(StatType::INTELLIGENCE, base["intelligence"].get<uint32_t>());
		if (base.contains("dexterity")) ps.set_stat(StatType::DEXTERITY, base["dexterity"].get<uint32_t>());
		if (base.contains("magic")) ps.set_stat(StatType::MAGIC, base["magic"].get<uint32_t>());
		if (base.contains("charisma")) ps.set_stat(StatType::CHARISMA, base["charisma"].get<uint32_t>());
	}

	// Load derived stats
	if (j.contains("derived")) {
		const auto& derived = j["derived"];
		if (derived.contains("level")) ps.set_stat(StatType::LEVEL, derived["level"].get<uint32_t>());
		if (derived.contains("experience")) ps.set_stat(StatType::EXPERIENCE, derived["experience"].get<uint32_t>());
		if (derived.contains("health")) ps.set_stat(StatType::HEALTH, derived["health"].get<uint32_t>());
		if (derived.contains("mana")) ps.set_stat(StatType::MANA, derived["mana"].get<uint32_t>());
		if (derived.contains("stamina")) ps.set_stat(StatType::STAMINA, derived["stamina"].get<uint32_t>());
		if (derived.contains("stat_points")) ps.set_stat(StatType::AVAILABLE_STAT_POINTS, derived["stat_points"].get<uint32_t>(), 0);
		if (derived.contains("ek_count")) ps.set_stat(StatType::EK_COUNT, derived["ek_count"].get<uint32_t>());
		if (derived.contains("criticals")) ps.set_stat(StatType::CRITICALS, derived["criticals"].get<uint32_t>());
	}

	// Recalculate derived values after loading
	ps.calculate_all();
}

#pragma once
#include <cstdint>
#include <string_view>

// Each entity has exactly 40 sprites in its PAK file
constexpr size_t SPRITES_PER_ENTITY = 40;

// Entity ID definitions
enum class EntityID : uint16_t {
	DUMMY,
	SLIME,

	COUNT
};

// Calculate the starting index in entity_sprites for a given entity
constexpr size_t EntitySpriteBase(EntityID entity) {
	return static_cast<size_t>(entity) * SPRITES_PER_ENTITY;
}

// Calculate a specific sprite index for an entity (base + offset)
constexpr size_t EntitySpriteIndex(EntityID entity, size_t sprite_offset) {
	return EntitySpriteBase(entity) + sprite_offset;
}

// Entity PAK file mapping - add new entities here
struct EntityPakEntry {
	EntityID id;
	std::string_view pak_file;
};

// List of all entity PAK files to load
// Order doesn't matter - the EntityID determines the sprite index
constexpr EntityPakEntry ENTITY_PAK_LIST[] = {
	{ EntityID::DUMMY, "dummy.pak" },
	{ EntityID::SLIME, "slime.pak" },
	// Add more entities here as needed:
	// { EntityID::SKELETON, "skeleton.pak" },
};
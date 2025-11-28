#pragma once
#include "raylib_include.h"
#include <string>
#include <cstdint>
#include <stdexcept>
#include <optional>
#include <filesystem>
#include <fstream>
#include "json.hpp"

enum class EquipType : uint8_t
{
    Head,
    Back,
    Shirt,
    Chest,
    Legs,
    Feet,
    Weapon,
    Shield,
    Accessory,
    None = 255
};

struct SpriteItemIndices
{
    uint32_t pak_start_index;
    uint32_t pak_end_index;
    uint32_t equip_frame_index;
    uint32_t ground_frame_index;
    uint32_t pack_frame_index;
};

struct ItemMetadataEntry
{
    std::string pak_file;
    uint32_t id;
    EquipType equip_type;
    std::optional<SpriteItemIndices> male;
    std::optional<SpriteItemIndices> female;
};

// ------------------------------
// SpriteItemIndices parser
// ------------------------------
inline void from_json(const nlohmann::json& j, SpriteItemIndices& s)
{
    j.at("pak_index_start").get_to(s.pak_start_index);
    j.at("pak_index_end").get_to(s.pak_end_index);
    j.at("equip_frame_index").get_to(s.equip_frame_index);
    j.at("ground_frame_index").get_to(s.ground_frame_index);
    j.at("pack_frame_index").get_to(s.pack_frame_index);
}

// ------------------------------
// EquipType parser (case-insensitive)
// ------------------------------
inline void from_json(const nlohmann::json& j, EquipType& e)
{
    const std::string s = stdx::to_lower(j.get<std::string>());

    if (s == "head")       e = EquipType::Head;
    else if (s == "back")       e = EquipType::Back;
    else if (s == "shirt")      e = EquipType::Shirt;
    else if (s == "chest")      e = EquipType::Chest;
    else if (s == "legs")       e = EquipType::Legs;
    else if (s == "feet")       e = EquipType::Feet;
    else if (s == "weapon")     e = EquipType::Weapon;
    else if (s == "shield")     e = EquipType::Shield;
    else if (s == "accessory")  e = EquipType::Accessory;
    else if (s == "none")       e = EquipType::None;
    else
        throw std::runtime_error("Invalid equip_type enum: " + s);
}

// ------------------------------
// ItemMetadataEntry parser
// ------------------------------
inline void from_json(const nlohmann::json& j, ItemMetadataEntry& m)
{
    j.at("pak_file").get_to(m.pak_file);
    j.at("id").get_to(m.id);

    // EquipType is optional, defaults to None
    if (j.contains("equip_type") && !j["equip_type"].is_null())
        j.at("equip_type").get_to(m.equip_type);
    else
        m.equip_type = EquipType::None;

    // male (optional)
    if (j.contains("male") && !j["male"].is_null())
        m.male = j.at("male").get<SpriteItemIndices>();
    else
        m.male = std::nullopt;

    // female (optional)
    if (j.contains("female") && !j["female"].is_null())
        m.female = j.at("female").get<SpriteItemIndices>();
    else
        m.female = std::nullopt;

    // REQUIRE at least one of them
    if (!m.male.has_value() && !m.female.has_value())
    {
        throw std::runtime_error(
            "ItemMetadataEntry error: both 'male' and 'female' are missing or null for: "
            + m.pak_file);
    }
}

// ------------------------------
// Loaders
// ------------------------------
inline ItemMetadataEntry LoadMetadataEntry(const std::filesystem::path& path)
{
    std::ifstream f(path);
    if (!f.is_open())
        throw std::runtime_error("Failed to open metadata JSON: " + path.string());

    nlohmann::json j;
    f >> j;

    return j.get<ItemMetadataEntry>();
}

inline std::vector<ItemMetadataEntry> LoadMetadataEntries(const std::filesystem::path& path)
{
    std::ifstream f(path);
    if (!f.is_open())
        throw std::runtime_error("Failed to open metadata JSON: " + path.string());

    nlohmann::json j;
    f >> j;

    if (j.is_array())
        return j.get<std::vector<ItemMetadataEntry>>();

    return { j.get<ItemMetadataEntry>() };
}

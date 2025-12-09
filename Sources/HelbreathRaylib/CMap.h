#pragma once
#include "raylib_include.h"
#include "global_constants.h"
#include <functional>
#include <filesystem>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include "entity.h"
#include "EntityManager.h"

#pragma pack(push, 1)
struct MTDHeader {
    char magic[5];
    uint32_t crc;
    int16_t mapIdentifierLength;
    int16_t mapNameLength;
    int16_t mapSizeX;
    int16_t mapSizeY;
    int16_t tileSize;
    uint8_t padding[100];
};
#pragma pack(pop)

struct CTile {
    int16_t tile_sprite{};
    int16_t tile_sprite_frame{};
    int16_t object_sprite{};
    int16_t object_sprite_frame{};
    bool is_move_allowed{};
    bool is_teleport{};
};

inline std::pair<int, int> get_world_tile_mouse_position(raylib::Camera2D camera)
{
    raylib::Vector2 MposWorld = GetScreenToWorld2D(rlx::get_mouse_position(), camera);
    const int Tx = (int)floorf((MposWorld.x + constant::TILE_HALF) / constant::TILE_SIZE);
    const int Ty = (int)floorf((MposWorld.y + constant::TILE_HALF) / constant::TILE_SIZE);
    return { Tx, Ty };
}

class Entity;

class CMapData {
public:
    CMapData() = default;
    explicit CMapData(int16_t map_size_x, int16_t map_size_y, int16_t tile_size, const std::string& map_identifier, const std::string& map_name)
        : _map_size_x(map_size_x), _map_size_y(map_size_y), _tile_size(tile_size), _map_identifier(map_identifier), _map_name(map_name)
    {
        _tile.assign(_map_size_x, std::vector<CTile>(_map_size_y));
    }

    bool in_bounds(int16_t x, int16_t y) const
    {
        return x >= 0 && y >= 0 &&
            x < _map_size_x &&
            y < _map_size_y;
    }

    CTile* get_tile(int16_t x, int16_t y)
    {
        if (!in_bounds(x, y)) return nullptr;
        return &_tile[x][y];
    }

    const CTile* get_tile(int16_t x, int16_t y) const
    {
        if (!in_bounds(x, y)) return nullptr;
        return &_tile[x][y];
    }

    int width() const { return _map_size_x; }
    int height() const { return _map_size_y; }
    int tile_size() const { return _tile_size; }

    void for_each_tile_region(raylib::Camera2D camera, int x_range, int y_range, const EntityManager&,
        const std::function<void(int16_t, int16_t, int32_t, int32_t, CTile&, Entity*)>& fn);

    const std::string& get_map_identifier() const { return _map_identifier; }
    const std::string& get_map_name() const { return _map_name; }

    std::unique_ptr<std::unordered_set<uint64_t>> reserved_tiles = std::make_unique<std::unordered_set<uint64_t>>();

    // Each map owns its own EntityManager for entities on this map
    EntityManager& get_entity_manager() { return _entity_manager; }
    const EntityManager& get_entity_manager() const { return _entity_manager; }

private:
    int16_t _map_size_x{};
    int16_t _map_size_y{};
    int16_t _tile_size{};
    std::vector<std::vector<CTile>> _tile;
    std::string _map_identifier{};
    std::string _map_name{};
    EntityManager _entity_manager{}; // Each map has its own entity manager
};


class CMapLoader {
private:
    std::vector<std::filesystem::path> _map_files;

    CMapLoader() = default;

public:
    static CMapLoader& _get()
    {
        static CMapLoader instance;
        return instance;
    }

    CMapLoader(const CMapLoader&) = delete;
    CMapLoader& operator=(const CMapLoader&) = delete;

    static void register_map_file(uint8_t index, const std::filesystem::path& path)
    {
        auto& Self = _get();

        if (!std::filesystem::exists(path)) {
            std::printf("Map file not found: %s\n", path.string().c_str());
            return;
        }

        if (index >= Self._map_files.size())
            Self._map_files.resize(index + 1);

        Self._map_files[index] = path;
        std::printf("Registered map at index %u: %s\n", index, path.string().c_str());
    }

    static std::unique_ptr<CMapData> load_by_index(uint8_t index)
    {
        auto& Self = _get();

        if (index >= Self._map_files.size() || Self._map_files[index].empty()) {
            std::printf("No map registered at index %u\n", index);
            return nullptr;
        }

        return _load_mtd_file(Self._map_files[index]);
    }

    static size_t get_map_count()
    {
        return _get()._map_files.size();
    }

private:
    static std::unique_ptr<CMapData> _load_mtd_file(const std::filesystem::path& path)
    {
        if (!std::filesystem::exists(path)) {
            std::printf("Map not found: %s\n", path.string().c_str());
            return nullptr;
        }

        std::ifstream File(path, std::ios::binary);
        if (!File.is_open()) {
            std::printf("Failed to open %s\n", path.string().c_str());
            return nullptr;
        }

        std::vector<uint8_t> Data(std::istreambuf_iterator<char>(File), {});
        if (Data.size() < 5 + 4 + 2 + 2 + 2) {
            std::printf("Invalid file: %s\n", path.string().c_str());
            return nullptr;
        }

        const uint8_t* P = Data.data();
        if (std::memcmp(P, "<MTD>", 5) != 0) {
            std::printf("Invalid header in %s\n", path.string().c_str());
            return nullptr;
        }
        P += 5;

        uint32_t Crc = 0;
        std::memcpy(&Crc, P, 4);
        P += 4;

        int16_t IdLen = 0;
        std::memcpy(&IdLen, P, 2);
        P += 2;
        std::string MapIdentifier(reinterpret_cast<const char*>(P), IdLen);
        P += IdLen;

        int16_t NameLen = 0;
        std::memcpy(&NameLen, P, 2);
        P += 2;
        std::string MapName(reinterpret_cast<const char*>(P), NameLen);
        P += NameLen;

        int16_t MapSizeX = 0, MapSizeY = 0, TileSize = 0;
        std::memcpy(&MapSizeX, P, 2); P += 2;
        std::memcpy(&MapSizeY, P, 2); P += 2;
        std::memcpy(&TileSize, P, 2); P += 2;

        P += 100;

        if (MapSizeX <= 0 || MapSizeY <= 0) {
            std::printf("Bad dimensions in %s\n", path.string().c_str());
            return nullptr;
        }

        const int MapWidth = MapSizeX;
        const int MapHeight = MapSizeY;
        const int TotalTiles = MapWidth * MapHeight;
        const size_t TileBytes = static_cast<size_t>(TotalTiles) * 10u;

        if (P + 7 + TileBytes + 8 > Data.data() + Data.size()) {
            std::printf("Unexpected EOF in %s\n", path.string().c_str());
            return nullptr;
        }

        if (std::memcmp(P, "<TILES>", 7) != 0) {
            std::printf("Missing <TILES> in %s\n", path.string().c_str());
            return nullptr;
        }
        P += 7;

        const uint8_t* TileStart = P;
        uint64_t Sum = 0;
        for (size_t i = 0; i < TileBytes; ++i) Sum += static_cast<uint64_t>(TileStart[i] + i);
        const uint32_t ComputedCRC = static_cast<uint32_t>(Sum ^ Data.size());
        if (ComputedCRC != Crc) {
            std::printf("CRC mismatch in %s (expected %u, got %u)\n", path.string().c_str(), Crc, ComputedCRC);
            return nullptr;
        }

        auto MapData = std::make_unique<CMapData>(MapWidth, MapHeight, TileSize, MapIdentifier, MapName);
        const uint8_t* Tp = TileStart;
        for (int y = 0; y < MapHeight; ++y) {
            for (int x = 0; x < MapWidth; ++x) {
                if (CTile* T = MapData->get_tile(static_cast<int16_t>(x), static_cast<int16_t>(y))) {
                    std::memcpy(&T->tile_sprite, Tp, 2); Tp += 2;
                    std::memcpy(&T->tile_sprite_frame, Tp, 2); Tp += 2;
                    std::memcpy(&T->object_sprite, Tp, 2); Tp += 2;
                    std::memcpy(&T->object_sprite_frame, Tp, 2); Tp += 2;
                    T->is_move_allowed = (*Tp++) != 0;
                    T->is_teleport = (*Tp++) != 0;
                }
            }
        }

        std::printf("Loaded MTD: %s (name: %s) %dx%d tiles (%d total), tile=%d, CRC %u OK\n",
            MapIdentifier.c_str(), MapName.c_str(), MapWidth, MapHeight, TotalTiles, TileSize, Crc);

        return MapData;
    }
};
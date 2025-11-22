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
    int16_t m_sTileSprite{};
    int16_t m_sTileSpriteFrame{};
    int16_t m_sObjectSprite{};
    int16_t m_sObjectSpriteFrame{};
    bool m_bIsMoveAllowed{};
    bool m_bIsTeleport{};
};

inline std::pair<int, int> GetWorldTileMousePosition(Camera2D camera)
{
    Vector2 mposWorld = GetScreenToWorld2D(GetMousePosition(), camera);
    const int tx = (int)floorf((mposWorld.x + constant::TILE_HALF) / constant::TILE_SIZE);
    const int ty = (int)floorf((mposWorld.y + constant::TILE_HALF) / constant::TILE_SIZE);
    return { tx, ty };
}

class Entity;

class CMapData {
public:
    CMapData() = default;
    explicit CMapData(int16_t _mapSizeX, int16_t _mapSizeY, int16_t _tileSize, const std::string& mapIdentifier, const std::string& mapName)
        : mapSizeX(_mapSizeX), mapSizeY(_mapSizeY), tileSize(_tileSize), m_mapIdentifier(mapIdentifier), m_mapName(mapName)
    {
        m_tile.assign(mapSizeX, std::vector<CTile>(mapSizeY));
    }

    bool InBounds(int16_t x, int16_t y) const
    {
        return x >= 0 && y >= 0 &&
            x < mapSizeX &&
            y < mapSizeY;
    }

    CTile* GetTile(int16_t x, int16_t y)
    {
        if (!InBounds(x, y)) return nullptr;
        return &m_tile[x][y];
    }

    const CTile* GetTile(int16_t x, int16_t y) const
    {
        if (!InBounds(x, y)) return nullptr;
        return &m_tile[x][y];
    }

    int Width() const { return mapSizeX; }
    int Height() const { return mapSizeY; }
    int TileSize() const { return tileSize; }

    void ForEachTileRegion(Camera2D camera, int x_range, int y_range, const std::vector<std::unique_ptr<Entity>>&,
        const std::function<void(int16_t, int16_t, int32_t, int32_t, CTile&, Entity*)>& fn);

    const std::string& GetMapIdentifier() const { return m_mapIdentifier; }
    const std::string& GetMapName() const { return m_mapName; }

private:
    int16_t mapSizeX{};
    int16_t mapSizeY{};
    int16_t tileSize{};
    std::vector<std::vector<CTile>> m_tile;
    std::string m_mapIdentifier{};
    std::string m_mapName{};
};


class CMapLoader {
private:
    std::vector<std::filesystem::path> m_mapFiles;

    CMapLoader() = default;

public:
    static CMapLoader& Instance()
    {
        static CMapLoader instance;
        return instance;
    }

    CMapLoader(const CMapLoader&) = delete;
    CMapLoader& operator=(const CMapLoader&) = delete;

    static void RegisterMapFile(uint8_t index, const std::filesystem::path& path)
    {
        auto& self = Instance();

        if (!std::filesystem::exists(path)) {
            std::printf("Map file not found: %s\n", path.string().c_str());
            return;
        }

        if (index >= self.m_mapFiles.size())
            self.m_mapFiles.resize(index + 1);

        self.m_mapFiles[index] = path;
        std::printf("Registered map at index %u: %s\n", index, path.string().c_str());
    }

    static std::unique_ptr<CMapData> LoadByIndex(uint8_t index)
    {
        auto& self = Instance();

        if (index >= self.m_mapFiles.size() || self.m_mapFiles[index].empty()) {
            std::printf("No map registered at index %u\n", index);
            return nullptr;
        }

        return LoadMTDFile(self.m_mapFiles[index]);
    }

    static size_t GetMapCount()
    {
        return Instance().m_mapFiles.size();
    }

private:
    static std::unique_ptr<CMapData> LoadMTDFile(const std::filesystem::path& path)
    {
        if (!std::filesystem::exists(path)) {
            std::printf("Map not found: %s\n", path.string().c_str());
            return nullptr;
        }

        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            std::printf("Failed to open %s\n", path.string().c_str());
            return nullptr;
        }

        std::vector<uint8_t> data(std::istreambuf_iterator<char>(file), {});
        if (data.size() < 5 + 4 + 2 + 2 + 2) {
            std::printf("Invalid file: %s\n", path.string().c_str());
            return nullptr;
        }

        const uint8_t* p = data.data();
        if (std::memcmp(p, "<MTD>", 5) != 0) {
            std::printf("Invalid header in %s\n", path.string().c_str());
            return nullptr;
        }
        p += 5;

        uint32_t crc = 0;
        std::memcpy(&crc, p, 4);
        p += 4;

        int16_t idLen = 0;
        std::memcpy(&idLen, p, 2);
        p += 2;
        std::string mapIdentifier(reinterpret_cast<const char*>(p), idLen);
        p += idLen;

        int16_t nameLen = 0;
        std::memcpy(&nameLen, p, 2);
        p += 2;
        std::string mapName(reinterpret_cast<const char*>(p), nameLen);
        p += nameLen;

        int16_t mapSizeX = 0, mapSizeY = 0, tileSize = 0;
        std::memcpy(&mapSizeX, p, 2); p += 2;
        std::memcpy(&mapSizeY, p, 2); p += 2;
        std::memcpy(&tileSize, p, 2); p += 2;

        p += 100;

        if (mapSizeX <= 0 || mapSizeY <= 0) {
            std::printf("Bad dimensions in %s\n", path.string().c_str());
            return nullptr;
        }

        const int mapWidth = mapSizeX;
        const int mapHeight = mapSizeY;
        const int totalTiles = mapWidth * mapHeight;
        const size_t tileBytes = static_cast<size_t>(totalTiles) * 10u;

        if (p + 7 + tileBytes + 8 > data.data() + data.size()) {
            std::printf("Unexpected EOF in %s\n", path.string().c_str());
            return nullptr;
        }

        if (std::memcmp(p, "<TILES>", 7) != 0) {
            std::printf("Missing <TILES> in %s\n", path.string().c_str());
            return nullptr;
        }
        p += 7;

        const uint8_t* tileStart = p;
        uint64_t sum = 0;
        for (size_t i = 0; i < tileBytes; ++i) sum += static_cast<uint64_t>(tileStart[i] + i);
        const uint32_t computedCRC = static_cast<uint32_t>(sum ^ data.size());
        if (computedCRC != crc) {
            std::printf("CRC mismatch in %s (expected %u, got %u)\n", path.string().c_str(), crc, computedCRC);
            return nullptr;
        }

        auto mapData = std::make_unique<CMapData>(mapWidth, mapHeight, tileSize, mapIdentifier, mapName);
        const uint8_t* tp = tileStart;
        for (int y = 0; y < mapHeight; ++y) {
            for (int x = 0; x < mapWidth; ++x) {
                if (CTile* t = mapData->GetTile(static_cast<int16_t>(x), static_cast<int16_t>(y))) {
                    std::memcpy(&t->m_sTileSprite, tp, 2); tp += 2;
                    std::memcpy(&t->m_sTileSpriteFrame, tp, 2); tp += 2;
                    std::memcpy(&t->m_sObjectSprite, tp, 2); tp += 2;
                    std::memcpy(&t->m_sObjectSpriteFrame, tp, 2); tp += 2;
                    t->m_bIsMoveAllowed = (*tp++) != 0;
                    t->m_bIsTeleport = (*tp++) != 0;
                }
            }
        }

        std::printf("Loaded MTD: %s (name: %s) %dx%d tiles (%d total), tile=%d, CRC %u OK\n",
            mapIdentifier.c_str(), mapName.c_str(), mapWidth, mapHeight, totalTiles, tileSize, crc);

        return mapData;
    }
};
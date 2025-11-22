#include "CMap.h"
#include "entity.h"

void CMapData::ForEachTileRegion(Camera2D camera, int x_range, int y_range, const std::vector<std::unique_ptr<Entity>>& entities,
    const std::function<void(int16_t, int16_t, int32_t, int32_t, CTile&, Entity*)>& fn)
{
    const int screenTilesX = (int)((constant::BASE_WIDTH * camera.zoom) / constant::TILE_SIZE) + x_range;
    const int screenTilesY = (int)((constant::BASE_HEIGHT * camera.zoom) / constant::TILE_SIZE) + y_range;

    int centerX = (int)(camera.target.x / constant::TILE_SIZE);
    int centerY = (int)(camera.target.y / constant::TILE_SIZE);

    int radiusX = screenTilesX / 2;
    int radiusY = screenTilesY / 2;
    int radiusSquared = radiusX * radiusX; // Use circular radius

    int startX = std::max(0, centerX - radiusX);
    int startY = std::max(0, centerY - radiusY);
    int endX = std::min((int)mapSizeX, centerX + radiusX);
    int endY = std::min((int)mapSizeY, centerY + radiusY);

    for (int16_t y = startY; y < endY; ++y)
    {
        for (int16_t x = startX; x < endX; ++x)
        {
            // Circle culling
            int dx = x - centerX;
            int dy = y - centerY;
            if (dx * dx + dy * dy > radiusSquared)
                continue;

            int32_t pX = (x * constant::TILE_SIZE) - constant::TILE_HALF;
            int32_t pY = (y * constant::TILE_SIZE) - constant::TILE_HALF;

            if (!InBounds(x, y))
                continue;

            auto it = std::find_if(entities.begin(), entities.end(),
                [x, y](const std::unique_ptr<Entity>& e) {
                    auto pos = e->GetPosition();
                    return pos.get_tile_x() == x && pos.get_tile_y() == y;
				});

			Entity* entityPtr = (it != entities.end()) ? it->get() : nullptr;

            fn(x, y, pX, pY, m_tile[x][y], entityPtr);
        }
    }
}

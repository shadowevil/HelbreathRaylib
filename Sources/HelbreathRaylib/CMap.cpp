#include "CMap.h"
#include "entity.h"

void CMapData::for_each_tile_region(Camera2D camera, int x_range, int y_range, const std::vector<std::unique_ptr<Entity>>& entities,
    const std::function<void(int16_t, int16_t, int32_t, int32_t, CTile&, Entity*)>& fn)
{
    const int ScreenTilesX = (int)((constant::BASE_WIDTH * camera.zoom) / constant::TILE_SIZE) + x_range;
    const int ScreenTilesY = (int)((constant::BASE_HEIGHT * camera.zoom) / constant::TILE_SIZE) + y_range;

    int CenterX = (int)(camera.target.x / constant::TILE_SIZE);
    int CenterY = (int)(camera.target.y / constant::TILE_SIZE);

    int RadiusX = ScreenTilesX / 2;
    int RadiusY = ScreenTilesY / 2;
    int RadiusSquared = RadiusX * RadiusX; // Use circular radius

    int StartX = std::max(0, CenterX - RadiusX);
    int StartY = std::max(0, CenterY - RadiusY);
    int EndX = std::min((int)_map_size_x, CenterX + RadiusX);
    int EndY = std::min((int)_map_size_y, CenterY + RadiusY);

    for (int16_t y = StartY; y < EndY; ++y)
    {
        for (int16_t x = StartX; x < EndX; ++x)
        {
            // Circle culling
            int Dx = x - CenterX;
            int Dy = y - CenterY;
            if (Dx * Dx + Dy * Dy > RadiusSquared)
                continue;

            int32_t PX = (x * constant::TILE_SIZE) - constant::TILE_HALF;
            int32_t PY = (y * constant::TILE_SIZE) - constant::TILE_HALF;

            if (!in_bounds(x, y))
                continue;

            auto It = std::find_if(entities.begin(), entities.end(),
                [x, y](const std::unique_ptr<Entity>& e) {
                    auto Pos = e->get_position();
                    return Pos.get_tile_x() == x && Pos.get_tile_y() == y;
				});

			Entity* EntityPtr = (It != entities.end()) ? It->get() : nullptr;

            fn(x, y, PX, PY, _tile[x][y], EntityPtr);
        }
    }
}

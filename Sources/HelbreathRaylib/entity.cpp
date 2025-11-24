#include "Application.h"
#include "Helbreath.h"
#include "Game.h"
#include "entity.h"
#include "CMap.h"
#include "ItemMetadata.h"

Entity::Entity()
	:
	m_application(Helbreath::Instance()),
	m_game(m_application.GetPrimaryLayer<Game>()),
	m_modelSprites(m_game.m_modelSprites),
	m_itemMetadata(m_game.m_itemMetadata),
	m_entities(m_game.m_entities),
	m_activeMap(nullptr)
{ }

void Entity::Update()
{
	if (m_isMoving)
	{
		UpdateMovement();
	}

	if (m_attachedCamera)
	{
		m_attachedCamera->target = { (float)position.get_pixel_x(), (float)position.get_pixel_y() };
	}

	OnUpdate();
}

void Entity::SetActiveMap(CMapData* map) {
	m_activeMap = map;
	m_mapWidth = map->Width();
	m_mapHeight = map->Height();
	m_reservedTiles = map->g_reservedTiles.get();
}

void Entity::MoveTo(const GamePosition& target, bool run)
{
	if (position == target)
		return;
	if (m_finalTarget == target && m_isMoving)
		return;

	// Validate and adjust target if necessary
	GamePosition validTarget = FindValidTarget(target);

	m_finalTarget = validTarget;
	m_isRunning = run;
	m_stopRequested = false;

	GamePosition pathStart = m_isMoving ? m_currentStepTarget : position;
	BuildPath(m_nextMovementPath, pathStart, validTarget);

	if (!m_isMoving && !m_nextMovementPath.empty())
	{
		m_movementPath = std::move(m_nextMovementPath);
		m_nextMovementPath.clear();
		m_isMoving = true;
		m_currentPathIndex = 0;
		StartNextStep();
		if (current_animation_type == STAND)
		{
			if(current_animation_type != RUN && m_isRunning) {
				SetAnimation(RUN, WeaponUsed::HAND);
			} else if(current_animation_type != WALK) {
				SetAnimation(WALK, WeaponUsed::HAND);
			}
		}
	}
}

GamePosition Entity::FindValidTarget(const GamePosition& target)
{
	int tx = target.get_tile_x();
	int ty = target.get_tile_y();

	// Clamp to map bounds
	if (m_mapWidth > 0 && m_mapHeight > 0)
	{
		tx = std::max(0, std::min(tx, (int)m_mapWidth - 1));
		ty = std::max(0, std::min(ty, (int)m_mapHeight - 1));
	}

	// Check if target tile is valid
	if (m_activeMap->GetTile(tx, ty)->m_bIsMoveAllowed && !IsTileOccupied(tx, ty))
	{
		return GamePosition(tx, ty);
	}

	// Target invalid - find closest valid tile
	int offsets[8][2] = {
		{0, -1}, {1, 0}, {0, 1}, {-1, 0},
		{1, -1}, {1, 1}, {-1, 1}, {-1, -1}
	};

	// Spiral search outward
	for (int radius = 1; radius < 20; ++radius)
	{
		for (int dx = -radius; dx <= radius; ++dx)
		{
			for (int dy = -radius; dy <= radius; ++dy)
			{
				if (abs(dx) != radius && abs(dy) != radius)
					continue;

				int checkX = tx + dx;
				int checkY = ty + dy;

				if (m_mapWidth > 0 && m_mapHeight > 0)
				{
					if (checkX < 0 || checkX >= (int)m_mapWidth ||
						checkY < 0 || checkY >= (int)m_mapHeight)
						continue;
				}

				if (!m_activeMap->GetTile(checkX, checkY)->m_bIsMoveAllowed)
					continue;

				if (IsTileOccupied(checkX, checkY))
					continue;

				// Check if this tile has at least one walkable neighbor
				bool hasWalkableNeighbor = false;
				for (int i = 0; i < 8; ++i)
				{
					int neighborX = checkX + offsets[i][0];
					int neighborY = checkY + offsets[i][1];

					if (m_mapWidth > 0 && m_mapHeight > 0)
					{
						if (neighborX < 0 || neighborX >= (int)m_mapWidth ||
							neighborY < 0 || neighborY >= (int)m_mapHeight)
							continue;
					}

					if (m_activeMap->GetTile(neighborX, neighborY)->m_bIsMoveAllowed)
					{
						hasWalkableNeighbor = true;
						break;
					}
				}

				if (hasWalkableNeighbor)
				{
					return GamePosition(checkX, checkY);
				}
			}
		}
	}

	// Fallback to current position if no valid tile found
	return position;
}

bool Entity::IsTileOccupied(int tile_x, int tile_y) const
{
	uint64_t key = MakeTileKey(tile_x, tile_y);

	if (m_reservedTiles && m_reservedTiles->count(key) > 0)
		return true;

	for (const auto& entity : m_entities)
	{
		if (entity.get() == this) continue;

		const auto& entity_pos = entity->GetPosition();
		if (entity_pos.get_tile_x() == tile_x && entity_pos.get_tile_y() == tile_y)
			return true;
	}
	return false;
}

void Entity::BuildPath(std::vector<GamePosition>& path, const GamePosition& start, const GamePosition& end)
{
	path.clear();

	int startX = start.get_tile_x();
	int startY = start.get_tile_y();
	int endX = end.get_tile_x();
	int endY = end.get_tile_y();

	if (m_mapWidth > 0 && m_mapHeight > 0)
	{
		endX = std::max(0, std::min(endX, (int)m_mapWidth - 1));
		endY = std::max(0, std::min(endY, (int)m_mapHeight - 1));
	}

	std::vector<GamePosition> directPath;
	bool directSuccess = BuildPathDirect(directPath, start, GamePosition(endX, endY));

	if (directSuccess)
	{
		path = directPath;
		return;
	}

	if (directPath.empty())
	{
		int offsets[8][2] = {
			{0, -1}, {1, 0}, {0, 1}, {-1, 0},
			{1, -1}, {1, 1}, {-1, 1}, {-1, -1}
		};

		std::vector<GamePosition> bestPath;

		for (int i = 0; i < 8; ++i)
		{
			int neighborX = startX + offsets[i][0];
			int neighborY = startY + offsets[i][1];

			if (m_mapWidth > 0 && m_mapHeight > 0)
			{
				if (neighborX < 0 || neighborX >= (int)m_mapWidth ||
					neighborY < 0 || neighborY >= (int)m_mapHeight)
					continue;
			}

			if (!m_activeMap->GetTile(neighborX, neighborY)->m_bIsMoveAllowed)
				continue;

			if (IsTileOccupied(neighborX, neighborY))
				continue;

			std::vector<GamePosition> attemptPath;
			attemptPath.emplace_back(neighborX, neighborY);

			std::vector<GamePosition> remainingPath;
			bool success = BuildPathDirect(remainingPath, GamePosition(neighborX, neighborY), GamePosition(endX, endY));

			attemptPath.insert(attemptPath.end(), remainingPath.begin(), remainingPath.end());

			if (success && attemptPath.size() > bestPath.size())
			{
				bestPath = attemptPath;
			}
			else if (!success && attemptPath.size() > bestPath.size())
			{
				bestPath = attemptPath;
			}
		}

		path = bestPath;
		return;
	}

	int offsets[8][2] = {
		{0, -1}, {1, 0}, {0, 1}, {-1, 0},
		{1, -1}, {1, 1}, {-1, 1}, {-1, -1}
	};

	std::vector<GamePosition> bestPath = directPath;

	for (int attempt = 0; attempt < 8; ++attempt)
	{
		int targetX = endX + offsets[attempt][0];
		int targetY = endY + offsets[attempt][1];

		if (m_mapWidth > 0 && m_mapHeight > 0)
		{
			if (targetX < 0 || targetX >= (int)m_mapWidth ||
				targetY < 0 || targetY >= (int)m_mapHeight)
				continue;
		}

		if (!m_activeMap->GetTile(targetX, targetY)->m_bIsMoveAllowed)
			continue;

		if (IsTileOccupied(targetX, targetY))
			continue;

		std::vector<GamePosition> attemptPath;
		bool success = BuildPathDirect(attemptPath, start, GamePosition(targetX, targetY));

		if (success && attemptPath.size() > bestPath.size())
		{
			bestPath = attemptPath;
		}
		else if (!success && attemptPath.size() > bestPath.size())
		{
			bestPath = attemptPath;
		}
	}

	path = bestPath;
}

bool Entity::BuildPathDirect(std::vector<GamePosition>& path, const GamePosition& start, const GamePosition& end)
{
	path.clear();

	int startX = start.get_tile_x();
	int startY = start.get_tile_y();
	int endX = end.get_tile_x();
	int endY = end.get_tile_y();

	int dx = endX - startX;
	int dy = endY - startY;

	int stepX = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
	int stepY = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;

	int absX = std::abs(dx);
	int absY = std::abs(dy);

	bool reachedEnd = false;

	if (absX > absY)
	{
		int error = absX / 2;
		int currentX = startX;
		int currentY = startY;

		for (int i = 0; i < absX; ++i)
		{
			currentX += stepX;
			error -= absY;

			if (error < 0)
			{
				currentY += stepY;
				error += absX;
			}

			if (!m_activeMap->GetTile(currentX, currentY)->m_bIsMoveAllowed ||
				IsTileOccupied(currentX, currentY))
			{
				break;
			}

			path.emplace_back(currentX, currentY);

			if (currentX == endX && currentY == endY)
			{
				reachedEnd = true;
				break;
			}
		}
	}
	else
	{
		int error = absY / 2;
		int currentX = startX;
		int currentY = startY;

		for (int i = 0; i < absY; ++i)
		{
			currentY += stepY;
			error -= absX;

			if (error < 0)
			{
				currentX += stepX;
				error += absY;
			}

			if (!m_activeMap->GetTile(currentX, currentY)->m_bIsMoveAllowed ||
				IsTileOccupied(currentX, currentY))
			{
				break;
			}

			path.emplace_back(currentX, currentY);

			if (currentX == endX && currentY == endY)
			{
				reachedEnd = true;
				break;
			}
		}
	}

	return reachedEnd;
}

void Entity::StartNextStep()
{
	if (m_currentPathIndex >= m_movementPath.size())
	{
		m_isMoving = false;
		SetAnimation(STAND, WeaponUsed::HAND);
		return;
	}

	m_currentStepTarget = m_movementPath[m_currentPathIndex];

	uint64_t key = MakeTileKey(m_currentStepTarget.get_tile_x(), m_currentStepTarget.get_tile_y());

	if (IsTileOccupied(m_currentStepTarget.get_tile_x(), m_currentStepTarget.get_tile_y()))
	{
		StopMovement();
		return;
	}

	if (m_reservedTiles)
	{
		m_reservedTiles->insert(key);
		m_reservedTileKey = key;
	}

	m_moveStartPixelX = position.get_pixel_x();
	m_moveStartPixelY = position.get_pixel_y();
	m_moveProgress = 0.0f;
	current_direction = GetDirectionToPoint(position, m_currentStepTarget);

	int dx = m_currentStepTarget.get_pixel_x() - position.get_pixel_x();
	int dy = m_currentStepTarget.get_pixel_y() - position.get_pixel_y();
	m_currentStepDistance = sqrtf((float)(dx * dx + dy * dy));
}

void Entity::UpdateMovement()
{
	float speedMultiplier = m_isRunning ? 1.00f : 0.5f;
	float effectiveSpeed = m_baseSpeed * m_internalSpeedMultiplier * speedMultiplier;

	float deltaProgress = (effectiveSpeed * GetFrameTime()) / m_currentStepDistance;
	m_moveProgress += deltaProgress;

	if (m_moveProgress >= 1.0f)
	{
		m_moveProgress = 0.0f;
		position = m_currentStepTarget;

		if (m_reservedTiles && m_reservedTileKey != 0)
		{
			m_reservedTiles->erase(m_reservedTileKey);
			m_reservedTileKey = 0;
		}

		m_currentPathIndex++;

		// Check for stop request first
		if (m_stopRequested)
		{
			m_movementPath.clear();
			m_nextMovementPath.clear();
			m_isMoving = false;
			m_stopRequested = false;
			SetAnimation(STAND, WeaponUsed::HAND);
			return;
		}

		// Check for queued path
		if (!m_nextMovementPath.empty())
		{
			m_movementPath = std::move(m_nextMovementPath);
			m_nextMovementPath.clear();
			m_currentPathIndex = 0;
			m_isMoving = true;
			StartNextStep();
			return;
		}

		// No queued path - continue current path or recalculate to final target
		if (m_currentPathIndex >= m_movementPath.size())
		{
			if (position != m_finalTarget)
			{
				BuildPath(m_movementPath, position, m_finalTarget);
				m_currentPathIndex = 0;

				if (!m_movementPath.empty())
				{
					StartNextStep();
				}
				else
				{
					m_isMoving = false;
					SetAnimation(STAND, WeaponUsed::HAND);
				}
			}
			else
			{
				m_isMoving = false;
				m_movementPath.clear();
				SetAnimation(STAND, WeaponUsed::HAND);
			}
		}
		else
		{
			StartNextStep();
		}
	}
	else
	{
		int32_t newPixelX = int32_t(m_moveStartPixelX + (((int32_t)m_currentStepTarget.get_pixel_x()) - m_moveStartPixelX) * m_moveProgress);
		int32_t newPixelY = int32_t(m_moveStartPixelY + (((int32_t)m_currentStepTarget.get_pixel_y()) - m_moveStartPixelY) * m_moveProgress);

		position.set_pixel_position(newPixelX, newPixelY);
	}
}

void Entity::RequestStopMovement()
{
	m_stopRequested = true;
}

void Entity::StopMovement()
{
	if (m_reservedTiles && m_reservedTileKey != 0)
	{
		m_reservedTiles->erase(m_reservedTileKey);
		m_reservedTileKey = 0;
	}

	m_movementPath.clear();
	m_nextMovementPath.clear();
	m_isMoving = false;
	m_stopRequested = false;
	SetAnimation(STAND, WeaponUsed::HAND);
}

void Entity::RenderDebugMovement()
{
	if (m_movementPath.empty())
		return;

	for (size_t i = 0; i < m_movementPath.size(); ++i)
	{
		const auto& step = m_movementPath[i];
		int px = step.get_pixel_x();
		int py = step.get_pixel_y();

		Color tileColor = (i < m_currentPathIndex)
			? Color{ 100, 100, 100, 100 }
		: Color{ 0, 255, 0, 100 };

		DrawRectangle(px - constant::TILE_HALF, py - constant::TILE_HALF, constant::TILE_SIZE, constant::TILE_SIZE, tileColor);

		if (i == m_currentPathIndex)
		{
			DrawRectangleLinesEx(
				rlRectangle{ (float)px - constant::TILE_HALF,
				  (float)py - constant::TILE_HALF,
				  constant::TILE_SIZE,
				  constant::TILE_SIZE },
				1, YELLOW);
		}
	}

	if (m_finalTarget.get_tile_x() > 0 || m_finalTarget.get_tile_y() > 0)
	{
		int fx = m_finalTarget.get_pixel_x();
		int fy = m_finalTarget.get_pixel_y();

		DrawRectangleLinesEx(
			rlRectangle{ (float)fx - constant::TILE_HALF,
			  (float)fy - constant::TILE_HALF,
			  constant::TILE_SIZE,
			  constant::TILE_SIZE },
			1, RED);
	}
}
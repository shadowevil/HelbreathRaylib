#include "Application.h"
#include "Game.h"
#include "entity.h"
#include "CMap.h"
#include "ItemMetadata.h"

Entity::Entity()
	: _game(Application::get_layer<Game>()),
	_model_sprites(_game.model_sprites),
	_item_metadata(_game.item_metadata),
	_entities(_game.entities),
	_active_map(nullptr)
{ }

void Entity::update()
{
	if (_is_moving)
	{
		_update_movement();
	}

	if (_attached_camera)
	{
		_attached_camera->target = { (float)position.get_pixel_x(), (float)position.get_pixel_y() };
	}

	on_update();
}

void Entity::set_active_map(CMapData* map) {
	_active_map = map;
	_map_width = map->width();
	_map_height = map->height();
	_reserved_tiles = map->reserved_tiles.get();
}

void Entity::move_to(const GamePosition& target, bool run)
{
	if (position == target)
		return;
	if (_final_target == target && _is_moving)
		return;

	// Validate and adjust target if necessary
	GamePosition ValidTarget = find_valid_target(target);

	_final_target = ValidTarget;
	_is_running = run;
	_stop_requested = false;

	GamePosition PathStart = _is_moving ? _current_step_target : position;
	_build_path(_next_movement_path, PathStart, ValidTarget);

	if (!_is_moving && !_next_movement_path.empty())
	{
		_movement_path = std::move(_next_movement_path);
		_next_movement_path.clear();
		_is_moving = true;
		_current_path_index = 0;
		_start_next_step();
		if (current_animation_type == STAND)
		{
			if(current_animation_type != RUN && _is_running) {
				set_animation(RUN, WeaponUsed::HAND);
			} else if(current_animation_type != WALK) {
				set_animation(WALK, WeaponUsed::HAND);
			}
		}
	}
}

GamePosition Entity::find_valid_target(const GamePosition& target)
{
	int Tx = target.get_tile_x();
	int Ty = target.get_tile_y();

	// Clamp to map bounds
	if (_map_width > 0 && _map_height > 0)
	{
		Tx = std::max(0, std::min(Tx, (int)_map_width - 1));
		Ty = std::max(0, std::min(Ty, (int)_map_height - 1));
	}

	// Check if target tile is valid
	if (_active_map->get_tile(Tx, Ty)->is_move_allowed && !is_tile_occupied(Tx, Ty))
	{
		return GamePosition(Tx, Ty);
	}

	// Target invalid - find closest valid tile
	int Offsets[8][2] = {
		{0, -1}, {1, 0}, {0, 1}, {-1, 0},
		{1, -1}, {1, 1}, {-1, 1}, {-1, -1}
	};

	// Spiral search outward
	for (int Radius = 1; Radius < 20; ++Radius)
	{
		for (int Dx = -Radius; Dx <= Radius; ++Dx)
		{
			for (int Dy = -Radius; Dy <= Radius; ++Dy)
			{
				if (abs(Dx) != Radius && abs(Dy) != Radius)
					continue;

				int CheckX = Tx + Dx;
				int CheckY = Ty + Dy;

				if (_map_width > 0 && _map_height > 0)
				{
					if (CheckX < 0 || CheckX >= (int)_map_width ||
						CheckY < 0 || CheckY >= (int)_map_height)
						continue;
				}

				if (!_active_map->get_tile(CheckX, CheckY)->is_move_allowed)
					continue;

				if (is_tile_occupied(CheckX, CheckY))
					continue;

				// Check if this tile has at least one walkable neighbor
				bool HasWalkableNeighbor = false;
				for (int I = 0; I < 8; ++I)
				{
					int NeighborX = CheckX + Offsets[I][0];
					int NeighborY = CheckY + Offsets[I][1];

					if (_map_width > 0 && _map_height > 0)
					{
						if (NeighborX < 0 || NeighborX >= (int)_map_width ||
							NeighborY < 0 || NeighborY >= (int)_map_height)
							continue;
					}

					if (_active_map->get_tile(NeighborX, NeighborY)->is_move_allowed)
					{
						HasWalkableNeighbor = true;
						break;
					}
				}

				if (HasWalkableNeighbor)
				{
					return GamePosition(CheckX, CheckY);
				}
			}
		}
	}

	// Fallback to current position if no valid tile found
	return position;
}

bool Entity::is_tile_occupied(int tile_x, int tile_y) const
{
	uint64_t Key = _make_tile_key(tile_x, tile_y);

	if (_reserved_tiles && _reserved_tiles->count(Key) > 0)
		return true;

	for (const auto& Entity : _entities)
	{
		if (Entity.get() == this) continue;

		const auto& EntityPos = Entity->get_position();
		if (EntityPos.get_tile_x() == tile_x && EntityPos.get_tile_y() == tile_y)
			return true;
	}
	return false;
}

void Entity::_build_path(std::vector<GamePosition>& path, const GamePosition& start, const GamePosition& end)
{
	path.clear();

	int StartX = start.get_tile_x();
	int StartY = start.get_tile_y();
	int EndX = end.get_tile_x();
	int EndY = end.get_tile_y();

	if (_map_width > 0 && _map_height > 0)
	{
		EndX = std::max(0, std::min(EndX, (int)_map_width - 1));
		EndY = std::max(0, std::min(EndY, (int)_map_height - 1));
	}

	std::vector<GamePosition> DirectPath;
	bool DirectSuccess = _build_path_direct(DirectPath, start, GamePosition(EndX, EndY));

	if (DirectSuccess)
	{
		path = DirectPath;
		return;
	}

	if (DirectPath.empty())
	{
		int Offsets[8][2] = {
			{0, -1}, {1, 0}, {0, 1}, {-1, 0},
			{1, -1}, {1, 1}, {-1, 1}, {-1, -1}
		};

		std::vector<GamePosition> BestPath;

		for (int I = 0; I < 8; ++I)
		{
			int NeighborX = StartX + Offsets[I][0];
			int NeighborY = StartY + Offsets[I][1];

			if (_map_width > 0 && _map_height > 0)
			{
				if (NeighborX < 0 || NeighborX >= (int)_map_width ||
					NeighborY < 0 || NeighborY >= (int)_map_height)
					continue;
			}

			if (!_active_map->get_tile(NeighborX, NeighborY)->is_move_allowed)
				continue;

			if (is_tile_occupied(NeighborX, NeighborY))
				continue;

			std::vector<GamePosition> AttemptPath;
			AttemptPath.emplace_back(NeighborX, NeighborY);

			std::vector<GamePosition> RemainingPath;
			bool Success = _build_path_direct(RemainingPath, GamePosition(NeighborX, NeighborY), GamePosition(EndX, EndY));

			AttemptPath.insert(AttemptPath.end(), RemainingPath.begin(), RemainingPath.end());

			if (Success && AttemptPath.size() > BestPath.size())
			{
				BestPath = AttemptPath;
			}
			else if (!Success && AttemptPath.size() > BestPath.size())
			{
				BestPath = AttemptPath;
			}
		}

		path = BestPath;
		return;
	}

	int Offsets[8][2] = {
		{0, -1}, {1, 0}, {0, 1}, {-1, 0},
		{1, -1}, {1, 1}, {-1, 1}, {-1, -1}
	};

	std::vector<GamePosition> BestPath = DirectPath;

	for (int Attempt = 0; Attempt < 8; ++Attempt)
	{
		int TargetX = EndX + Offsets[Attempt][0];
		int TargetY = EndY + Offsets[Attempt][1];

		if (_map_width > 0 && _map_height > 0)
		{
			if (TargetX < 0 || TargetX >= (int)_map_width ||
				TargetY < 0 || TargetY >= (int)_map_height)
				continue;
		}

		if (!_active_map->get_tile(TargetX, TargetY)->is_move_allowed)
			continue;

		if (is_tile_occupied(TargetX, TargetY))
			continue;

		std::vector<GamePosition> AttemptPath;
		bool Success = _build_path_direct(AttemptPath, start, GamePosition(TargetX, TargetY));

		if (Success && AttemptPath.size() > BestPath.size())
		{
			BestPath = AttemptPath;
		}
		else if (!Success && AttemptPath.size() > BestPath.size())
		{
			BestPath = AttemptPath;
		}
	}

	path = BestPath;
}

bool Entity::_build_path_direct(std::vector<GamePosition>& path, const GamePosition& start, const GamePosition& end)
{
	path.clear();

	int StartX = start.get_tile_x();
	int StartY = start.get_tile_y();
	int EndX = end.get_tile_x();
	int EndY = end.get_tile_y();

	int Dx = EndX - StartX;
	int Dy = EndY - StartY;

	int StepX = (Dx > 0) ? 1 : (Dx < 0) ? -1 : 0;
	int StepY = (Dy > 0) ? 1 : (Dy < 0) ? -1 : 0;

	int AbsX = std::abs(Dx);
	int AbsY = std::abs(Dy);

	bool ReachedEnd = false;

	if (AbsX > AbsY)
	{
		int Error = AbsX / 2;
		int CurrentX = StartX;
		int CurrentY = StartY;

		for (int I = 0; I < AbsX; ++I)
		{
			CurrentX += StepX;
			Error -= AbsY;

			if (Error < 0)
			{
				CurrentY += StepY;
				Error += AbsX;
			}

			if (!_active_map->get_tile(CurrentX, CurrentY)->is_move_allowed ||
				is_tile_occupied(CurrentX, CurrentY))
			{
				break;
			}

			path.emplace_back(CurrentX, CurrentY);

			if (CurrentX == EndX && CurrentY == EndY)
			{
				ReachedEnd = true;
				break;
			}
		}
	}
	else
	{
		int Error = AbsY / 2;
		int CurrentX = StartX;
		int CurrentY = StartY;

		for (int I = 0; I < AbsY; ++I)
		{
			CurrentY += StepY;
			Error -= AbsX;

			if (Error < 0)
			{
				CurrentX += StepX;
				Error += AbsY;
			}

			if (!_active_map->get_tile(CurrentX, CurrentY)->is_move_allowed ||
				is_tile_occupied(CurrentX, CurrentY))
			{
				break;
			}

			path.emplace_back(CurrentX, CurrentY);

			if (CurrentX == EndX && CurrentY == EndY)
			{
				ReachedEnd = true;
				break;
			}
		}
	}

	return ReachedEnd;
}

void Entity::_start_next_step()
{
	if (_current_path_index >= _movement_path.size())
	{
		_is_moving = false;
		set_animation(STAND, WeaponUsed::HAND);
		return;
	}

	_current_step_target = _movement_path[_current_path_index];

	uint64_t Key = _make_tile_key(_current_step_target.get_tile_x(), _current_step_target.get_tile_y());

	if (is_tile_occupied(_current_step_target.get_tile_x(), _current_step_target.get_tile_y()))
	{
		stop_movement();
		return;
	}

	if (_reserved_tiles)
	{
		_reserved_tiles->insert(Key);
		_reserved_tile_key = Key;
	}

	_move_start_pixel_x = position.get_pixel_x();
	_move_start_pixel_y = position.get_pixel_y();
	_move_progress = 0.0f;
	current_direction = get_direction_to_point(position, _current_step_target);

	int Dx = _current_step_target.get_pixel_x() - position.get_pixel_x();
	int Dy = _current_step_target.get_pixel_y() - position.get_pixel_y();
	_current_step_distance = sqrtf((float)(Dx * Dx + Dy * Dy));
}

void Entity::_update_movement()
{
	float SpeedMultiplier = _is_running ? 1.00f : 0.5f;
	float EffectiveSpeed = _base_speed * _internal_speed_multiplier * SpeedMultiplier;

	float DeltaProgress = (EffectiveSpeed * GetFrameTime()) / _current_step_distance;
	_move_progress += DeltaProgress;

	if (_move_progress >= 1.0f)
	{
		_move_progress = 0.0f;
		position = _current_step_target;

		if (_reserved_tiles && _reserved_tile_key != 0)
		{
			_reserved_tiles->erase(_reserved_tile_key);
			_reserved_tile_key = 0;
		}

		_current_path_index++;

		// Check for stop request first
		if (_stop_requested)
		{
			_movement_path.clear();
			_next_movement_path.clear();
			_is_moving = false;
			_stop_requested = false;
			set_animation(STAND, WeaponUsed::HAND);
			return;
		}

		// Check for queued path
		if (!_next_movement_path.empty())
		{
			_movement_path = std::move(_next_movement_path);
			_next_movement_path.clear();
			_current_path_index = 0;
			_is_moving = true;
			_start_next_step();
			return;
		}

		// No queued path - continue current path or recalculate to final target
		if (_current_path_index >= _movement_path.size())
		{
			if (position != _final_target)
			{
				_build_path(_movement_path, position, _final_target);
				_current_path_index = 0;

				if (!_movement_path.empty())
				{
					_start_next_step();
				}
				else
				{
					_is_moving = false;
					set_animation(STAND, WeaponUsed::HAND);
				}
			}
			else
			{
				_is_moving = false;
				_movement_path.clear();
				set_animation(STAND, WeaponUsed::HAND);
			}
		}
		else
		{
			_start_next_step();
		}
	}
	else
	{
		int32_t NewPixelX = int32_t(_move_start_pixel_x + (((int32_t)_current_step_target.get_pixel_x()) - _move_start_pixel_x) * _move_progress);
		int32_t NewPixelY = int32_t(_move_start_pixel_y + (((int32_t)_current_step_target.get_pixel_y()) - _move_start_pixel_y) * _move_progress);

		position.set_pixel_position(NewPixelX, NewPixelY);
	}
}

void Entity::_request_stop_movement()
{
	_stop_requested = true;
}

void Entity::stop_movement()
{
	if (_reserved_tiles && _reserved_tile_key != 0)
	{
		_reserved_tiles->erase(_reserved_tile_key);
		_reserved_tile_key = 0;
	}

	_movement_path.clear();
	_next_movement_path.clear();
	_is_moving = false;
	_stop_requested = false;
	set_animation(STAND, WeaponUsed::HAND);
}

void Entity::render_debug_movement()
{
	if (_movement_path.empty())
		return;

	for (size_t I = 0; I < _movement_path.size(); ++I)
	{
		const auto& Step = _movement_path[I];
		int Px = Step.get_pixel_x();
		int Py = Step.get_pixel_y();

		Color TileColor = (I < _current_path_index)
			? Color{ 100, 100, 100, 100 }
		: Color{ 0, 255, 0, 100 };

		DrawRectangle(Px - constant::TILE_HALF, Py - constant::TILE_HALF, constant::TILE_SIZE, constant::TILE_SIZE, TileColor);

		if (I == _current_path_index)
		{
			DrawRectangleLinesEx(
				raylib::Rectangle{ (float)Px - constant::TILE_HALF,
				  (float)Py - constant::TILE_HALF,
				  constant::TILE_SIZE,
				  constant::TILE_SIZE },
				1, YELLOW);
		}
	}

	if (_final_target.get_tile_x() > 0 || _final_target.get_tile_y() > 0)
	{
		int Fx = _final_target.get_pixel_x();
		int Fy = _final_target.get_pixel_y();

		DrawRectangleLinesEx(
			raylib::Rectangle{ (float)Fx - constant::TILE_HALF,
			  (float)Fy - constant::TILE_HALF,
			  constant::TILE_SIZE,
			  constant::TILE_SIZE },
			1, RED);
	}
}
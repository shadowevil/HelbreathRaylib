#include "Application.h"
#include "Game.h"
#include "entity.h"
#include "CMap.h"
#include "ItemMetadata.h"
#include "FontSystem.h"
#include <format>

Entity::Entity()
	: _game(Application::get_layer<Game>()),
	_model_sprites(_game.model_sprites),
	_item_metadata(_game.item_metadata),
	_entity_sprites(_game.entity_sprites),
	_active_map(nullptr),
	_stats(std::make_unique<EntityStats>())
{ }

void Entity::update()
{
	// Skip most updates if marked for removal
	if (_should_remove)
		return;

	// Handle death state updates (fade-out, etc.)
	if (_is_dying || _is_dead)
	{
		_update_death();
		// Update damage indicators even while dying
		update_damage_indicators(raylib::GetFrameTime());
		on_update();
		return;
	}

	if (_is_moving)
	{
		on_before_movement_update();
		_update_movement();
	}

	if (_attached_camera)
	{
		_attached_camera->target = { (float)position.get_pixel_x(), (float)position.get_pixel_y() };
	}

	// Update damage indicators
	update_damage_indicators(raylib::GetFrameTime());

	on_update();
}

void Entity::set_active_map(CMapData* map) {
	_active_map = map;
	_map_width = map->width();
	_map_height = map->height();
	_reserved_tiles = map->reserved_tiles.get();
}

std::string Entity::get_active_map_identifier() const
{
	return _active_map ? _active_map->get_map_identifier() : "unknown";
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

void Entity::dash_to(const GamePosition& target)
{
	// Dash is a fast single-tile movement - no pathfinding
	if (position == target)
		return;

	// Can't dash while already moving
	if (_is_moving)
		return;

	// Simple single-step path
	_movement_path.clear();
	_movement_path.push_back(target);
	_next_movement_path.clear();

	_final_target = target;
	_is_running = false;
	_is_dashing = true;  // Enable dash speed
	_stop_requested = false;

	_is_moving = true;
	_current_path_index = 0;
	_start_next_step();

	// Dash uses run animation but at much faster speed
	set_animation(RUN, WeaponUsed::HAND);
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

	if (!_active_map)
		return false;

	for (const auto& entity_ptr : _active_map->get_entity_manager())
	{
		if (entity_ptr.get() == this) continue;

		const auto& EntityPos = entity_ptr->get_position();
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
	// Handle tile pause (entity requested delay before next step)
	if (_is_paused_at_tile)
	{
		_tile_pause_remaining -= raylib::GetFrameTime();
		if (_tile_pause_remaining <= 0.0f)
		{
			_is_paused_at_tile = false;
			_tile_pause_remaining = 0.0f;
			// Resume movement - start the next step
			_start_next_step();
		}
		return;
	}

	// Movement time synced with animation cycles:
	// Walk: 1 cycle * 8 frames * 0.085s = 0.68s per tile
	// Run:  1 cycle * 8 frames * 0.047s = 0.376s per tile
	// Dash: ~0.25s per tile (fast jump movement, synced with 4-frame dash animation)
	// Moving to any adjacent tile (cardinal or diagonal) takes one full animation cycle
	float TileTime = _is_dashing ? 0.25f : (_is_running ? 0.376f : 0.68f);

	float DeltaProgress = raylib::GetFrameTime() / TileTime;
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

		// Call the tile reached hook - entity can set pause or do other custom behavior
		on_tile_reached();

		// Check for stop request first
		if (_stop_requested)
		{
			_movement_path.clear();
			_next_movement_path.clear();
			_is_moving = false;
			_stop_requested = false;
			_is_paused_at_tile = false;
			set_animation(STAND, WeaponUsed::HAND);
			return;
		}

		// If entity requested a pause, wait before continuing
		if (_is_paused_at_tile)
		{
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
			// Check if this was a dash - call completion callback
			if (_is_dashing)
			{
				_is_dashing = false;
				_is_moving = false;
				_movement_path.clear();
				// Don't set STAND animation - let on_dash_complete handle it
				on_dash_complete();
				return;
			}

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
	// Always draw mouse hovered tile outline when camera is attached
	if (_attached_camera)
	{
		GamePosition hover_tile = get_tile_world_mouse_position(*_attached_camera);
		int hover_px = hover_tile.get_tile_x() * constant::TILE_SIZE;
		int hover_py = hover_tile.get_tile_y() * constant::TILE_SIZE;
		raylib::DrawRectangleLinesEx(
			raylib::Rectangle{ (float)hover_px - constant::TILE_HALF,
			  (float)hover_py - constant::TILE_HALF,
			  constant::TILE_SIZE,
			  constant::TILE_SIZE },
			1, raylib::BLUE);
	}

	if (_movement_path.empty())
		return;

	for (size_t I = 0; I < _movement_path.size(); ++I)
	{
		const auto& Step = _movement_path[I];
		int Px = Step.get_pixel_x();
		int Py = Step.get_pixel_y();

		raylib::Color TileColor = (I < _current_path_index)
			? raylib::Color{ 100, 100, 100, 100 }
		: raylib::Color{ 0, 255, 0, 100 };

		raylib::DrawRectangle(Px - constant::TILE_HALF, Py - constant::TILE_HALF, constant::TILE_SIZE, constant::TILE_SIZE, TileColor);

		if (I == _current_path_index)
		{
			raylib::DrawRectangleLinesEx(
				raylib::Rectangle{ (float)Px - constant::TILE_HALF,
				  (float)Py - constant::TILE_HALF,
				  constant::TILE_SIZE,
				  constant::TILE_SIZE },
				1, raylib::YELLOW);
		}
	}

	if (_final_target.get_tile_x() > 0 || _final_target.get_tile_y() > 0)
	{
		int Fx = _final_target.get_pixel_x();
		int Fy = _final_target.get_pixel_y();

		raylib::DrawRectangleLinesEx(
			raylib::Rectangle{ (float)Fx - constant::TILE_HALF,
			  (float)Fy - constant::TILE_HALF,
			  constant::TILE_SIZE,
			  constant::TILE_SIZE },
			1, raylib::RED);
	}
}

bool Entity::is_point_over_entity(int world_x, int world_y) const
{
	PAKLib::sprite_rect bounds = get_entity_bounds();

	// Calculate the actual rendered position (pixel position + pivot offset)
	int left = position.get_pixel_x() + bounds.pivotX;
	int top = position.get_pixel_y() + bounds.pivotY;
	int right = left + bounds.width;
	int bottom = top + bounds.height;

	return world_x >= left && world_x <= right && world_y >= top && world_y <= bottom;
}

void Entity::render_nameplate() const
{
	PAKLib::sprite_rect bounds = get_entity_bounds();

	// Position nameplate below the entity
	float center_x = static_cast<float>(position.get_pixel_x());
	float bottom_y = static_cast<float>(position.get_pixel_y() + bounds.pivotY + bounds.height) + 5.0f;  // 5 pixels below sprite

	// Format: "DisplayName" on first line, "[GUID]" on second line
	std::string guid_str = std::format("[{:08X}]", static_cast<uint32_t>(_guid.get_id() & 0xFFFFFFFF));

	// Get font for measurements
	raylib::Font& name_font = FontSystem::get_font(FontFamily::Default, 14);
	raylib::Font& guid_font = FontSystem::get_font(FontFamily::Default, 12);

	// Measure text widths to center them
	raylib::Vector2 name_size = raylib::MeasureTextEx(name_font, _display_name.c_str(), 14.0f, 1.0f);
	raylib::Vector2 guid_size = raylib::MeasureTextEx(guid_font, guid_str.c_str(), 12.0f, 1.0f);

	// Draw display name (centered, white with black outline)
	float name_x = center_x - (name_size.x / 2.0f);
	float name_y = bottom_y;

	// Draw outline using shadow style
	FontSystem::draw_text(FontFamily::Default, 14, _display_name.c_str(), name_x, name_y, raylib::WHITE, FontStyle::Regular | FontStyle::Shadow);

	// Draw GUID (centered, gray)
	float guid_x = center_x - (guid_size.x / 2.0f);
	float guid_y = bottom_y + name_size.y + 2.0f;

	FontSystem::draw_text(FontFamily::Default, 12, guid_str.c_str(), guid_x, guid_y, raylib::Color{ 180, 180, 180, 255 });
}

void Entity::attack(Entity* target)
{
	if (!target)
		return;

	// Call the before-attack hook to get damage (can be overridden)
	int damage = on_before_attack(target);

	// If damage is negative (miss, dodge, etc.), just show indicator - don't deliver attack
	if (damage < 0)
	{
		target->spawn_damage_indicator(damage);
	}
	else
	{
		// Deliver the attack to the target
		target->receive_attack(this, damage);
	}

	// Call the after-attack hook (can be overridden)
	on_after_attack(target);
}

void Entity::receive_attack(Entity* attacker, int damage)
{
	// Can't receive attacks if already dead/dying
	if (_is_dying || _is_dead)
		return;

	// Spawn damage indicator
	spawn_damage_indicator(damage);

	// Call the receive-attack hook (can be overridden) - if it returns false, don't apply damage
	if (!on_receive_attack(attacker, damage))
		return;

	// Apply damage to health via the internal hit handler
	_on_hit(attacker, damage);
}

bool Entity::is_in_attack_range_of(const Entity* target) const
{
	if (!target)
		return false;

	// Calculate tile distance to target (adjacent = within 1 tile)
	int dx = std::abs(static_cast<int>(target->get_position().get_tile_x()) - static_cast<int>(position.get_tile_x()));
	int dy = std::abs(static_cast<int>(target->get_position().get_tile_y()) - static_cast<int>(position.get_tile_y()));

	// Adjacent if both dx and dy are <= 1 (includes diagonals)
	return dx <= 1 && dy <= 1;
}

void Entity::spawn_damage_indicator(int damage)
{
	// Bump all existing indicators - give them upward velocity to simulate being pushed
	for (auto& indicator : _damage_indicators)
	{
		if (!indicator.is_bumped)
		{
			// First time being bumped - give initial velocity
			indicator.is_bumped = true;
			indicator.is_settling = false;
			indicator.velocity_y = DamageIndicator::BUMP_INITIAL_VELOCITY;
		}
		else
		{
			// Already bumped, add more velocity (stacks being pushed further)
			indicator.velocity_y += DamageIndicator::BUMP_INITIAL_VELOCITY * 0.5f;
		}
	}

	// Create new damage indicator - starts below final position and settles in
	DamageIndicator new_indicator;
	new_indicator.damage = damage;
	new_indicator.time_remaining = DamageIndicator::MAX_DURATION;
	new_indicator.offset_y = DamageIndicator::SETTLE_START_OFFSET;  // Start below
	new_indicator.velocity_y = 0.0f;
	new_indicator.alpha = 1.0f;
	new_indicator.is_bumped = false;
	new_indicator.is_settling = true;
	new_indicator.bounce_time = 0.0f;

	_damage_indicators.push_back(new_indicator);
}

void Entity::update_damage_indicators(float delta_time)
{
	for (auto it = _damage_indicators.begin(); it != _damage_indicators.end(); )
	{
		it->time_remaining -= delta_time;

		if (it->is_bumped)
		{
			// Apply velocity to position (negative velocity = upward movement)
			it->offset_y += it->velocity_y * delta_time;

			// Decelerate velocity toward zero (slow down the rise)
			if (it->velocity_y < 0.0f)
			{
				it->velocity_y += DamageIndicator::BUMP_DECELERATION * delta_time;
				if (it->velocity_y > 0.0f)
					it->velocity_y = 0.0f;
			}

			// Fade out bumped indicators
			it->alpha -= DamageIndicator::BUMP_FADE_SPEED * delta_time;
		}
		else if (it->is_settling)
		{
			// New indicator settling into place (moving upward toward offset_y = 0)
			it->offset_y -= DamageIndicator::SETTLE_SPEED * delta_time;

			if (it->offset_y <= 0.0f)
			{
				// Reached final position - start rubberband bounce
				it->offset_y = 0.0f;
				it->is_settling = false;
				it->bounce_time = DamageIndicator::BOUNCE_DURATION;
			}
		}
		else if (it->bounce_time > 0.0f)
		{
			// Rubberband bounce - overshoots upward then settles back
			it->bounce_time -= delta_time;

			if (it->bounce_time > 0.0f)
			{
				// Use a damped sine wave for rubberband effect
				// Starts at 0, goes negative (up), then oscillates back to 0
				float progress = 1.0f - (it->bounce_time / DamageIndicator::BOUNCE_DURATION);
				// Damped oscillation: sin curve that starts at 0, peaks negative, returns to 0
				float bounce = -sinf(progress * 3.14159f) * DamageIndicator::BOUNCE_INTENSITY * (1.0f - progress);
				it->offset_y = bounce;
			}
			else
			{
				it->offset_y = 0.0f;
				it->bounce_time = 0.0f;
			}
		}
		else
		{
			// Normal indicators just count down time
			// Fade out in the last 0.3 seconds
			if (it->time_remaining < 0.3f)
			{
				it->alpha = it->time_remaining / 0.3f;
			}
		}

		// Remove if expired or fully faded
		if (it->time_remaining <= 0.0f || it->alpha <= 0.0f)
		{
			it = _damage_indicators.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void Entity::render_damage_indicators() const
{
	if (_damage_indicators.empty())
		return;

	PAKLib::sprite_rect bounds = get_entity_bounds();

	// Base position above entity head
	float center_x = static_cast<float>(position.get_pixel_x());
	float base_y = static_cast<float>(position.get_pixel_y() + bounds.pivotY) - 5.0f;  // 5 pixels above sprite top

	for (const auto& indicator : _damage_indicators)
	{
		// Calculate alpha for color
		uint8_t alpha = static_cast<uint8_t>(std::clamp(indicator.alpha, 0.0f, 1.0f) * 255.0f);

		// Vibrant golden yellow color
		raylib::Color damage_color = { 255, 215, 0, alpha };

		// Determine font, size, and text based on damage
		uint8_t font_family;
		int font_size;
		std::string display_text;
		raylib::Color text_color;

		if (indicator.damage == AttackResult::MISS)
		{
			// Miss - show "miss" text
			font_family = FontFamily::Default;
			font_size = 12;
			display_text = "miss";
			text_color = { 200, 200, 200, alpha };  // Gray for miss
		}
		else if (indicator.damage > 128)
		{
			// Critical hit - show "crit!" instead of damage number
			font_family = FontFamily::Fancy;
			font_size = 14;
			display_text = "crit!";
			text_color = damage_color;
		}
		else if (indicator.damage > 12)
		{
			// High damage - fancy font
			font_family = FontFamily::Fancy;
			font_size = 14;
			display_text = std::format("-{}", indicator.damage);
			text_color = damage_color;
		}
		else
		{
			// Normal damage - default font
			font_family = FontFamily::Default;
			font_size = 12;
			display_text = std::format("-{}", indicator.damage);
			text_color = damage_color;
		}

		// Get text width for centering
		raylib::Font& font = FontSystem::get_font(font_family, font_size);
		raylib::Vector2 text_size = raylib::MeasureTextEx(font, display_text.c_str(), static_cast<float>(font_size), 1.0f);

		float text_x = center_x - (text_size.x / 2.0f);
		float text_y = base_y + indicator.offset_y - text_size.y;

		// Draw text with shadow for visibility
		FontSystem::draw_text(font_family, font_size, display_text.c_str(), text_x, text_y, text_color, FontStyle::Regular | FontStyle::Shadow);
	}
}

void Entity::_on_hit(Entity* attacker, int damage)
{
	// Skip if already dying or dead
	if (_is_dying || _is_dead)
		return;

	// Skip if damage is negative (miss, dodge, block, etc.)
	if (damage < 0)
		return;

	// Apply damage to health
	uint32_t current_hp = _stats->get_stat(StatType::HEALTH);
	if (static_cast<uint32_t>(damage) >= current_hp)
	{
		_stats->set_stat(StatType::HEALTH, 0);
		_on_dying(attacker);
	}
	else
	{
		_stats->set_stat(StatType::HEALTH, current_hp - damage);
	}
}

void Entity::_on_dying(Entity* attacker)
{
	// Allow override to cancel dying
	if (!on_before_dying(attacker))
		return;

	_is_dying = true;
	_killed_by_entity = attacker;

	// Stop movement immediately
	if (_is_moving)
	{
		stop_movement();
	}

	// Set dying animation
	set_animation(AnimationType::DYING, WeaponUsed::HAND);

	// Call after dying hook
	on_after_dying(attacker);
}

void Entity::_on_death()
{
	// Initialize death wait timer on first call
	if (_dying_wait_timer == 0.0f)
	{
		_dying_wait_timer = static_cast<float>(raylib::GetTime());
	}

	// Initialize opacity timer
	if (_death_opacity_timer == 0.0f)
	{
		_death_opacity_timer = static_cast<float>(raylib::GetTime());
	}

	// If already dead, handle fade-out
	if (_is_dead)
	{
		float current_time = static_cast<float>(raylib::GetTime());

		// Wait for dying interval before fading
		if (current_time - _dying_wait_timer >= _dying_wait_interval)
		{
			// Fade out over time
			if (current_time - _death_opacity_timer >= _death_opacity_interval)
			{
				_death_opacity_timer = current_time;
				_dead_opacity -= 0.05f;
			}

			if (_dead_opacity <= 0.0f)
			{
				_dead_opacity = 0.0f;
				_should_remove = true;
			}
		}
		return;
	}

	// Allow override to cancel death
	if (!on_before_death())
		return;

	_is_dead = true;

	// Call after death hook
	on_after_death();
}

void Entity::_update_death()
{
	// If dying but not dead yet, check if dying animation finished
	if (_is_dying && !_is_dead)
	{
		// Check if dying animation is complete
		// For now, use the animation system - dying animation should complete then trigger death
		if (current_animation.is_finished() && current_animation_type == AnimationType::DYING)
		{
			_on_death();
		}
	}
	// If dead, continue death handling (fade-out)
	else if (_is_dead && !_should_remove)
	{
		_on_death();
	}
}
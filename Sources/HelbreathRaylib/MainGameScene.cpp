#include "MainGameScene.h"
#include "PAK.h"
#include "Sprite.h"
#include "SceneManager.h"
#include "FontSystem.h"
#include "Application.h"
#include "IDs.h"
#include "CMap.h"
#include "entities.h"
#include "ItemIDs.h"

MainGameScene::MainGameScene(const GameEntryData& entry_data)
	: _entry_data(entry_data)
{
}

void MainGameScene::on_initialize()
{
	_map_data = CMapLoader::load_by_index(MapID::Default);

	// Create the player from entry data
	_player = &get_entity_manager().Add<Player>(_entry_data.appearance, _entry_data.stats);

	// Set initial position (Default map spawn point)
	_player->set_position({ 135, 37 });
	_player->set_active_map(_map_data.get());

	// Add a test dummy entity near the player spawn - stationary with infinite HP for testing
	auto& dummy = get_entity_manager().Add<EntityDummy>();
	dummy.set_position({ 140, 40 });  // 5 tiles east, 3 tiles south of player
	dummy.set_spawn_point({ 140, 40 });
	dummy.set_movement_type(EntityMovementType::Stationary);  // Stationary for easier testing
	dummy.get_stats().set_stat(StatType::HEALTH, 999999, 999999);  // Infinite HP for testing
	dummy.set_active_map(_map_data.get());

	// Add a test slime entity at the SW edge of dummy's movement radius
	auto& slime = get_entity_manager().Add<EntitySlime>();
	slime.set_position({ 135, 45 });  // 5 tiles SW of dummy spawn
	slime.set_spawn_point({ 135, 45 });
	slime.set_movement_type(EntityMovementType::Random);
	slime.set_movement_radius(5);
	slime.set_active_map(_map_data.get());

	// Setup camera
	_camera.target = { (float)(_player->get_position().get_pixel_x()), (float)(_player->get_position().get_pixel_y()) };
	_camera.offset = { constant::BASE_WIDTH * 0.5f, constant::BASE_HEIGHT * 0.5f };
	_camera.zoom = 1.0f;
	_camera.rotation = 0.0f;

	_player->attach_camera(_camera);
}

void MainGameScene::on_uninitialize()
{

}

void MainGameScene::on_update()
{
	if (!_map_data)
		return;

	// Update targeting (hover detection) - must happen before entity updates
	_update_targeting();

	// Handle left-click/hold on entity to attack - must happen before entity updates
	// so the player knows about the target before processing movement
	if (raylib::IsMouseButtonDown(raylib::MOUSE_BUTTON_LEFT))
	{
		Entity* current_target = _player->get_attack_target();

		if (_targeted_entity)
		{
			// Mouse is over an entity - set or update attack target
			if (current_target != _targeted_entity)
			{
				_player->set_attack_target(_targeted_entity);
			}
		}
		else if (current_target && !_player->is_attacking())
		{
			// Mouse moved off target and we're not mid-attack - clear target to allow movement
			_player->clear_attack_target();
		}
		// If we're mid-attack, keep the target until attack finishes
	}

	// Handle self-click for pickup (hold to continuously pickup items)
	if (raylib::IsMouseButtonDown(raylib::MOUSE_BUTTON_LEFT) && !_targeted_entity)
	{
		// Check if player is clicking on themselves (sprite or tile)
		raylib::Vector2 mouse_world = raylib::GetScreenToWorld2D(rlx::get_mouse_position(), _camera);
		int world_x = static_cast<int>(mouse_world.x);
		int world_y = static_cast<int>(mouse_world.y);

		// Check sprite bounds first, then fall back to tile check
		bool clicking_self = _player->is_point_over_entity(world_x, world_y);
		if (!clicking_self)
		{
			GamePosition mouse_tile = get_tile_world_mouse_position(_camera);
			clicking_self = (mouse_tile.get_tile_x() == _player->get_position().get_tile_x() &&
			                 mouse_tile.get_tile_y() == _player->get_position().get_tile_y());
		}

		if (clicking_self)
		{
			// Player clicked on themselves - trigger pickup animation
			// try_pickup() returns false if already picking up, so continuous hold works
			_player->try_pickup();
		}
	}

	// Update all entities
	for (auto& Entity : get_entity_manager()) {
		Entity->update();
	}

	// Remove entities marked for removal (died and finished fade-out)
	get_entity_manager().RemoveMarkedEntities();

	// Handle middle-click teleport (debug)
	if (IsMouseButtonPressed(raylib::MOUSE_BUTTON_MIDDLE))
	{
		GamePosition TilePos = get_tile_world_mouse_position(_camera);
		_player->set_position(TilePos);
	}
}

void MainGameScene::on_render()
{
	if(!_map_data)
		return;
	raylib::rlPushMatrix();	// Pop upscale matrix
	raylib::BeginMode2D(_camera);	// Push camera matrix
	{
		// Map
		_map_data->for_each_tile_region(_camera, 18, 18, get_entity_manager(), [&](int16_t TileX, int16_t TileY, int PX, int PY, const CTile& Tile, Entity* TileOwner) {
			auto TileSpr = map_tiles[Tile.tile_sprite];

			TileSpr->draw(PX, PY, Tile.tile_sprite_frame);
			});

		//_map_data->ForEachTileRegion(_camera, [&](int16_t TileX, int16_t TileY, int PX, int PY, const CTile& Tile)
		//    {
		//        // non-visual debug
		//        if (!Tile.m_bIsMoveAllowed)
		//            DrawRectangle(PX, PY, constant::TILE_SIZE, constant::TILE_SIZE, rlx::RGBA(255, 0, 0, 42));
		//        if (Tile.m_bIsTeleport)
		//            DrawRectangle(PX, PY, constant::TILE_SIZE, constant::TILE_SIZE, rlx::RGBA(0, 0, 255, 42));
		//    });

		_player->render_debug_movement();

		// Objects
		_draw_objects_with_shadow();

		// Render nameplate for targeted entity
		if (_targeted_entity)
		{
			_targeted_entity->render_nameplate();
		}
	}
	raylib::EndMode2D();
	raylib::rlPopMatrix();	// Push upscale matrix
}

void MainGameScene::_draw_objects_with_shadow()
{
	std::vector<DrawEntry> List;
	List.reserve(512);
	_map_data->for_each_tile_region(_camera, 32, 32, get_entity_manager(), [&](int16_t TileX, int16_t TileY, int PX, int PY, const CTile& Tile, Entity* TileOwner)
		{
			if (Tile.object_sprite != 0)
			{
				List.push_back({ TileY, DrawEntry::ObjectShadow, &Tile, nullptr, PX, PY });
				List.push_back({ TileY, DrawEntry::Object, &Tile, nullptr, PX, PY });
			}

			if (!TileOwner)
				return;

			auto EntityTx = TileOwner->get_position().get_tile_x();
			auto EntityTy = TileOwner->get_position().get_tile_y();

			if (TileX == EntityTx && TileY == EntityTy)
			{
				List.push_back({ EntityTy, DrawEntry::EntityShadow, nullptr, TileOwner, 0, 0 });
				List.push_back({ EntityTy, DrawEntry::Entity, nullptr, TileOwner, 0, 0 });
			}
		});

	// Sort by Y, then by type
	// Order: ObjectShadow < EntityShadow < Entity < Object
	std::sort(List.begin(), List.end(),
		[](const DrawEntry& A, const DrawEntry& B)
		{
			if (A.sortY != B.sortY)
				return A.sortY < B.sortY;

			// Custom type order at same Y
			auto GetOrder = [](DrawEntry::Type T) {
				switch (T) {
				case DrawEntry::ObjectShadow: return 0;
				case DrawEntry::EntityShadow: return 1;
				case DrawEntry::Entity: return 2;
				case DrawEntry::Object: return 3;
				}
				return 0;
				};

			return GetOrder(A.type) < GetOrder(B.type);
		});
	_draw_all(List);
}

void MainGameScene::_draw_all(const std::vector<DrawEntry>& list)
{
	static float LightTime = 0.5f;
	//LightTime += GetFrameTime() * 0.5f;
	float LightX = sinf(LightTime) * 1.5f;
	float LightY = 0.75f;
	const raylib::Vector2 LightDir = { LightX, LightY };

	// Calculate opacity based on light angle
	// Fade out when LightX approaches extremes (-1.5 to 1.5)
	float AngleFactor = fabsf(LightX / 1.5f); // 0.0 at center, 1.0 at extremes
	float FadeStart = 0.7f; // Start fading at 70% of max angle
	float Opacity = 1.0f;
	if (AngleFactor > FadeStart)
	{
		Opacity = 1.0f - ((AngleFactor - FadeStart) / (1.0f - FadeStart));
	}

	uint8_t ShadowAlpha = uint8_t(128 * Opacity);
	raylib::Color ShadowTint = { 0, 0, 0, ShadowAlpha };

	for (auto& E : list)
	{
		switch (E.type)
		{
		case DrawEntry::ObjectShadow:
		{
			// Skip shadow for certain objects, mostly trees and buildings
			if (E.tile->object_sprite < 100 || E.tile->object_sprite >= 300 ||
				ShadowlessObjects.contains(E.tile->object_sprite))
				continue;

			auto Spr = map_tiles[E.tile->object_sprite];
			auto Frame = E.tile->object_sprite_frame;
			if ((E.tile->object_sprite >= 100 && E.tile->object_sprite < 150) && Frame > 0)
				Frame = 0;
			auto Rect = Spr->get_frame_rectangle(Frame);

			raylib::rlPushMatrix();

			float BaseX, BaseY;

			// offset fix for trees
			if (E.tile->object_sprite >= 100 && E.tile->object_sprite <= 150)
			{
				BaseX = (float)(E.pX);
				BaseY = (float)(E.pY);
			}
			else	// all other objects
			{
				float RenderedLeft = (float)(E.pX + Rect.pivotX);
				float RenderedTop = (float)(E.pY + Rect.pivotY);
				float RenderedRight = RenderedLeft + Rect.width;
				float RenderedBottom = RenderedTop + Rect.height;

				BaseX = (RenderedLeft + RenderedRight) * 0.5f;
				BaseY = RenderedBottom;
			}

			raylib::rlTranslatef(BaseX, BaseY, 0.0f);

			float SkewMatrix[16] = {
				1.0f, 0.0f, 0.0f, 0.0f,
				LightDir.x, LightDir.y, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};
			raylib::rlMultMatrixf(SkewMatrix);

			raylib::rlTranslatef(-BaseX, -BaseY, 0.0f);

			Spr->draw(E.pX, E.pY, E.tile->object_sprite_frame, ShadowTint);

			raylib::rlPopMatrix();
		}
		break;
		case DrawEntry::Object:
		{
			auto Spr = map_tiles[E.tile->object_sprite];
			uint8_t Alpha = 255;

			// Handle tree transparency when player is behind
			if (E.tile->object_sprite >= 100 && E.tile->object_sprite <= 150)
			{
				auto Pp = _player->get_position();
				int PlayerTx = Pp.get_tile_x();
				int PlayerTy = Pp.get_tile_y();

				auto Rect = Spr->get_frame_rectangle(E.tile->object_sprite_frame);

				// Calculate object bounds in pixels (including pivot)
				int ObjLeft = E.pX + Rect.pivotX;
				int ObjRight = E.pX + Rect.pivotX + Rect.width;
				int ObjTop = E.pY + Rect.pivotY;
				int ObjBottom = E.pY + Rect.pivotY + Rect.height;

				int PlayerPx = Pp.get_pixel_x();
				int PlayerPy = Pp.get_pixel_y();

				// Player is behind (lower Y) and within bounds
				if (PlayerTy < E.sortY &&
					PlayerPx >= ObjLeft && PlayerPx <= ObjRight &&
					PlayerPy >= ObjTop && PlayerPy <= ObjBottom)
				{
					Alpha = 128;
				}
			}

			Spr->draw(E.pX, E.pY, E.tile->object_sprite_frame, rlx::RGBA(255, 255, 255, Alpha));
		}
		break;
		case DrawEntry::EntityShadow:
		{
			auto Pp = E.entity->get_position();
			int PX = Pp.get_pixel_x();
			int PY = Pp.get_pixel_y();

			raylib::rlPushMatrix();

			float BaseX = (float)PX;
			float BaseY = (float)PY;

			raylib::rlTranslatef(BaseX, BaseY, 0.0f);

			float SkewMatrix[16] = {
				1.0f, 0.0f, 0.0f, 0.0f,
				LightDir.x, LightDir.y, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};
			raylib::rlMultMatrixf(SkewMatrix);

			raylib::rlTranslatef(-BaseX, -BaseY, 0.0f);

			E.entity->on_render_shadow();

			raylib::rlPopMatrix();
		}
		break;
		case DrawEntry::Entity:
			E.entity->on_render();
			E.entity->render_damage_indicators();
			break;
		}
	}
}

void MainGameScene::_update_targeting()
{
	// Find entity under mouse cursor
	_targeted_entity = _find_entity_at_mouse();
}

Entity* MainGameScene::_find_entity_at_mouse() const
{
	// Get mouse position in world coordinates
	raylib::Vector2 mouse_world = raylib::GetScreenToWorld2D(rlx::get_mouse_position(), _camera);
	int world_x = static_cast<int>(mouse_world.x);
	int world_y = static_cast<int>(mouse_world.y);

	// Calculate the tile position of the mouse
	GamePosition mouse_tile = get_tile_world_mouse_position(_camera);
	int mouse_tile_x = mouse_tile.get_tile_x();
	int mouse_tile_y = mouse_tile.get_tile_y();

	Entity* tile_entity = nullptr;  // Fallback: entity on the clicked tile

	// Check all entities (except player) to see if mouse is over them
	for (auto& entity_ptr : get_entity_manager())
	{
		// Skip the player - we don't want to target ourselves
		if (entity_ptr.get() == _player)
			continue;

		// Skip dead/dying entities - can't target them
		if (!entity_ptr->is_targetable())
			continue;

		// Primary check: mouse over sprite bounds (precise targeting)
		if (entity_ptr->is_point_over_entity(world_x, world_y))
		{
			return entity_ptr.get();
		}

		// Fallback check: entity is on the clicked tile
		if (!tile_entity)
		{
			const auto& entity_pos = entity_ptr->get_position();
			if (entity_pos.get_tile_x() == mouse_tile_x && entity_pos.get_tile_y() == mouse_tile_y)
			{
				tile_entity = entity_ptr.get();
			}
		}
	}

	// Return tile-based entity if no sprite hit was found
	return tile_entity;
}
#include "TestScene.h"
#include "PAK.h"
#include "Sprite.h"
#include "SceneManager.h"
#include "FontSystem.h"
#include "Application.h"
#include "IDs.h"
#include "CMap.h"
#include "entity.h"
#include "ItemIDs.h"

inline Player* other_player = nullptr;

void TestScene::on_initialize()
{
	_map_data = CMapLoader::load_by_index(MapID::gshop_1);

	// _player is the local client player
	_player = &get_entity_manager().Add<Player>(PlayerAppearance{
		.gender = GENDER_MALE,
		.skin_color_index = 0,
		.hair_style = HAIR_STYLE_0,
		.hair_color_index = 0,
		.underwear_color_index = 0,
		.equipment = [this]() {
			Equipment Eq{};
			Eq.legs.try_equip(ItemID::SHORTS, item_metadata);
			Eq.feet.try_equip(ItemID::BOOTS, item_metadata);
			return Eq;
		}()
		});
	other_player = &get_entity_manager().Add<Player>(PlayerAppearance{
		.gender = GENDER_FEMALE,
		.skin_color_index = 1,
		.hair_style = HAIR_STYLE_6,
		.hair_color_index = 0,
		.underwear_color_index = 1
		});

	_player->set_position({ 54, 39 });
	_player->set_active_map(_map_data.get());

	other_player->set_position({ 50, 39 });
	other_player->set_active_map(_map_data.get());

	_camera.target = { (float)(_player->get_position().get_pixel_x()), (float)(_player->get_position().get_pixel_y()) };
	_camera.offset = { constant::BASE_WIDTH * 0.5f, constant::BASE_HEIGHT * 0.5f };
	_camera.zoom = 1.0f;
	_camera.rotation = 0.0f;

	_player->attach_camera(_camera);
}

void TestScene::on_uninitialize()
{

}

void TestScene::on_update()
{
	static double MoveTimer = 0.0;
	if (raylib::GetTime() - MoveTimer >= 5.0 && !other_player->is_moving())
	{
		if (other_player->get_position() == GamePosition{ 50, 39 })
		{
			other_player->move_to({ 50, 45 });
		}
		else {
			other_player->move_to({ 50, 39 });
		}

		MoveTimer = raylib::GetTime();
	}

	for (auto& Entity : get_entity_manager()) {
		Entity->update();
	}

	if (IsMouseButtonPressed(raylib::MOUSE_BUTTON_MIDDLE))
	{
		GamePosition TilePos = get_tile_world_mouse_position(_camera);
		_player->set_position(TilePos);
		_debug_text += " Tile X: " + std::to_string(TilePos.get_tile_x()) + "\n";
		_debug_text += " Tile Y: " + std::to_string(TilePos.get_tile_y()) + "\n";
		printf("%s", _debug_text.c_str());
	//	auto Tile = _map_data->GetTile(TilePos.get_tile_x(), TilePos.get_tile_y());

	//	// non-visual debug
	//	_debug_text = "Tile information:\n";
	//	_debug_text += " Tile X: " + std::to_string(TilePos.get_tile_x()) + "\n";
	//	_debug_text += " Tile Y: " + std::to_string(TilePos.get_tile_y()) + "\n";
	//	_debug_text += " Move Allowed: " + std::string(Tile->m_bIsMoveAllowed ? "Yes" : "No") + "\n";
	//	_debug_text += " Is Teleport: " + std::string(Tile->m_bIsTeleport ? "Yes" : "No") + "\n";
	//	_debug_text += "----------------------------\n";
	//	_debug_text += " MapTileSprite: " + std::to_string(Tile->m_sTileSprite) + "\n";
	//	_debug_text += " MapTileSpriteFrame: " + std::to_string(Tile->m_sTileSpriteFrame) + "\n";
	//	_debug_text += " MapObjectSprite: " + std::to_string(Tile->m_sObjectSprite) + "\n";
	//	_debug_text += " MapObjectSpriteFrame: " + std::to_string(Tile->m_sObjectSpriteFrame) + "\n";
	//	_debug_text += "----------------------------\n";
	//	printf(_debug_text.c_str());
	}
}

void TestScene::on_render()
{
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

		other_player->render_debug_movement();

		// Objects
		_draw_objects_with_shadow();
	}
	raylib::EndMode2D();
	raylib::rlPopMatrix();	// Push upscale matrix
}

void TestScene::_draw_objects_with_shadow()
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

void TestScene::_draw_all(const std::vector<DrawEntry>& list)
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
			break;
		}
	}
}
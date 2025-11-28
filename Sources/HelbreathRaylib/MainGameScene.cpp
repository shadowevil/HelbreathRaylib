#include "MainGameScene.h"
#include "PAK.h"
#include "Sprite.h"
#include "SceneManager.h"
#include "FontSystem.h"
#include "Application.h"
#include "IDs.h"
#include "CMap.h"
#include "entity.h"
#include "ItemIDs.h"

void MainGameScene::OnInitialize()
{
	m_mapData = CMapLoader::LoadByIndex(MapID::Default);

	m_camera.target = { (float)(m_player->GetPosition().get_pixel_x()), (float)(m_player->GetPosition().get_pixel_y()) };
	m_camera.offset = { constant::BASE_WIDTH * 0.5f, constant::BASE_HEIGHT * 0.5f };
	m_camera.zoom = 1.0f;
	m_camera.rotation = 0.0f;

}

void MainGameScene::OnUninitialize()
{

}

void MainGameScene::OnUpdate()
{
	if (!m_mapData)
		return;
}

void MainGameScene::OnRender()
{
	if(!m_mapData)
		return;
	rlPushMatrix();	// Pop upscale matrix
	BeginMode2D(m_camera);	// Push camera matrix
	{
		// Map
		m_mapData->ForEachTileRegion(m_camera, 18, 18, m_entities, [&](int16_t tileX, int16_t tileY, int pX, int pY, const CTile& tile, Entity* tile_owner) {
			auto tileSpr = m_mapTiles[tile.m_sTileSprite];

			tileSpr->Draw(pX, pY, tile.m_sTileSpriteFrame);
			});

		//m_mapData->ForEachTileRegion(m_camera, [&](int16_t tileX, int16_t tileY, int pX, int pY, const CTile& tile)
		//    {
		//        // non-visual debug
		//        if (!tile.m_bIsMoveAllowed)
		//            DrawRectangle(pX, pY, constant::TILE_SIZE, constant::TILE_SIZE, rlx::RGBA(255, 0, 0, 42));
		//        if (tile.m_bIsTeleport)
		//            DrawRectangle(pX, pY, constant::TILE_SIZE, constant::TILE_SIZE, rlx::RGBA(0, 0, 255, 42));
		//    });

		m_player->RenderDebugMovement();

		// Objects
		DrawObjectsWithShadow();
	}
	EndMode2D();
	rlPopMatrix();	// Push upscale matrix
}

void MainGameScene::DrawObjectsWithShadow()
{
	std::vector<DrawEntry> list;
	list.reserve(512);
	m_mapData->ForEachTileRegion(m_camera, 32, 32, m_entities, [&](int16_t tileX, int16_t tileY, int pX, int pY, const CTile& tile, Entity* tile_owner)
		{
			if (tile.m_sObjectSprite != 0)
			{
				list.push_back({ tileY, DrawEntry::ObjectShadow, &tile, nullptr, pX, pY });
				list.push_back({ tileY, DrawEntry::Object, &tile, nullptr, pX, pY });
			}

			if (!tile_owner)
				return;

			auto entity_tx = tile_owner->GetPosition().get_tile_x();
			auto entity_ty = tile_owner->GetPosition().get_tile_y();

			if (tileX == entity_tx && tileY == entity_ty)
			{
				list.push_back({ entity_ty, DrawEntry::EntityShadow, nullptr, tile_owner, 0, 0 });
				list.push_back({ entity_ty, DrawEntry::Entity, nullptr, tile_owner, 0, 0 });
			}
		});

	// Sort by Y, then by type
	// Order: ObjectShadow < EntityShadow < Entity < Object
	std::sort(list.begin(), list.end(),
		[](const DrawEntry& a, const DrawEntry& b)
		{
			if (a.sortY != b.sortY)
				return a.sortY < b.sortY;

			// Custom type order at same Y
			auto getOrder = [](DrawEntry::Type t) {
				switch (t) {
				case DrawEntry::ObjectShadow: return 0;
				case DrawEntry::EntityShadow: return 1;
				case DrawEntry::Entity: return 2;
				case DrawEntry::Object: return 3;
				}
				return 0;
				};

			return getOrder(a.type) < getOrder(b.type);
		});
	DrawAll(list);
}

void MainGameScene::DrawAll(const std::vector<DrawEntry>& list)
{
	static float lightTime = 0.5f;
	//lightTime += GetFrameTime() * 0.5f;
	float lightX = sinf(lightTime) * 1.5f;
	float lightY = 0.75f;
	const Vector2 lightDir = { lightX, lightY };

	// Calculate opacity based on light angle
	// Fade out when lightX approaches extremes (-1.5 to 1.5)
	float angleFactor = fabsf(lightX / 1.5f); // 0.0 at center, 1.0 at extremes
	float fadeStart = 0.7f; // Start fading at 70% of max angle
	float opacity = 1.0f;
	if (angleFactor > fadeStart)
	{
		opacity = 1.0f - ((angleFactor - fadeStart) / (1.0f - fadeStart));
	}

	uint8_t shadowAlpha = uint8_t(128 * opacity);
	Color shadowTint = { 0, 0, 0, shadowAlpha };

	for (auto& e : list)
	{
		switch (e.type)
		{
		case DrawEntry::ObjectShadow:
		{
			// Skip shadow for certain objects, mostly trees and buildings
			if (e.tile->m_sObjectSprite < 100 || e.tile->m_sObjectSprite >= 300 ||
				ShadowlessObjects.contains(e.tile->m_sObjectSprite))
				continue;

			auto spr = m_mapTiles[e.tile->m_sObjectSprite];
			auto frame = e.tile->m_sObjectSpriteFrame;
			if ((e.tile->m_sObjectSprite >= 100 && e.tile->m_sObjectSprite < 150) && frame > 0)
				frame = 0;
			auto rect = spr->GetFrameRectangle(frame);

			rlPushMatrix();

			float baseX, baseY;

			// offset fix for trees 
			if (e.tile->m_sObjectSprite >= 100 && e.tile->m_sObjectSprite <= 150)
			{
				baseX = (float)(e.pX);
				baseY = (float)(e.pY);
			}
			else	// all other objects
			{
				float renderedLeft = (float)(e.pX + rect.pivotX);
				float renderedTop = (float)(e.pY + rect.pivotY);
				float renderedRight = renderedLeft + rect.width;
				float renderedBottom = renderedTop + rect.height;

				baseX = (renderedLeft + renderedRight) * 0.5f;
				baseY = renderedBottom;
			}

			rlTranslatef(baseX, baseY, 0.0f);

			float skewMatrix[16] = {
				1.0f, 0.0f, 0.0f, 0.0f,
				lightDir.x, lightDir.y, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};
			rlMultMatrixf(skewMatrix);

			rlTranslatef(-baseX, -baseY, 0.0f);

			spr->Draw(e.pX, e.pY, e.tile->m_sObjectSpriteFrame, shadowTint);

			rlPopMatrix();
		}
		break;
		case DrawEntry::Object:
		{
			auto spr = m_mapTiles[e.tile->m_sObjectSprite];
			uint8_t alpha = 255;

			// Handle tree transparency when player is behind
			if (e.tile->m_sObjectSprite >= 100 && e.tile->m_sObjectSprite <= 150)
			{
				auto pp = m_player->GetPosition();
				int player_tx = pp.get_tile_x();
				int player_ty = pp.get_tile_y();

				auto rect = spr->GetFrameRectangle(e.tile->m_sObjectSpriteFrame);

				// Calculate object bounds in pixels (including pivot)
				int objLeft = e.pX + rect.pivotX;
				int objRight = e.pX + rect.pivotX + rect.width;
				int objTop = e.pY + rect.pivotY;
				int objBottom = e.pY + rect.pivotY + rect.height;

				int playerPx = pp.get_pixel_x();
				int playerPy = pp.get_pixel_y();

				// Player is behind (lower Y) and within bounds
				if (player_ty < e.sortY &&
					playerPx >= objLeft && playerPx <= objRight &&
					playerPy >= objTop && playerPy <= objBottom)
				{
					alpha = 128;
				}
			}

			spr->Draw(e.pX, e.pY, e.tile->m_sObjectSpriteFrame, rlx::RGBA(255, 255, 255, alpha));
		}
		break;
		case DrawEntry::EntityShadow:
		{
			auto pp = e.entity->GetPosition();
			int pX = pp.get_pixel_x();
			int pY = pp.get_pixel_y();

			rlPushMatrix();

			float baseX = (float)pX;
			float baseY = (float)pY;

			rlTranslatef(baseX, baseY, 0.0f);

			float skewMatrix[16] = {
				1.0f, 0.0f, 0.0f, 0.0f,
				lightDir.x, lightDir.y, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};
			rlMultMatrixf(skewMatrix);

			rlTranslatef(-baseX, -baseY, 0.0f);

			e.entity->OnRenderShadow();

			rlPopMatrix();
		}
		break;
		case DrawEntry::Entity:
			e.entity->OnRender();
			break;
		}
	}
}
#include "Scenes.h"
#include <algorithm>
#include "ItemMetadata.h"

void LoadingScene::OnInitialize()
{
	CSpriteLoader::OpenPAK(constant::SPRITE_PATH / "Scenes.pak", [&](CSpriteLoader& loader) {
		m_sprites[SPRID_LOADINGSCREEN] = loader.GetSprite(SPR_LOADINGSCREEN::PAK_INDEX);
		});
}

void LoadingScene::OnUninitialize()
{

}

void LoadingScene::OnUpdate()
{
	switch (m_loadingStep) {
	case 0: LoadScenes(); break;
	case 1: LoadInterface(); break;
	case 15: LoadMaleGameModels(); break;
	case 35: LoadFemaleGameModels(); break;
	case 50: LoadItems(); break;
	case 80: LoadRegisterMaps(); break;
	}

	if (m_loadingStep == 100)
		m_sceneManager.SetScene<MainGameScene>();
	else
		m_loadingStep = (uint8_t)std::min(m_loadingStep + 1, 100);
}

void LoadingScene::OnRender()
{
	m_sprites[SPRID_LOADINGSCREEN]->Draw(0, 0, SPR_LOADINGSCREEN::BACKGROUND);
}

void LoadingScene::LoadSprite(size_t uStart, size_t uCount, const std::string& file, CSpriteCollection& collection)
{
	CSpriteLoader::OpenPAK(file, [&](CSpriteLoader& loader) {
		for (size_t i = 0; i < uCount; ++i)
			collection[i + uStart] = loader.GetSprite(i);
		});
}

void LoadingScene::LoadScenes()
{
	CSpriteLoader::OpenPAK(constant::SPRITE_PATH / "scenes.pak", [&](CSpriteLoader& loader) {
		m_sprites[SPRID_MAINMENUSCREEN] = loader.GetSprite(SPR_MAINMENUSCREEN::PAK_INDEX);
		m_sprites[SPRID_EXITSCREEN] = loader.GetSprite(SPR_EXITSCREEN::PAK_INDEX);
		m_sprites[SPRID_NEWACCOUNTSCREEN] = loader.GetSprite(SPR_NEWACCOUNTSCREEN::PAK_INDEX);
		m_sprites[SPRID_LOGINSCREEN] = loader.GetSprite(SPR_LOGINSCREEN::PAK_INDEX);
		m_sprites[SPRID_CHARACTERSELECTSCREEN] = loader.GetSprite(SPR_CHARACTERSELECTSCREEN::PAK_INDEX);
		m_sprites[SPRID_CREATECHARARCTERSCREEN] = loader.GetSprite(SPR_CREATECHARARCTERSCREEN::PAK_INDEX);
		});
}

void LoadingScene::LoadInterface()
{
	CSpriteLoader::OpenPAK(constant::SPRITE_PATH / "interface.pak", [&](CSpriteLoader& loader) {
		m_sprites[SPRID_MOUSECURSOR] = loader.GetSprite(SPR_MOUSECURSOR::PAK_INDEX);
		m_sprites[SPRID_BUTTONS] = loader.GetSprite(SPR_BUTTONS::PAK_INDEX);
		m_sprites[SPRID_EQUIP_MODEL] = loader.GetSprite(SPR_EQUIP::PAK_INDEX_MODEL);
		m_sprites[SPRID_EQUIP_HAIRSTYLE] = loader.GetSprite(SPR_EQUIP::PAK_INDEX_HAIRSTYLE);
		m_sprites[SPRID_EQUIP_UNDERWEAR] = loader.GetSprite(SPR_EQUIP::PAK_INDEX_UNDERWEAR);
		});
}

void LoadingScene::LoadMaleGameModels()
{
	CSpriteLoader::OpenPAK(constant::SPRITE_MALE_PATH / "male.pak", [&](CSpriteLoader& loader, PAKLib::pak& pak) {
		for (auto& sprite : pak.sprites)
		{
			size_t index = &sprite - pak.sprites.data();
			m_modelSprites[MDLID_MALE_GAME_MODEL + index] = loader.GetSprite(index);
		}
		});
	for (size_t hairStyleIndex = 1; hairStyleIndex <= HairStyle::HAIR_STYLE_COUNT; ++hairStyleIndex)
	{
		CSpriteLoader::OpenPAK(constant::SPRITE_MALE_PATH / std::string("male_hair" + std::to_string(hairStyleIndex) + ".pak"), [&](CSpriteLoader& loader, PAKLib::pak& pak) {
			for (auto& sprite : pak.sprites)
			{
				size_t pakIndex = &sprite - pak.sprites.data();
				m_modelSprites[MDLID_MALE_HAIRSTYLE_START + ((hairStyleIndex - 1) * 12) + pakIndex] = loader.GetSprite(pakIndex);
			}
			});
	}
	CSpriteLoader::OpenPAK(constant::SPRITE_MALE_PATH / "male_underwear.pak", [&](CSpriteLoader& loader, PAKLib::pak& pak) {
		for (auto& sprite : pak.sprites)
		{
			size_t index = &sprite - pak.sprites.data();
			m_modelSprites[MDLID_MALE_UNDERWEAR + index] = loader.GetSprite(index);
		}
		});
}

void LoadingScene::LoadFemaleGameModels()
{
	CSpriteLoader::OpenPAK(constant::SPRITE_FEMALE_PATH / "female.pak", [&](CSpriteLoader& loader, PAKLib::pak& pak) {
		for (auto& sprite : pak.sprites)
		{
			size_t index = &sprite - pak.sprites.data();
			m_modelSprites[MDLID_FEMALE_GAME_MODEL + index] = loader.GetSprite(index);
		}
		});
	for (size_t hairStyleIndex = 1; hairStyleIndex <= HairStyle::HAIR_STYLE_COUNT; ++hairStyleIndex)
	{
		CSpriteLoader::OpenPAK(constant::SPRITE_FEMALE_PATH / std::string("female_hair" + std::to_string(hairStyleIndex) + ".pak"), [&](CSpriteLoader& loader, PAKLib::pak& pak) {
			for (auto& sprite : pak.sprites)
			{
				size_t pakIndex = &sprite - pak.sprites.data();
				m_modelSprites[MDLID_FEMALE_HAIRSTYLE_START + ((hairStyleIndex - 1) * 12) + pakIndex] = loader.GetSprite(pakIndex);
			}
			});
	}
	CSpriteLoader::OpenPAK(constant::SPRITE_FEMALE_PATH / "female_underwear.pak", [&](CSpriteLoader& loader, PAKLib::pak& pak) {
		for (auto& sprite : pak.sprites)
		{
			size_t index = &sprite - pak.sprites.data();
			m_modelSprites[MDLID_FEMALE_UNDERWEAR + index] = loader.GetSprite(index);
		}
		});
}

void LoadingScene::LoadItems()
{
	// ------------------------------------------------------------
	// Load item atlas (equip / ground / pack)
	// ------------------------------------------------------------
	CSpriteLoader::OpenPAK(constant::SPRITE_PATH / "item_atlas.pak",
		[&](CSpriteLoader& loader)
		{
			m_sprites[SPRID_ITEM_ATLAS_EQUIP] = loader.GetSprite(0);
			m_sprites[SPRID_ITEM_ATLAS_GROUND] = loader.GetSprite(1);
			m_sprites[SPRID_ITEM_ATLAS_PACK] = loader.GetSprite(2);
		});

	// ------------------------------------------------------------
	// Load metadata list
	// ------------------------------------------------------------
	m_itemMetadata = LoadMetadataEntries(constant::DATA_PATH / "ItemMetadata.json");

	for (auto& metadata : m_itemMetadata)
	{
		size_t index = &metadata - m_itemMetadata.data();
		auto path = constant::SPRITE_ITEM_PATH / metadata.pak_file;

		// ------------------------------------------------------------
		// Validate frame indices (only if metadata present)
		// ------------------------------------------------------------
		if (metadata.male)
		{
			m_sprites[SPRID_ITEM_ATLAS_EQUIP]->GetFrameRectangle(metadata.male->equip_frame_index);
			m_sprites[SPRID_ITEM_ATLAS_GROUND]->GetFrameRectangle(metadata.male->ground_frame_index);
			m_sprites[SPRID_ITEM_ATLAS_PACK]->GetFrameRectangle(metadata.male->pack_frame_index);

			if (metadata.male->pak_end_index < metadata.male->pak_start_index)
			{
				throw std::runtime_error(
					"Invalid male sprite range in " + metadata.pak_file +
					": start=" + std::to_string(metadata.male->pak_start_index) +
					" end=" + std::to_string(metadata.male->pak_end_index)
				);
			}
		}

		if (metadata.female)
		{
			m_sprites[SPRID_ITEM_ATLAS_EQUIP]->GetFrameRectangle(metadata.female->equip_frame_index);
			m_sprites[SPRID_ITEM_ATLAS_GROUND]->GetFrameRectangle(metadata.female->ground_frame_index);
			m_sprites[SPRID_ITEM_ATLAS_PACK]->GetFrameRectangle(metadata.female->pack_frame_index);

			if (metadata.female->pak_end_index < metadata.female->pak_start_index)
			{
				throw std::runtime_error(
					"Invalid female sprite range in " + metadata.pak_file +
					": start=" + std::to_string(metadata.female->pak_start_index) +
					" end=" + std::to_string(metadata.female->pak_end_index)
				);
			}
		}

		// ------------------------------------------------------------
		// Base index (fixed 24-slot stride)
		// ------------------------------------------------------------
		uint32_t male_base_index = MDLID_MALE_ITEM_START + static_cast<uint32_t>(index) * 24;
		uint32_t female_base_index = MDLID_FEMALE_ITEM_START + static_cast<uint32_t>(index) * 24;

		// ------------------------------------------------------------
		// Load sprites for this item PAK
		// ------------------------------------------------------------
		CSpriteLoader::OpenPAK(path, [&](CSpriteLoader& loader)
			{
				// -----------------------------
				// Load MALE (if present)
				// -----------------------------
				if (metadata.male)
				{
					uint32_t start = metadata.male->pak_start_index;
					uint32_t end = metadata.male->pak_end_index;
					uint32_t count = (end - start + 1);

					for (uint32_t i = 0; i < count; i++)
					{
						uint32_t pakIndex = start + i;
						m_modelSprites[male_base_index + i] = loader.GetSprite(pakIndex);
					}
				}

				// -----------------------------
				// Load FEMALE (if present)
				// -----------------------------
				if (metadata.female)
				{
					uint32_t start = metadata.female->pak_start_index;
					uint32_t end = metadata.female->pak_end_index;
					uint32_t count = (end - start + 1);

					for (uint32_t i = 0; i < count; i++)
					{
						uint32_t pakIndex = start + i;
						m_modelSprites[female_base_index + i] = loader.GetSprite(pakIndex);
					}
				}
			});

		printf("Loaded item metadata %zu: %s\n", index, metadata.pak_file.c_str());
	}
}

void LoadingScene::LoadRegisterMaps()
{
	LoadSprite(0, 32, (constant::SPRITE_MAP_PATH / "maptiles1.pak").string(), m_mapTiles);
	CSpriteLoader::OpenPAK((constant::SPRITE_MAP_PATH / "Structures1.pak").string(), [&](CSpriteLoader& inst) {
		m_mapTiles[51] = inst.GetSprite(1);
		m_mapTiles[55] = inst.GetSprite(5);
		});
	LoadSprite(70, 27, (constant::SPRITE_MAP_PATH / "Sinside1.pak").string(), m_mapTiles);
	LoadSprite(100, 46, (constant::SPRITE_MAP_PATH / "TREES1.pak").string(), m_mapTiles);
	LoadSprite(200, 10, (constant::SPRITE_MAP_PATH / "Objects1.pak").string(), m_mapTiles);
	LoadSprite(211, 5, (constant::SPRITE_MAP_PATH / "Objects2.pak").string(), m_mapTiles);
	LoadSprite(216, 4, (constant::SPRITE_MAP_PATH / "Objects3.pak").string(), m_mapTiles);
	LoadSprite(220, 2, (constant::SPRITE_MAP_PATH / "objects4.pak").string(), m_mapTiles);
	LoadSprite(223, 3, (constant::SPRITE_MAP_PATH / "Tile223-225.pak").string(), m_mapTiles);
	LoadSprite(226, 4, (constant::SPRITE_MAP_PATH / "Tile226-229.pak").string(), m_mapTiles);
	LoadSprite(230, 9, (constant::SPRITE_MAP_PATH / "Objects5.pak").string(), m_mapTiles);
	LoadSprite(238, 4, (constant::SPRITE_MAP_PATH / "Objects6.pak").string(), m_mapTiles);
	LoadSprite(242, 7, (constant::SPRITE_MAP_PATH / "Objects7.pak").string(), m_mapTiles);
	LoadSprite(300, 15, (constant::SPRITE_MAP_PATH / "maptiles2.pak").string(), m_mapTiles);
	LoadSprite(320, 10, (constant::SPRITE_MAP_PATH / "maptiles4.pak").string(), m_mapTiles);
	LoadSprite(330, 19, (constant::SPRITE_MAP_PATH / "maptiles5.pak").string(), m_mapTiles);
	LoadSprite(349, 4, (constant::SPRITE_MAP_PATH / "maptiles6.pak").string(), m_mapTiles);
	LoadSprite(353, 9, (constant::SPRITE_MAP_PATH / "maptiles353-361.pak").string(), m_mapTiles);
	LoadSprite(363, 4, (constant::SPRITE_MAP_PATH / "Tile363-366.pak").string(), m_mapTiles);
	LoadSprite(367, 1, (constant::SPRITE_MAP_PATH / "Tile367-367.pak").string(), m_mapTiles);
	LoadSprite(370, 12, (constant::SPRITE_MAP_PATH / "Tile370-381.pak").string(), m_mapTiles);
	LoadSprite(382, 6, (constant::SPRITE_MAP_PATH / "Tile382-387.pak").string(), m_mapTiles);
	LoadSprite(388, 15, (constant::SPRITE_MAP_PATH / "Tile388-402.pak").string(), m_mapTiles);
	LoadSprite(403, 3, (constant::SPRITE_MAP_PATH / "Tile403-405.pak").string(), m_mapTiles);
	LoadSprite(406, 16, (constant::SPRITE_MAP_PATH / "Tile406-421.pak").string(), m_mapTiles);
	LoadSprite(422, 8, (constant::SPRITE_MAP_PATH / "Tile422-429.pak").string(), m_mapTiles);
	LoadSprite(430, 14, (constant::SPRITE_MAP_PATH / "Tile430-443.pak").string(), m_mapTiles);
	LoadSprite(444, 1, (constant::SPRITE_MAP_PATH / "Tile444-444.pak").string(), m_mapTiles);
	LoadSprite(445, 17, (constant::SPRITE_MAP_PATH / "Tile445-461.pak").string(), m_mapTiles);
	LoadSprite(462, 12, (constant::SPRITE_MAP_PATH / "Tile462-473.pak").string(), m_mapTiles);
	LoadSprite(474, 15, (constant::SPRITE_MAP_PATH / "Tile474-488.pak").string(), m_mapTiles);
	LoadSprite(489, 34, (constant::SPRITE_MAP_PATH / "Tile489-522.pak").string(), m_mapTiles);
	LoadSprite(523, 8, (constant::SPRITE_MAP_PATH / "Tile523-530.pak").string(), m_mapTiles);
	LoadSprite(531, 10, (constant::SPRITE_MAP_PATH / "Tile531-540.pak").string(), m_mapTiles);
	LoadSprite(541, 5, (constant::SPRITE_MAP_PATH / "Tile541-545.pak").string(), m_mapTiles);

	CMapLoader::RegisterMapFile(MapID::middle2nd, constant::MAPDATA_PATH / "2ndmiddle.mtd");
	CMapLoader::RegisterMapFile(MapID::Abaddon, constant::MAPDATA_PATH / "Abaddon.mtd");
	CMapLoader::RegisterMapFile(MapID::arebrk11, constant::MAPDATA_PATH / "arebrk11.mtd");
	CMapLoader::RegisterMapFile(MapID::arebrk12, constant::MAPDATA_PATH / "arebrk12.mtd");
	CMapLoader::RegisterMapFile(MapID::arebrk21, constant::MAPDATA_PATH / "arebrk21.mtd");
	CMapLoader::RegisterMapFile(MapID::arebrk22, constant::MAPDATA_PATH / "arebrk22.mtd");
	CMapLoader::RegisterMapFile(MapID::arefarm, constant::MAPDATA_PATH / "arefarm.mtd");
	CMapLoader::RegisterMapFile(MapID::arejail, constant::MAPDATA_PATH / "arejail.mtd");
	CMapLoader::RegisterMapFile(MapID::ARESDEN, constant::MAPDATA_PATH / "ARESDEN.mtd");
	CMapLoader::RegisterMapFile(MapID::aresdend1, constant::MAPDATA_PATH / "aresdend1.mtd");
	CMapLoader::RegisterMapFile(MapID::areuni, constant::MAPDATA_PATH / "areuni.mtd");
	CMapLoader::RegisterMapFile(MapID::arewrhus, constant::MAPDATA_PATH / "arewrhus.mtd");
	CMapLoader::RegisterMapFile(MapID::bisle, constant::MAPDATA_PATH / "bisle.mtd");
	CMapLoader::RegisterMapFile(MapID::bsmith_1, constant::MAPDATA_PATH / "bsmith_1.mtd");
	CMapLoader::RegisterMapFile(MapID::bsmith_1f, constant::MAPDATA_PATH / "bsmith_1f.mtd");
	CMapLoader::RegisterMapFile(MapID::bsmith_2, constant::MAPDATA_PATH / "bsmith_2.mtd");
	CMapLoader::RegisterMapFile(MapID::bsmith_2f, constant::MAPDATA_PATH / "bsmith_2f.mtd");
	CMapLoader::RegisterMapFile(MapID::BTField, constant::MAPDATA_PATH / "BTField.mtd");
	CMapLoader::RegisterMapFile(MapID::cath_1, constant::MAPDATA_PATH / "cath_1.mtd");
	CMapLoader::RegisterMapFile(MapID::cath_2, constant::MAPDATA_PATH / "cath_2.mtd");
	CMapLoader::RegisterMapFile(MapID::cityhall_1, constant::MAPDATA_PATH / "cityhall_1.mtd");
	CMapLoader::RegisterMapFile(MapID::cityhall_2, constant::MAPDATA_PATH / "cityhall_2.mtd");
	CMapLoader::RegisterMapFile(MapID::Cmdhall_1, constant::MAPDATA_PATH / "Cmdhall_1.mtd");
	CMapLoader::RegisterMapFile(MapID::Cmdhall_2, constant::MAPDATA_PATH / "Cmdhall_2.mtd");
	CMapLoader::RegisterMapFile(MapID::Default, constant::MAPDATA_PATH / "Default.mtd");
	CMapLoader::RegisterMapFile(MapID::dglv2, constant::MAPDATA_PATH / "dglv2.mtd");
	CMapLoader::RegisterMapFile(MapID::dglv3, constant::MAPDATA_PATH / "dglv3.mtd");
	CMapLoader::RegisterMapFile(MapID::dglv4, constant::MAPDATA_PATH / "dglv4.mtd");
	CMapLoader::RegisterMapFile(MapID::DruncnCity, constant::MAPDATA_PATH / "DruncnCity.mtd");
	CMapLoader::RegisterMapFile(MapID::elvbrk11, constant::MAPDATA_PATH / "elvbrk11.mtd");
	CMapLoader::RegisterMapFile(MapID::elvbrk12, constant::MAPDATA_PATH / "elvbrk12.mtd");
	CMapLoader::RegisterMapFile(MapID::elvbrk21, constant::MAPDATA_PATH / "elvbrk21.mtd");
	CMapLoader::RegisterMapFile(MapID::elvbrk22, constant::MAPDATA_PATH / "elvbrk22.mtd");
	CMapLoader::RegisterMapFile(MapID::elvfarm, constant::MAPDATA_PATH / "elvfarm.mtd");
	CMapLoader::RegisterMapFile(MapID::ELVINE, constant::MAPDATA_PATH / "ELVINE.mtd");
	CMapLoader::RegisterMapFile(MapID::elvined1, constant::MAPDATA_PATH / "elvined1.mtd");
	CMapLoader::RegisterMapFile(MapID::elvjail, constant::MAPDATA_PATH / "elvjail.mtd");
	CMapLoader::RegisterMapFile(MapID::elvuni, constant::MAPDATA_PATH / "elvuni.mtd");
	CMapLoader::RegisterMapFile(MapID::elvwrhus, constant::MAPDATA_PATH / "elvwrhus.mtd");
	CMapLoader::RegisterMapFile(MapID::fightzone1, constant::MAPDATA_PATH / "fightzone1.mtd");
	CMapLoader::RegisterMapFile(MapID::fightzone2, constant::MAPDATA_PATH / "fightzone2.mtd");
	CMapLoader::RegisterMapFile(MapID::fightzone3, constant::MAPDATA_PATH / "fightzone3.mtd");
	CMapLoader::RegisterMapFile(MapID::fightzone4, constant::MAPDATA_PATH / "fightzone4.mtd");
	CMapLoader::RegisterMapFile(MapID::fightzone5, constant::MAPDATA_PATH / "fightzone5.mtd");
	CMapLoader::RegisterMapFile(MapID::fightzone6, constant::MAPDATA_PATH / "fightzone6.mtd");
	CMapLoader::RegisterMapFile(MapID::fightzone7, constant::MAPDATA_PATH / "fightzone7.mtd");
	CMapLoader::RegisterMapFile(MapID::fightzone8, constant::MAPDATA_PATH / "fightzone8.mtd");
	CMapLoader::RegisterMapFile(MapID::fightzone9, constant::MAPDATA_PATH / "fightzone9.mtd");
	CMapLoader::RegisterMapFile(MapID::gldhall_1, constant::MAPDATA_PATH / "gldhall_1.mtd");
	CMapLoader::RegisterMapFile(MapID::gldhall_2, constant::MAPDATA_PATH / "gldhall_2.mtd");
	CMapLoader::RegisterMapFile(MapID::GodH, constant::MAPDATA_PATH / "GodH.mtd");
	CMapLoader::RegisterMapFile(MapID::gshop_1, constant::MAPDATA_PATH / "gshop_1.mtd");
	CMapLoader::RegisterMapFile(MapID::gshop_1f, constant::MAPDATA_PATH / "gshop_1f.mtd");
	CMapLoader::RegisterMapFile(MapID::gshop_2, constant::MAPDATA_PATH / "gshop_2.mtd");
	CMapLoader::RegisterMapFile(MapID::gshop_2f, constant::MAPDATA_PATH / "gshop_2f.mtd");
	CMapLoader::RegisterMapFile(MapID::HRampart, constant::MAPDATA_PATH / "HRampart.mtd");
	CMapLoader::RegisterMapFile(MapID::huntzone1, constant::MAPDATA_PATH / "huntzone1.mtd");
	CMapLoader::RegisterMapFile(MapID::huntzone2, constant::MAPDATA_PATH / "huntzone2.mtd");
	CMapLoader::RegisterMapFile(MapID::huntzone3, constant::MAPDATA_PATH / "huntzone3.mtd");
	CMapLoader::RegisterMapFile(MapID::huntzone4, constant::MAPDATA_PATH / "huntzone4.mtd");
	CMapLoader::RegisterMapFile(MapID::icebound, constant::MAPDATA_PATH / "icebound.mtd");
	CMapLoader::RegisterMapFile(MapID::inferniaA, constant::MAPDATA_PATH / "inferniaA.mtd");
	CMapLoader::RegisterMapFile(MapID::inferniaB, constant::MAPDATA_PATH / "inferniaB.mtd");
	CMapLoader::RegisterMapFile(MapID::maze, constant::MAPDATA_PATH / "maze.mtd");
	CMapLoader::RegisterMapFile(MapID::middled1n, constant::MAPDATA_PATH / "middled1n.mtd");
	CMapLoader::RegisterMapFile(MapID::middled1x, constant::MAPDATA_PATH / "middled1x.mtd");
	CMapLoader::RegisterMapFile(MapID::middleland, constant::MAPDATA_PATH / "middleland.mtd");
	CMapLoader::RegisterMapFile(MapID::procella, constant::MAPDATA_PATH / "procella.mtd");
	CMapLoader::RegisterMapFile(MapID::Resurr1, constant::MAPDATA_PATH / "Resurr1.mtd");
	CMapLoader::RegisterMapFile(MapID::Resurr2, constant::MAPDATA_PATH / "Resurr2.mtd");
	CMapLoader::RegisterMapFile(MapID::Toh1, constant::MAPDATA_PATH / "Toh1.mtd");
	CMapLoader::RegisterMapFile(MapID::Toh2, constant::MAPDATA_PATH / "Toh2.mtd");
	CMapLoader::RegisterMapFile(MapID::Toh3, constant::MAPDATA_PATH / "Toh3.mtd");
	CMapLoader::RegisterMapFile(MapID::wrhus_1, constant::MAPDATA_PATH / "wrhus_1.mtd");
	CMapLoader::RegisterMapFile(MapID::wrhus_1f, constant::MAPDATA_PATH / "wrhus_1f.mtd");
	CMapLoader::RegisterMapFile(MapID::wrhus_2, constant::MAPDATA_PATH / "wrhus_2.mtd");
	CMapLoader::RegisterMapFile(MapID::wrhus_2f, constant::MAPDATA_PATH / "wrhus_2f.mtd");
	CMapLoader::RegisterMapFile(MapID::wzdtwr_1, constant::MAPDATA_PATH / "wzdtwr_1.mtd");
	CMapLoader::RegisterMapFile(MapID::wzdtwr_2, constant::MAPDATA_PATH / "wzdtwr_2.mtd");
}

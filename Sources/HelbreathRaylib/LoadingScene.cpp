#include "Scenes.h"
#include <algorithm>
#include "ItemMetadata.h"

void LoadingScene::on_initialize()
{
	CSpriteLoader::open_pak(constant::SPRITE_PATH / "scenes.pak", [&](CSpriteLoader& Loader) {
		sprites[SPRID_LOADINGSCREEN] = Loader.get_sprite(SPR_LOADINGSCREEN::PAK_INDEX);
		});
}

void LoadingScene::on_uninitialize()
{

}

void LoadingScene::on_update()
{
	switch (_loading_step) {
	case 0: _load_scenes(); break;
	case 1: _load_interface(); break;
	case 15: _load_male_game_models(); break;
	case 35: _load_female_game_models(); break;
	case 50: _load_items(); break;
	case 80: _load_register_maps(); break;
	}

	if (_loading_step == 100)
		scene_manager.set_scene<TestScene>();
	else
		_loading_step = (uint8_t)std::min(_loading_step + 1, 100);
}

void LoadingScene::on_render()
{
	sprites[SPRID_LOADINGSCREEN]->draw(0, 0, SPR_LOADINGSCREEN::BACKGROUND);
}

void LoadingScene::_load_sprite(size_t u_start, size_t u_count, const std::string& file, CSpriteCollection& collection)
{
	CSpriteLoader::open_pak(file, [&](CSpriteLoader& Loader) {
		for (size_t i = 0; i < u_count; ++i)
			collection[i + u_start] = Loader.get_sprite(i);
		});
}

void LoadingScene::_load_scenes()
{
	CSpriteLoader::open_pak(constant::SPRITE_PATH / "scenes.pak", [&](CSpriteLoader& Loader) {
		sprites[SPRID_MAINMENUSCREEN] = Loader.get_sprite(SPR_MAINMENUSCREEN::PAK_INDEX);
		sprites[SPRID_EXITSCREEN] = Loader.get_sprite(SPR_EXITSCREEN::PAK_INDEX);
		sprites[SPRID_NEWACCOUNTSCREEN] = Loader.get_sprite(SPR_NEWACCOUNTSCREEN::PAK_INDEX);
		sprites[SPRID_LOGINSCREEN] = Loader.get_sprite(SPR_LOGINSCREEN::PAK_INDEX);
		sprites[SPRID_CHARACTERSELECTSCREEN] = Loader.get_sprite(SPR_CHARACTERSELECTSCREEN::PAK_INDEX);
		sprites[SPRID_CREATECHARARCTERSCREEN] = Loader.get_sprite(SPR_CREATECHARARCTERSCREEN::PAK_INDEX);
		});
}

void LoadingScene::_load_interface()
{
	CSpriteLoader::open_pak(constant::SPRITE_PATH / "interface.pak", [&](CSpriteLoader& Loader) {
		sprites[SPRID_MOUSECURSOR] = Loader.get_sprite(SPR_MOUSECURSOR::PAK_INDEX);
		sprites[SPRID_BUTTONS] = Loader.get_sprite(SPR_BUTTONS::PAK_INDEX);
		sprites[SPRID_EQUIP_MODEL] = Loader.get_sprite(SPR_EQUIP::PAK_INDEX_MODEL);
		sprites[SPRID_EQUIP_HAIRSTYLE] = Loader.get_sprite(SPR_EQUIP::PAK_INDEX_HAIRSTYLE);
		sprites[SPRID_EQUIP_UNDERWEAR] = Loader.get_sprite(SPR_EQUIP::PAK_INDEX_UNDERWEAR);
		});
}

void LoadingScene::_load_male_game_models()
{
	CSpriteLoader::open_pak(constant::SPRITE_MALE_PATH / "male.pak", [&](CSpriteLoader& Loader, PAKLib::pak& Pak) {
		for (auto& Sprite : Pak.sprites)
		{
			size_t Index = &Sprite - Pak.sprites.data();
			model_sprites[MDLID_MALE_GAME_MODEL + Index] = Loader.get_sprite(Index);
		}
		});
	for (size_t HairStyleIndex = 1; HairStyleIndex <= HairStyle::HAIR_STYLE_COUNT; ++HairStyleIndex)
	{
		CSpriteLoader::open_pak(constant::SPRITE_MALE_PATH / std::string("male_hair" + std::to_string(HairStyleIndex) + ".pak"), [&](CSpriteLoader& Loader, PAKLib::pak& Pak) {
			for (auto& Sprite : Pak.sprites)
			{
				size_t PakIndex = &Sprite - Pak.sprites.data();
				model_sprites[MDLID_MALE_HAIRSTYLE_START + ((HairStyleIndex - 1) * 12) + PakIndex] = Loader.get_sprite(PakIndex);
			}
			});
	}
	CSpriteLoader::open_pak(constant::SPRITE_MALE_PATH / "male_underwear.pak", [&](CSpriteLoader& Loader, PAKLib::pak& Pak) {
		for (auto& Sprite : Pak.sprites)
		{
			size_t Index = &Sprite - Pak.sprites.data();
			model_sprites[MDLID_MALE_UNDERWEAR + Index] = Loader.get_sprite(Index);
		}
		});
}

void LoadingScene::_load_female_game_models()
{
	CSpriteLoader::open_pak(constant::SPRITE_FEMALE_PATH / "female.pak", [&](CSpriteLoader& Loader, PAKLib::pak& Pak) {
		for (auto& Sprite : Pak.sprites)
		{
			size_t Index = &Sprite - Pak.sprites.data();
			model_sprites[MDLID_FEMALE_GAME_MODEL + Index] = Loader.get_sprite(Index);
		}
		});
	for (size_t HairStyleIndex = 1; HairStyleIndex <= HairStyle::HAIR_STYLE_COUNT; ++HairStyleIndex)
	{
		CSpriteLoader::open_pak(constant::SPRITE_FEMALE_PATH / std::string("female_hair" + std::to_string(HairStyleIndex) + ".pak"), [&](CSpriteLoader& Loader, PAKLib::pak& Pak) {
			for (auto& Sprite : Pak.sprites)
			{
				size_t PakIndex = &Sprite - Pak.sprites.data();
				model_sprites[MDLID_FEMALE_HAIRSTYLE_START + ((HairStyleIndex - 1) * 12) + PakIndex] = Loader.get_sprite(PakIndex);
			}
			});
	}
	CSpriteLoader::open_pak(constant::SPRITE_FEMALE_PATH / "female_underwear.pak", [&](CSpriteLoader& Loader, PAKLib::pak& Pak) {
		for (auto& Sprite : Pak.sprites)
		{
			size_t Index = &Sprite - Pak.sprites.data();
			model_sprites[MDLID_FEMALE_UNDERWEAR + Index] = Loader.get_sprite(Index);
		}
		});
}

void LoadingScene::_load_items()
{
	// ------------------------------------------------------------
	// Load item atlas (equip / ground / pack)
	// ------------------------------------------------------------
	CSpriteLoader::open_pak(constant::SPRITE_PATH / "item_atlas.pak",
		[&](CSpriteLoader& Loader)
		{
			sprites[SPRID_ITEM_ATLAS_EQUIP] = Loader.get_sprite(0);
			sprites[SPRID_ITEM_ATLAS_GROUND] = Loader.get_sprite(1);
			sprites[SPRID_ITEM_ATLAS_PACK] = Loader.get_sprite(2);
		});

	// ------------------------------------------------------------
	// Load metadata list
	// ------------------------------------------------------------
	item_metadata = LoadMetadataEntries(constant::DATA_PATH / "ItemMetadata.json");

	for (auto& Metadata : item_metadata)
	{
		size_t Index = &Metadata - item_metadata.data();
		auto Path = constant::SPRITE_ITEM_PATH / Metadata.pak_file;

		// ------------------------------------------------------------
		// Validate frame indices (only if metadata present)
		// ------------------------------------------------------------
		if (Metadata.male)
		{
			sprites[SPRID_ITEM_ATLAS_EQUIP]->get_frame_rectangle(Metadata.male->equip_frame_index);
			sprites[SPRID_ITEM_ATLAS_GROUND]->get_frame_rectangle(Metadata.male->ground_frame_index);
			sprites[SPRID_ITEM_ATLAS_PACK]->get_frame_rectangle(Metadata.male->pack_frame_index);

			if (Metadata.male->pak_end_index < Metadata.male->pak_start_index)
			{
				throw std::runtime_error(
					"Invalid male sprite range in " + Metadata.pak_file +
					": start=" + std::to_string(Metadata.male->pak_start_index) +
					" end=" + std::to_string(Metadata.male->pak_end_index)
				);
			}
		}

		if (Metadata.female)
		{
			sprites[SPRID_ITEM_ATLAS_EQUIP]->get_frame_rectangle(Metadata.female->equip_frame_index);
			sprites[SPRID_ITEM_ATLAS_GROUND]->get_frame_rectangle(Metadata.female->ground_frame_index);
			sprites[SPRID_ITEM_ATLAS_PACK]->get_frame_rectangle(Metadata.female->pack_frame_index);

			if (Metadata.female->pak_end_index < Metadata.female->pak_start_index)
			{
				throw std::runtime_error(
					"Invalid female sprite range in " + Metadata.pak_file +
					": start=" + std::to_string(Metadata.female->pak_start_index) +
					" end=" + std::to_string(Metadata.female->pak_end_index)
				);
			}
		}

		// ------------------------------------------------------------
		// Base index (fixed 24-slot stride)
		// ------------------------------------------------------------
		uint32_t MaleBaseIndex = MDLID_MALE_ITEM_START + static_cast<uint32_t>(Index) * 24;
		uint32_t FemaleBaseIndex = MDLID_FEMALE_ITEM_START + static_cast<uint32_t>(Index) * 24;

		// ------------------------------------------------------------
		// Load sprites for this item PAK
		// ------------------------------------------------------------
		CSpriteLoader::open_pak(Path, [&](CSpriteLoader& Loader)
			{
				// -----------------------------
				// Load MALE (if present)
				// -----------------------------
				if (Metadata.male)
				{
					uint32_t Start = Metadata.male->pak_start_index;
					uint32_t End = Metadata.male->pak_end_index;
					uint32_t Count = (End - Start + 1);

					for (uint32_t I = 0; I < Count; I++)
					{
						uint32_t PakIndex = Start + I;
						model_sprites[MaleBaseIndex + I] = Loader.get_sprite(PakIndex);
					}
				}

				// -----------------------------
				// Load FEMALE (if present)
				// -----------------------------
				if (Metadata.female)
				{
					uint32_t Start = Metadata.female->pak_start_index;
					uint32_t End = Metadata.female->pak_end_index;
					uint32_t Count = (End - Start + 1);

					for (uint32_t I = 0; I < Count; I++)
					{
						uint32_t PakIndex = Start + I;
						model_sprites[FemaleBaseIndex + I] = Loader.get_sprite(PakIndex);
					}
				}
			});

		printf("Loaded item metadata %zu: %s\n", Index, Metadata.pak_file.c_str());
	}
}

void LoadingScene::_load_register_maps()
{
	_load_sprite(0, 32, (constant::SPRITE_MAP_PATH / "maptiles1.pak").string(), map_tiles);
	CSpriteLoader::open_pak((constant::SPRITE_MAP_PATH / "Structures1.pak").string(), [&](CSpriteLoader& Inst) {
		map_tiles[51] = Inst.get_sprite(1);
		map_tiles[55] = Inst.get_sprite(5);
		});
	_load_sprite(70, 27, (constant::SPRITE_MAP_PATH / "Sinside1.pak").string(), map_tiles);
	_load_sprite(100, 46, (constant::SPRITE_MAP_PATH / "TREES1.PAK").string(), map_tiles);
	_load_sprite(200, 10, (constant::SPRITE_MAP_PATH / "Objects1.pak").string(), map_tiles);
	_load_sprite(211, 5, (constant::SPRITE_MAP_PATH / "Objects2.pak").string(), map_tiles);
	_load_sprite(216, 4, (constant::SPRITE_MAP_PATH / "Objects3.pak").string(), map_tiles);
	_load_sprite(220, 2, (constant::SPRITE_MAP_PATH / "objects4.pak").string(), map_tiles);
	_load_sprite(223, 3, (constant::SPRITE_MAP_PATH / "Tile223-225.pak").string(), map_tiles);
	_load_sprite(226, 4, (constant::SPRITE_MAP_PATH / "Tile226-229.pak").string(), map_tiles);
	_load_sprite(230, 9, (constant::SPRITE_MAP_PATH / "Objects5.pak").string(), map_tiles);
	_load_sprite(238, 4, (constant::SPRITE_MAP_PATH / "Objects6.pak").string(), map_tiles);
	_load_sprite(242, 7, (constant::SPRITE_MAP_PATH / "Objects7.pak").string(), map_tiles);
	_load_sprite(300, 15, (constant::SPRITE_MAP_PATH / "maptiles2.pak").string(), map_tiles);
	_load_sprite(320, 10, (constant::SPRITE_MAP_PATH / "maptiles4.pak").string(), map_tiles);
	_load_sprite(330, 19, (constant::SPRITE_MAP_PATH / "maptiles5.pak").string(), map_tiles);
	_load_sprite(349, 4, (constant::SPRITE_MAP_PATH / "maptiles6.pak").string(), map_tiles);
	_load_sprite(353, 9, (constant::SPRITE_MAP_PATH / "maptiles353-361.pak").string(), map_tiles);
	_load_sprite(363, 4, (constant::SPRITE_MAP_PATH / "Tile363-366.pak").string(), map_tiles);
	_load_sprite(367, 1, (constant::SPRITE_MAP_PATH / "Tile367-367.pak").string(), map_tiles);
	_load_sprite(370, 12, (constant::SPRITE_MAP_PATH / "Tile370-381.pak").string(), map_tiles);
	_load_sprite(382, 6, (constant::SPRITE_MAP_PATH / "Tile382-387.pak").string(), map_tiles);
	_load_sprite(388, 15, (constant::SPRITE_MAP_PATH / "Tile388-402.pak").string(), map_tiles);
	_load_sprite(403, 3, (constant::SPRITE_MAP_PATH / "Tile403-405.pak").string(), map_tiles);
	_load_sprite(406, 16, (constant::SPRITE_MAP_PATH / "Tile406-421.pak").string(), map_tiles);
	_load_sprite(422, 8, (constant::SPRITE_MAP_PATH / "Tile422-429.pak").string(), map_tiles);
	_load_sprite(430, 14, (constant::SPRITE_MAP_PATH / "Tile430-443.pak").string(), map_tiles);
	_load_sprite(444, 1, (constant::SPRITE_MAP_PATH / "Tile444-444.pak").string(), map_tiles);
	_load_sprite(445, 17, (constant::SPRITE_MAP_PATH / "Tile445-461.pak").string(), map_tiles);
	_load_sprite(462, 12, (constant::SPRITE_MAP_PATH / "Tile462-473.pak").string(), map_tiles);
	_load_sprite(474, 15, (constant::SPRITE_MAP_PATH / "Tile474-488.pak").string(), map_tiles);
	_load_sprite(489, 34, (constant::SPRITE_MAP_PATH / "Tile489-522.pak").string(), map_tiles);
	_load_sprite(523, 8, (constant::SPRITE_MAP_PATH / "Tile523-530.pak").string(), map_tiles);
	_load_sprite(531, 10, (constant::SPRITE_MAP_PATH / "Tile531-540.pak").string(), map_tiles);
	_load_sprite(541, 5, (constant::SPRITE_MAP_PATH / "Tile541-545.pak").string(), map_tiles);

	CMapLoader::register_map_file(MapID::middle2nd, constant::MAPDATA_PATH / "2ndmiddle.mtd");
	CMapLoader::register_map_file(MapID::Abaddon, constant::MAPDATA_PATH / "Abaddon.mtd");
	CMapLoader::register_map_file(MapID::arebrk11, constant::MAPDATA_PATH / "arebrk11.mtd");
	CMapLoader::register_map_file(MapID::arebrk12, constant::MAPDATA_PATH / "arebrk12.mtd");
	CMapLoader::register_map_file(MapID::arebrk21, constant::MAPDATA_PATH / "arebrk21.mtd");
	CMapLoader::register_map_file(MapID::arebrk22, constant::MAPDATA_PATH / "arebrk22.mtd");
	CMapLoader::register_map_file(MapID::arefarm, constant::MAPDATA_PATH / "arefarm.mtd");
	CMapLoader::register_map_file(MapID::arejail, constant::MAPDATA_PATH / "arejail.mtd");
	CMapLoader::register_map_file(MapID::ARESDEN, constant::MAPDATA_PATH / "ARESDEN.mtd");
	CMapLoader::register_map_file(MapID::aresdend1, constant::MAPDATA_PATH / "aresdend1.mtd");
	CMapLoader::register_map_file(MapID::areuni, constant::MAPDATA_PATH / "areuni.mtd");
	CMapLoader::register_map_file(MapID::arewrhus, constant::MAPDATA_PATH / "arewrhus.mtd");
	CMapLoader::register_map_file(MapID::bisle, constant::MAPDATA_PATH / "bisle.mtd");
	CMapLoader::register_map_file(MapID::bsmith_1, constant::MAPDATA_PATH / "bsmith_1.mtd");
	CMapLoader::register_map_file(MapID::bsmith_1f, constant::MAPDATA_PATH / "bsmith_1f.mtd");
	CMapLoader::register_map_file(MapID::bsmith_2, constant::MAPDATA_PATH / "bsmith_2.mtd");
	CMapLoader::register_map_file(MapID::bsmith_2f, constant::MAPDATA_PATH / "bsmith_2f.mtd");
	CMapLoader::register_map_file(MapID::BTField, constant::MAPDATA_PATH / "BTField.mtd");
	CMapLoader::register_map_file(MapID::cath_1, constant::MAPDATA_PATH / "cath_1.mtd");
	CMapLoader::register_map_file(MapID::cath_2, constant::MAPDATA_PATH / "cath_2.mtd");
	CMapLoader::register_map_file(MapID::cityhall_1, constant::MAPDATA_PATH / "cityhall_1.mtd");
	CMapLoader::register_map_file(MapID::cityhall_2, constant::MAPDATA_PATH / "cityhall_2.mtd");
	CMapLoader::register_map_file(MapID::Cmdhall_1, constant::MAPDATA_PATH / "Cmdhall_1.mtd");
	CMapLoader::register_map_file(MapID::Cmdhall_2, constant::MAPDATA_PATH / "Cmdhall_2.mtd");
	CMapLoader::register_map_file(MapID::Default, constant::MAPDATA_PATH / "Default.mtd");
	CMapLoader::register_map_file(MapID::dglv2, constant::MAPDATA_PATH / "dglv2.mtd");
	CMapLoader::register_map_file(MapID::dglv3, constant::MAPDATA_PATH / "dglv3.mtd");
	CMapLoader::register_map_file(MapID::dglv4, constant::MAPDATA_PATH / "dglv4.mtd");
	CMapLoader::register_map_file(MapID::DruncnCity, constant::MAPDATA_PATH / "DruncnCity.mtd");
	CMapLoader::register_map_file(MapID::elvbrk11, constant::MAPDATA_PATH / "elvbrk11.mtd");
	CMapLoader::register_map_file(MapID::elvbrk12, constant::MAPDATA_PATH / "elvbrk12.mtd");
	CMapLoader::register_map_file(MapID::elvbrk21, constant::MAPDATA_PATH / "elvbrk21.mtd");
	CMapLoader::register_map_file(MapID::elvbrk22, constant::MAPDATA_PATH / "elvbrk22.mtd");
	CMapLoader::register_map_file(MapID::elvfarm, constant::MAPDATA_PATH / "elvfarm.mtd");
	CMapLoader::register_map_file(MapID::ELVINE, constant::MAPDATA_PATH / "ELVINE.mtd");
	CMapLoader::register_map_file(MapID::elvined1, constant::MAPDATA_PATH / "elvined1.mtd");
	CMapLoader::register_map_file(MapID::elvjail, constant::MAPDATA_PATH / "elvjail.mtd");
	CMapLoader::register_map_file(MapID::elvuni, constant::MAPDATA_PATH / "elvuni.mtd");
	CMapLoader::register_map_file(MapID::elvwrhus, constant::MAPDATA_PATH / "elvwrhus.mtd");
	CMapLoader::register_map_file(MapID::fightzone1, constant::MAPDATA_PATH / "fightzone1.mtd");
	CMapLoader::register_map_file(MapID::fightzone2, constant::MAPDATA_PATH / "fightzone2.mtd");
	CMapLoader::register_map_file(MapID::fightzone3, constant::MAPDATA_PATH / "fightzone3.mtd");
	CMapLoader::register_map_file(MapID::fightzone4, constant::MAPDATA_PATH / "fightzone4.mtd");
	CMapLoader::register_map_file(MapID::fightzone5, constant::MAPDATA_PATH / "fightzone5.mtd");
	CMapLoader::register_map_file(MapID::fightzone6, constant::MAPDATA_PATH / "fightzone6.mtd");
	CMapLoader::register_map_file(MapID::fightzone7, constant::MAPDATA_PATH / "fightzone7.mtd");
	CMapLoader::register_map_file(MapID::fightzone8, constant::MAPDATA_PATH / "fightzone8.mtd");
	CMapLoader::register_map_file(MapID::fightzone9, constant::MAPDATA_PATH / "fightzone9.mtd");
	CMapLoader::register_map_file(MapID::gldhall_1, constant::MAPDATA_PATH / "gldhall_1.mtd");
	CMapLoader::register_map_file(MapID::gldhall_2, constant::MAPDATA_PATH / "gldhall_2.mtd");
	CMapLoader::register_map_file(MapID::GodH, constant::MAPDATA_PATH / "GodH.mtd");
	CMapLoader::register_map_file(MapID::gshop_1, constant::MAPDATA_PATH / "gshop_1.mtd");
	CMapLoader::register_map_file(MapID::gshop_1f, constant::MAPDATA_PATH / "gshop_1f.mtd");
	CMapLoader::register_map_file(MapID::gshop_2, constant::MAPDATA_PATH / "gshop_2.mtd");
	CMapLoader::register_map_file(MapID::gshop_2f, constant::MAPDATA_PATH / "gshop_2f.mtd");
	CMapLoader::register_map_file(MapID::HRampart, constant::MAPDATA_PATH / "HRampart.mtd");
	CMapLoader::register_map_file(MapID::huntzone1, constant::MAPDATA_PATH / "huntzone1.mtd");
	CMapLoader::register_map_file(MapID::huntzone2, constant::MAPDATA_PATH / "huntzone2.mtd");
	CMapLoader::register_map_file(MapID::huntzone3, constant::MAPDATA_PATH / "huntzone3.mtd");
	CMapLoader::register_map_file(MapID::huntzone4, constant::MAPDATA_PATH / "huntzone4.mtd");
	CMapLoader::register_map_file(MapID::icebound, constant::MAPDATA_PATH / "icebound.mtd");
	CMapLoader::register_map_file(MapID::inferniaA, constant::MAPDATA_PATH / "inferniaA.mtd");
	CMapLoader::register_map_file(MapID::inferniaB, constant::MAPDATA_PATH / "inferniaB.mtd");
	CMapLoader::register_map_file(MapID::maze, constant::MAPDATA_PATH / "maze.mtd");
	CMapLoader::register_map_file(MapID::middled1n, constant::MAPDATA_PATH / "middled1n.mtd");
	CMapLoader::register_map_file(MapID::middled1x, constant::MAPDATA_PATH / "middled1x.mtd");
	CMapLoader::register_map_file(MapID::middleland, constant::MAPDATA_PATH / "middleland.mtd");
	CMapLoader::register_map_file(MapID::procella, constant::MAPDATA_PATH / "procella.mtd");
	CMapLoader::register_map_file(MapID::Resurr1, constant::MAPDATA_PATH / "Resurr1.mtd");
	CMapLoader::register_map_file(MapID::Resurr2, constant::MAPDATA_PATH / "Resurr2.mtd");
	CMapLoader::register_map_file(MapID::Toh1, constant::MAPDATA_PATH / "Toh1.mtd");
	CMapLoader::register_map_file(MapID::Toh2, constant::MAPDATA_PATH / "Toh2.mtd");
	CMapLoader::register_map_file(MapID::Toh3, constant::MAPDATA_PATH / "Toh3.mtd");
	CMapLoader::register_map_file(MapID::wrhus_1, constant::MAPDATA_PATH / "wrhus_1.mtd");
	CMapLoader::register_map_file(MapID::wrhus_1f, constant::MAPDATA_PATH / "wrhus_1f.mtd");
	CMapLoader::register_map_file(MapID::wrhus_2, constant::MAPDATA_PATH / "wrhus_2.mtd");
	CMapLoader::register_map_file(MapID::wrhus_2f, constant::MAPDATA_PATH / "wrhus_2f.mtd");
	CMapLoader::register_map_file(MapID::wzdtwr_1, constant::MAPDATA_PATH / "wzdtwr_1.mtd");
	CMapLoader::register_map_file(MapID::wzdtwr_2, constant::MAPDATA_PATH / "wzdtwr_2.mtd");
}

#include "Scenes.h"

void CreateCharacterScene::on_initialize()
{
	created_appearance = created_character.get_appearance();
	created_appearance.gender = (GetRandomValue(0, 1) == 0 ? GENDER_MALE : GENDER_FEMALE);
	created_appearance.hair_style = static_cast<HairStyle>(GetRandomValue(0, HairStyle::HAIR_STYLE_COUNT - 1));
	created_appearance.hair_color_index = static_cast<HairColorIndex>(GetRandomValue(0, HairColorCount - 1));
	created_appearance.skin_color_index = static_cast<SkinColorIndex>(GetRandomValue(0, SkinColorCount - 1));
	created_appearance.underwear_color_index = static_cast<UnderwearColorIndex>(GetRandomValue(0, SkinColorCount - 1));
	created_character.set_direction((Dir)GetRandomValue(0, Dir::DIRECTION_COUNT - 1));
	created_character.update_appearance(created_appearance);
}

void CreateCharacterScene::on_uninitialize()
{
}

void CreateCharacterScene::on_update()
{
	GamePosition Pos{};
	Pos.set_pixel_position(500, 168);
	created_character.set_position(Pos);
	if (created_character.get_current_animation_type() != AnimationType::WALK)
		created_character.set_animation(AnimationType::WALK, WeaponUsed::HAND);
	static double DirectionTimer = GetTime();
	static double LastInterval = 5.0;
	if (rlx::HasElapsed(DirectionTimer, LastInterval))
	{
		created_character.set_direction((Dir)GetRandomValue(0, Dir::DIRECTION_COUNT - 1));
		DirectionTimer = GetTime();
		LastInterval = static_cast<double>(GetRandomValue(5, 10));
	}

	created_character.update();
}

void CreateCharacterScene::on_render()
{
	sprites[SPRID_CREATECHARARCTERSCREEN]->draw(0, 0, SPR_CREATECHARARCTERSCREEN::BACKGROUND);

	created_character.on_render();
}

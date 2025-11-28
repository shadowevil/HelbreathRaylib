#include "Scenes.h"

void CreateCharacterScene::OnInitialize()
{
	created_appearance = &created_character.GetAppearance();
	created_appearance->gender = (GetRandomValue(0, 1) == 0 ? GENDER_MALE : GENDER_FEMALE);
	created_appearance->hair_style = static_cast<HairStyle>(GetRandomValue(0, HairStyle::HAIR_STYLE_COUNT - 1));
	created_appearance->hair_color_index = static_cast<HairColorIndex>(GetRandomValue(0, HairColorCount - 1));
	created_appearance->skin_color_index = static_cast<SkinColorIndex>(GetRandomValue(0, SkinColorCount - 1));
	created_appearance->underwear_color_index = static_cast<UnderwearColorIndex>(GetRandomValue(0, SkinColorCount - 1));
	created_character.SetDirection((Dir)GetRandomValue(0, Dir::DIRECTION_COUNT - 1));
	created_character.UpdateAppearance(*created_appearance);
}

void CreateCharacterScene::OnUninitialize()
{
}

void CreateCharacterScene::OnUpdate()
{
	created_character.Update();
}

void CreateCharacterScene::OnRender()
{
	m_sprites[SPRID_CREATECHARARCTERSCREEN]->Draw(0, 0, SPR_CREATECHARARCTERSCREEN::BACKGROUND);

	GamePosition pos{};
	pos.set_pixel_position(500, 168);
	created_character.SetPosition(pos);
	if (created_character.GetCurrentAnimationType() != AnimationType::WALK)
		created_character.SetAnimation(AnimationType::WALK, WeaponUsed::HAND);
	static double direction_timer = GetTime();
	static double last_interval = 5.0;
	if(rlx::HasElapsed(direction_timer, last_interval))
	{
		created_character.SetDirection((Dir)GetRandomValue(0, Dir::DIRECTION_COUNT - 1));
		direction_timer = GetTime();
		last_interval = static_cast<double>(GetRandomValue(5, 10));
	}

	created_character.OnRender();
}

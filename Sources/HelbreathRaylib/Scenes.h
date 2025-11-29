#pragma once
// Includes for scenes
#include "LoadingScene.h"
#include "MainMenuScene.h"
#include "ExitGameScene.h"

//#include "TestScene.h"
#ifndef TEST_SCENE_ENABLED
#include "MainGameScene.h"
#endif

#include "CharacterSelectionScene.h"
#include "CreateCharacterScene.h"

// Other necessary includes
#include "PAK.h"
#include "Sprite.h"
#include "SceneManager.h"
#include "FontSystem.h"
#include "Application.h"
#include "IDs.h"
#include "stdafx.h"
#include "GameOver.h"
#include "Game.h"
#include "GameFlow.h"
#include "Title.h"
#include "Fade.h"
#include "SoundList.h"
#include "ObjectNames.h"

namespace
{
	const float SCREEN_WIDTH = 1920.0f;
	const float SCREEN_HEIGHT = 1080.0f;
	const Vector4 BACKGROUND_COLOR = { 0.08f, 0.02f, 0.02f, 1.0f };
	const Vector3 TITLE_POSITION = { -330.0f, 160.0f, 0.0f };
	const float TITLE_SCALE = 3.0f;
	const Vector4 TITLE_COLOR = { 1.0f, 0.2f, 0.2f, 1.0f };
	const float MENU_X = -110.0f;
	const float MENU_TOP_Y = -60.0f;
	const float MENU_SPACING = 80.0f;
	const float MENU_SCALE = 1.1f;
	const float CURSOR_OFFSET_X = -60.0f;
	const Vector4 SELECTED_COLOR = { 1.0f, 0.85f, 0.4f, 1.0f };
	const Vector4 UNSELECTED_COLOR = { 0.55f, 0.5f, 0.5f, 1.0f };
	const float INPUT_WAIT_TIME = 1.0f;
	const float STICK_THRESHOLD = 0.5f;
	const float STICK_RELEASE = 0.3f;
	const wchar_t* const MENU_LABELS[] = {
		L"リトライ",
		L"タイトルへ",
	};
}

GameOver::GameOver()
{
}

GameOver::~GameOver()
{
}

bool GameOver::Start()
{
	m_fade = Fade::Get();

	m_background.Init("Assets/sprite/UI_White.DDS", SCREEN_WIDTH, SCREEN_HEIGHT);
	m_background.SetMulColor(BACKGROUND_COLOR);
	m_background.Update();

	m_titleText.SetText(L"GAME OVER");
	m_titleText.SetPosition(TITLE_POSITION);
	m_titleText.SetScale(TITLE_SCALE);
	m_titleText.SetColor(TITLE_COLOR);

	for (int i = 0; i < enMenu_Num; i++) {
		m_menuText[i].SetText(MENU_LABELS[i]);
		m_menuText[i].SetPosition({ MENU_X, MENU_TOP_Y - MENU_SPACING * i, 0.0f });
		m_menuText[i].SetScale(MENU_SCALE);
	}
	m_cursorText.SetText(L">");
	m_cursorText.SetScale(MENU_SCALE);
	m_cursorText.SetColor(SELECTED_COLOR);
	UpdateMenuText();
	return true;
}

void GameOver::Update()
{
	if (m_isDecided) {
		if (m_fade->IsBlack()) {
			if (m_select == enMenu_Retry) {
				GameFlow::Get()->Retry();
				NewGO<Game>(0, ObjectName::GAME);
			}
			else {
				NewGO<Title>(0, ObjectName::TITLE);
			}
			m_fade->StartFadeIn();
			DeleteGO(this);
		}
		return;
	}

	m_timer += g_gameTime->GetFrameDeltaTime();
	if (m_timer < INPUT_WAIT_TIME || m_fade->IsFading()) {
		return;
	}

	UpdateSelect();
	if (g_pad[0]->IsTrigger(enButtonA)) {
		m_isDecided = true;
		m_fade->StartFadeOut();
		SoundList::Play(SoundId::MenuDecide, SoundList::MENU_VOLUME);
	}
}

void GameOver::UpdateSelect()
{
	int move = 0;
	if (g_pad[0]->IsTrigger(enButtonUp)) {
		move = -1;
	}
	if (g_pad[0]->IsTrigger(enButtonDown)) {
		move = 1;
	}

	float stickY = g_pad[0]->GetLStickYF();
	if (!m_isStickTilted && fabsf(stickY) >= STICK_THRESHOLD) {
		m_isStickTilted = true;
		move = stickY > 0.0f ? -1 : 1;
	}
	else if (fabsf(stickY) < STICK_RELEASE) {
		m_isStickTilted = false;
	}

	if (move == 0) {
		return;
	}
	m_select = (m_select + move + enMenu_Num) % enMenu_Num;
	SoundList::Play(SoundId::MenuCursor, SoundList::MENU_VOLUME);
	UpdateMenuText();
}

void GameOver::UpdateMenuText()
{
	for (int i = 0; i < enMenu_Num; i++) {
		m_menuText[i].SetColor(i == m_select ? SELECTED_COLOR : UNSELECTED_COLOR);
	}
	m_cursorText.SetPosition({ MENU_X + CURSOR_OFFSET_X, MENU_TOP_Y - MENU_SPACING * m_select, 0.0f });
}

void GameOver::Render(RenderContext& rc)
{
	m_background.Draw(rc);
	m_titleText.Draw(rc);
	if (m_timer < INPUT_WAIT_TIME) {
		return;
	}
	for (int i = 0; i < enMenu_Num; i++) {
		m_menuText[i].Draw(rc);
	}
	m_cursorText.Draw(rc);
}

#include "stdafx.h"
#include "GameOver.h"
#include "Game.h"
#include "Fade.h"

namespace
{
	const float SCREEN_WIDTH = 1920.0f;
	const float SCREEN_HEIGHT = 1080.0f;
	const Vector4 BACKGROUND_COLOR = { 0.08f, 0.02f, 0.02f, 1.0f };
	const Vector3 TITLE_POSITION = { -330.0f, 120.0f, 0.0f };
	const float TITLE_SCALE = 3.0f;
	const Vector3 RETRY_POSITION = { -150.0f, -80.0f, 0.0f };
	const float RETRY_SCALE = 1.2f;
	const Vector4 TITLE_COLOR = { 1.0f, 0.2f, 0.2f, 1.0f };
	const float INPUT_WAIT_TIME = 1.0f;
}

GameOver::GameOver()
{
}

GameOver::~GameOver()
{
}

bool GameOver::Start()
{
	m_fade = FindGO<Fade>("fade");

	m_background.Init("Assets/sprite/UI_White.DDS", SCREEN_WIDTH, SCREEN_HEIGHT);
	m_background.SetMulColor(BACKGROUND_COLOR);
	m_background.Update();

	m_titleText.SetText(L"GAME OVER");
	m_titleText.SetPosition(TITLE_POSITION);
	m_titleText.SetScale(TITLE_SCALE);
	m_titleText.SetColor(TITLE_COLOR);

	m_retryText.SetText(L"A : RETRY");
	m_retryText.SetPosition(RETRY_POSITION);
	m_retryText.SetScale(RETRY_SCALE);
	return true;
}

void GameOver::Update()
{
	if (m_isRetrying) {
		if (m_fade->IsBlack()) {
			NewGO<Game>(0, "game");
			m_fade->StartFadeIn();
			DeleteGO(this);
		}
		return;
	}

	m_timer += g_gameTime->GetFrameDeltaTime();
	if (m_timer < INPUT_WAIT_TIME || m_fade->IsFading()) {
		return;
	}
	if (g_pad[0]->IsTrigger(enButtonA)) {
		m_isRetrying = true;
		m_fade->StartFadeOut();
	}
}

void GameOver::Render(RenderContext& rc)
{
	m_background.Draw(rc);
	m_titleText.Draw(rc);
	if (m_timer >= INPUT_WAIT_TIME) {
		m_retryText.Draw(rc);
	}
}

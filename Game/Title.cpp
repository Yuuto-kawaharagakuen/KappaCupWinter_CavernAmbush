#include "stdafx.h"
#include "Title.h"
#include "Game.h"
#include "GameFlow.h"
#include "Fade.h"
#include "ObjectNames.h"

namespace
{
	const float SCREEN_WIDTH = 1920.0f;
	const float SCREEN_HEIGHT = 1080.0f;
	const Vector4 BACKGROUND_COLOR = { 0.09f, 0.07f, 0.04f, 1.0f };
	const Vector3 TITLE_POSITION = { -520.0f, 160.0f, 0.0f };
	const float TITLE_SCALE = 3.2f;
	const Vector4 TITLE_COLOR = { 0.95f, 0.78f, 0.45f, 1.0f };
	const Vector3 START_POSITION = { -150.0f, -140.0f, 0.0f };
	const float START_SCALE = 1.2f;
	const float INPUT_WAIT_TIME = 0.5f;
	const float BLINK_SPEED = 3.0f;
	const float BLINK_MIN_ALPHA = 0.25f;
}

Title::Title()
{
}

Title::~Title()
{
}

bool Title::Start()
{
	m_fade = Fade::Get();

	m_background.Init("Assets/sprite/UI_White.DDS", SCREEN_WIDTH, SCREEN_HEIGHT);
	m_background.SetMulColor(BACKGROUND_COLOR);
	m_background.Update();

	m_titleText.SetText(L"CAVERN AMBUSH");
	m_titleText.SetPosition(TITLE_POSITION);
	m_titleText.SetScale(TITLE_SCALE);
	m_titleText.SetColor(TITLE_COLOR);

	m_startText.SetText(L"A : START");
	m_startText.SetPosition(START_POSITION);
	m_startText.SetScale(START_SCALE);
	return true;
}

void Title::Update()
{
	m_timer += g_gameTime->GetFrameDeltaTime();

	float wave = (sinf(m_timer * BLINK_SPEED) + 1.0f) * 0.5f;
	float alpha = BLINK_MIN_ALPHA + (1.0f - BLINK_MIN_ALPHA) * wave;
	m_startText.SetColor({ 1.0f, 1.0f, 1.0f, m_isStarting ? 1.0f : alpha });

	if (m_isStarting) {
		if (m_fade->IsBlack()) {
			GameFlow::Get()->StartNewGame();
			NewGO<Game>(0, ObjectName::GAME);
			m_fade->StartFadeIn();
			DeleteGO(this);
		}
		return;
	}

	if (m_timer < INPUT_WAIT_TIME || m_fade->IsFading()) {
		return;
	}
	if (g_pad[0]->IsTrigger(enButtonA) || g_pad[0]->IsTrigger(enButtonStart)) {
		m_isStarting = true;
		m_fade->StartFadeOut();
	}
}

void Title::Render(RenderContext& rc)
{
	m_background.Draw(rc);
	m_titleText.Draw(rc);
	m_startText.Draw(rc);
}

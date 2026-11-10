#include "stdafx.h"
#include "StageClear.h"
#include "Game.h"
#include "Fade.h"
#include "ObjectNames.h"

namespace
{
	const float SCREEN_WIDTH = 1920.0f;
	const float SCREEN_HEIGHT = 1080.0f;
	const Vector4 BACKGROUND_COLOR = { 0.07f, 0.05f, 0.03f, 1.0f };
	const Vector3 TITLE_POSITION = { -410.0f, 180.0f, 0.0f };
	const float TITLE_SCALE = 3.0f;
	const Vector4 TITLE_COLOR = { 1.0f, 0.8f, 0.35f, 1.0f };
	const Vector3 TIME_POSITION = { -200.0f, 20.0f, 0.0f };
	const float TIME_SCALE = 1.4f;
	const Vector3 RETRY_POSITION = { -150.0f, -120.0f, 0.0f };
	const float RETRY_SCALE = 1.2f;
	const float INPUT_WAIT_TIME = 1.0f;
	const int SECONDS_PER_MINUTE = 60;
}

StageClear::StageClear()
{
}

StageClear::~StageClear()
{
}

bool StageClear::Start()
{
	m_fade = FindGO<Fade>(ObjectName::FADE);

	m_background.Init("Assets/sprite/UI_White.DDS", SCREEN_WIDTH, SCREEN_HEIGHT);
	m_background.SetMulColor(BACKGROUND_COLOR);
	m_background.Update();

	m_titleText.SetText(L"STAGE CLEAR");
	m_titleText.SetPosition(TITLE_POSITION);
	m_titleText.SetScale(TITLE_SCALE);
	m_titleText.SetColor(TITLE_COLOR);

	int minutes = static_cast<int>(m_clearTime) / SECONDS_PER_MINUTE;
	float seconds = m_clearTime - static_cast<float>(minutes * SECONDS_PER_MINUTE);
	wchar_t text[32];
	swprintf_s(text, L"TIME  %02d:%05.2f", minutes, seconds);
	m_timeText.SetText(text);
	m_timeText.SetPosition(TIME_POSITION);
	m_timeText.SetScale(TIME_SCALE);

	m_retryText.SetText(L"A : RETRY");
	m_retryText.SetPosition(RETRY_POSITION);
	m_retryText.SetScale(RETRY_SCALE);
	return true;
}

void StageClear::Update()
{
	if (m_isRetrying) {
		if (m_fade->IsBlack()) {
			NewGO<Game>(0, ObjectName::GAME);
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

void StageClear::Render(RenderContext& rc)
{
	m_background.Draw(rc);
	m_titleText.Draw(rc);
	m_timeText.Draw(rc);
	if (m_timer >= INPUT_WAIT_TIME) {
		m_retryText.Draw(rc);
	}
}

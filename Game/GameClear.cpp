#include "stdafx.h"
#include "GameClear.h"
#include "GameFlow.h"
#include "Title.h"
#include "Fade.h"
#include "ObjectNames.h"

namespace
{
	const float SCREEN_WIDTH = 1920.0f;
	const float SCREEN_HEIGHT = 1080.0f;
	const Vector4 BACKGROUND_COLOR = { 0.07f, 0.05f, 0.03f, 1.0f };
	const Vector3 TITLE_POSITION = { -380.0f, 200.0f, 0.0f };
	const float TITLE_SCALE = 3.0f;
	const Vector4 TITLE_COLOR = { 1.0f, 0.8f, 0.35f, 1.0f };
	const Vector3 TIME_POSITION = { -220.0f, 40.0f, 0.0f };
	const float TIME_SCALE = 1.4f;
	const Vector3 RETRY_COUNT_POSITION = { -220.0f, -40.0f, 0.0f };
	const float RETRY_COUNT_SCALE = 1.0f;
	const Vector4 RETRY_COUNT_COLOR = { 0.8f, 0.75f, 0.65f, 1.0f };
	const Vector3 NEXT_POSITION = { -190.0f, -180.0f, 0.0f };
	const float NEXT_SCALE = 1.1f;
	const float INPUT_WAIT_TIME = 1.0f;
	const int SECONDS_PER_MINUTE = 60;
}

GameClear::GameClear()
{
}

GameClear::~GameClear()
{
}

bool GameClear::Start()
{
	m_fade = Fade::Get();
	GameFlow* flow = GameFlow::Get();

	m_background.Init("Assets/sprite/UI_White.DDS", SCREEN_WIDTH, SCREEN_HEIGHT);
	m_background.SetMulColor(BACKGROUND_COLOR);
	m_background.Update();

	m_titleText.SetText(L"GAME CLEAR");
	m_titleText.SetPosition(TITLE_POSITION);
	m_titleText.SetScale(TITLE_SCALE);
	m_titleText.SetColor(TITLE_COLOR);

	float totalTime = flow->GetTotalTime();
	int minutes = static_cast<int>(totalTime) / SECONDS_PER_MINUTE;
	float seconds = totalTime - static_cast<float>(minutes * SECONDS_PER_MINUTE);
	wchar_t text[32];
	swprintf_s(text, L"TIME  %02d:%05.2f", minutes, seconds);
	m_timeText.SetText(text);
	m_timeText.SetPosition(TIME_POSITION);
	m_timeText.SetScale(TIME_SCALE);

	swprintf_s(text, L"RETRY  %d", flow->GetRetryCount());
	m_retryCountText.SetText(text);
	m_retryCountText.SetPosition(RETRY_COUNT_POSITION);
	m_retryCountText.SetScale(RETRY_COUNT_SCALE);
	m_retryCountText.SetColor(RETRY_COUNT_COLOR);

	m_nextText.SetText(L"A : タイトルへ");
	m_nextText.SetPosition(NEXT_POSITION);
	m_nextText.SetScale(NEXT_SCALE);
	return true;
}

void GameClear::Update()
{
	if (m_isDecided) {
		if (m_fade->IsBlack()) {
			NewGO<Title>(0, ObjectName::TITLE);
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
		m_isDecided = true;
		m_fade->StartFadeOut();
	}
}

void GameClear::Render(RenderContext& rc)
{
	m_background.Draw(rc);
	m_titleText.Draw(rc);
	m_timeText.Draw(rc);
	m_retryCountText.Draw(rc);
	if (m_timer >= INPUT_WAIT_TIME) {
		m_nextText.Draw(rc);
	}
}

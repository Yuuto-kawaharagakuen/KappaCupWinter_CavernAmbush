#include "stdafx.h"
#include "Fade.h"
#include "ObjectNames.h"

namespace
{
	const float FADE_TIME = 0.8f;
	const float SCREEN_WIDTH = 1920.0f;
	const float SCREEN_HEIGHT = 1080.0f;
	const int FADE_PRIORITY = 10;
}

Fade* Fade::Get()
{
	Fade* fade = FindGO<Fade>(ObjectName::FADE);
	if (fade == nullptr) {
		fade = NewGO<Fade>(FADE_PRIORITY, ObjectName::FADE);
	}
	return fade;
}

Fade::Fade()
{
}

Fade::~Fade()
{
}

bool Fade::Start()
{
	m_sprite.Init("Assets/sprite/UI_White.DDS", SCREEN_WIDTH, SCREEN_HEIGHT);
	m_sprite.SetMulColor({ 0.0f, 0.0f, 0.0f, m_alpha });
	m_sprite.Update();
	return true;
}

void Fade::StartFadeOut()
{
	m_state = enState_FadeOut;
}

void Fade::StartFadeIn()
{
	m_state = enState_FadeIn;
}

void Fade::Update()
{
	float step = g_gameTime->GetFrameDeltaTime() / FADE_TIME;
	if (m_state == enState_FadeOut) {
		m_alpha += step;
		if (m_alpha >= 1.0f) {
			m_alpha = 1.0f;
			m_state = enState_Idle;
		}
	}
	else if (m_state == enState_FadeIn) {
		m_alpha -= step;
		if (m_alpha <= 0.0f) {
			m_alpha = 0.0f;
			m_state = enState_Idle;
		}
	}
	m_sprite.SetMulColor({ 0.0f, 0.0f, 0.0f, m_alpha });
	m_sprite.Update();
}

void Fade::Render(RenderContext& rc)
{
	if (m_alpha > 0.0f) {
		m_sprite.Draw(rc);
	}
}

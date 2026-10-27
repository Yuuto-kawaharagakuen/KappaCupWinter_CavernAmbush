#include "stdafx.h"
#include "PlayerUI.h"
#include "Player.h"

namespace
{
	const float CROSSHAIR_SIZE = 48.0f;
	const Vector3 AMMO_TEXT_POSITION = { 450.0f, -380.0f, 0.0f };
	const float AMMO_TEXT_SCALE = 1.5f;
}

PlayerUI::PlayerUI()
{
}

PlayerUI::~PlayerUI()
{
}

bool PlayerUI::Start()
{
	m_crosshair.Init("Assets/sprite/Crosshair.DDS", CROSSHAIR_SIZE, CROSSHAIR_SIZE);
	m_crosshair.SetPosition(Vector3::Zero);
	m_crosshair.Update();

	m_ammoText.SetPosition(AMMO_TEXT_POSITION);
	m_ammoText.SetScale(AMMO_TEXT_SCALE);
	return true;
}

void PlayerUI::Update()
{
	if (m_player == nullptr) {
		return;
	}

	m_isCrosshairVisible = !m_player->IsDiving() && !m_player->IsSwitching();

	wchar_t text[64];
	if (m_player->IsReloading()) {
		swprintf_s(text, L"RELOAD %d / %d", m_player->GetAmmo(), m_player->GetMaxAmmo());
	}
	else {
		swprintf_s(text, L"  AMMO %d / %d", m_player->GetAmmo(), m_player->GetMaxAmmo());
	}
	m_ammoText.SetText(text);
}

void PlayerUI::Render(RenderContext& rc)
{
	if (m_isCrosshairVisible) {
		m_crosshair.Draw(rc);
	}
	m_ammoText.Draw(rc);
}

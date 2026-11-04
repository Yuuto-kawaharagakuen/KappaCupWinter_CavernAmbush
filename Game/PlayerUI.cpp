#include "stdafx.h"
#include "PlayerUI.h"
#include "Player.h"

namespace
{
	const Vector4 WHITE = { 1.0f, 1.0f, 1.0f, 1.0f };
	const Vector4 TEXT_SHADOW_COLOR = { 0.0f, 0.0f, 0.0f, 0.55f };
	const float TEXT_SHADOW_OFFSET = 2.0f;
	const float DIGIT_WIDTH = 33.0f;

	const Vector3 VIGNETTE_POSITION = { 0.0f, 0.0f, 0.0f };
	const Vector2 VIGNETTE_SIZE = { 1920.0f, 1080.0f };
	const Vector4 VIGNETTE_COLOR = { 0.85f, 0.06f, 0.04f, 1.0f };
	const float DAMAGE_FLASH_DECAY = 3.0f;
	const float DAMAGE_FLASH_ALPHA = 0.75f;
	const float LOW_HP_ALPHA = 0.28f;
	const float LOW_HP_PULSE_SPEED = 5.0f;

	const Vector3 HP_SHADE_POSITION = { -700.0f, -430.0f, 0.0f };
	const Vector3 AMMO_SHADE_POSITION = { 700.0f, -430.0f, 0.0f };
	const Vector2 SHADE_SIZE = { 760.0f, 320.0f };
	const Vector4 SHADE_COLOR = { 1.0f, 1.0f, 1.0f, 0.35f };

	const Vector3 HP_LABEL_POSITION = { -900.0f, -404.0f, 0.0f };
	const float HP_LABEL_SCALE = 0.55f;
	const Vector4 HP_LABEL_COLOR = { 1.0f, 1.0f, 1.0f, 0.7f };
	const Vector3 HP_VALUE_POSITION = { -852.0f, -384.0f, 0.0f };
	const float HP_VALUE_SCALE = 1.2f;
	const Vector3 HP_BAR_POSITION = { -900.0f, -462.0f, 0.0f };
	const Vector2 HP_BAR_SIZE = { 340.0f, 12.0f };
	const Vector4 HP_BAR_BACK_COLOR = { 0.0f, 0.0f, 0.0f, 0.45f };
	const Vector4 HP_BAR_LAG_COLOR = { 1.0f, 0.85f, 0.75f, 0.85f };
	const Vector4 HP_BAR_COLOR = { 0.95f, 0.97f, 0.92f, 1.0f };
	const Vector4 HP_BAR_LOW_COLOR = { 1.0f, 0.3f, 0.22f, 1.0f };
	const float HP_LOW_RATE = 0.3f;
	const float HP_RATE_SPEED = 14.0f;
	const float HP_LAG_DELAY = 0.45f;
	const float HP_LAG_SPEED = 0.8f;
	const float HP_SHAKE_DECAY = 6.0f;
	const float HP_SHAKE_POWER = 9.0f;

	const float AMMO_PIP_RIGHT = 900.0f;
	const float AMMO_PIP_Y = -466.0f;
	const float AMMO_PIP_SPACING = 19.0f;
	const Vector2 AMMO_PIP_SIZE = { 16.0f, 32.0f };
	const Vector4 AMMO_PIP_COLOR = { 1.0f, 0.93f, 0.78f, 1.0f };
	const Vector4 AMMO_PIP_EMPTY_COLOR = { 1.0f, 1.0f, 1.0f, 0.16f };
	const float AMMO_PIP_POP_TIME = 0.18f;
	const float AMMO_VALUE_RIGHT = 826.0f;
	const float AMMO_VALUE_Y = -350.0f;
	const float AMMO_VALUE_SCALE = 1.9f;
	const Vector4 AMMO_LOW_COLOR = { 1.0f, 0.55f, 0.25f, 1.0f };
	const Vector4 AMMO_EMPTY_COLOR = { 1.0f, 0.28f, 0.22f, 1.0f };
	const int AMMO_LOW_NUM = 3;
	const Vector3 AMMO_MAX_POSITION = { 836.0f, -394.0f, 0.0f };
	const float AMMO_MAX_SCALE = 0.7f;
	const Vector4 AMMO_MAX_COLOR = { 1.0f, 1.0f, 1.0f, 0.65f };
	const float AMMO_PUNCH_DECAY = 10.0f;
	const float AMMO_PUNCH_SCALE = 0.18f;
	const Vector3 AMMO_HINT_POSITION = { -150.0f, -70.0f, 0.0f };
	const float AMMO_HINT_SCALE = 0.6f;
	const float AMMO_HINT_BLINK_SPEED = 6.0f;

	const Vector2 CROSS_LINE_SIZE = { 8.0f, 20.0f };
	const float CROSS_GAP = 11.0f;
	const float CROSS_SPREAD_GAP = 16.0f;
	const float CROSS_SPREAD_DECAY = 9.0f;
	const float CROSS_MOVE_SPREAD = 0.35f;
	const Vector2 CROSS_DOT_SIZE = { 12.0f, 12.0f };
	const Vector2 HIT_LINE_SIZE = { 8.0f, 24.0f };
	const float HIT_GAP = 15.0f;
	const float HIT_TIME = 0.22f;
	const float HIT_POP = 8.0f;
	const Vector4 HIT_COLOR = { 1.0f, 1.0f, 1.0f, 1.0f };
	const Vector4 HIT_KILL_COLOR = { 1.0f, 0.25f, 0.2f, 1.0f };

	const Vector3 STATUS_PILL_POSITION = { 0.0f, 470.0f, 0.0f };
	const Vector2 STATUS_PILL_SIZE = { 300.0f, 46.0f };
	const Vector3 STATUS_DIVE_POSITION = { -120.0f, 486.0f, 0.0f };
	const Vector3 STATUS_NO_SHOT_POSITION = { -4.0f, 486.0f, 0.0f };
	const float STATUS_TEXT_SCALE = 0.62f;
	const Vector4 STATUS_DIVE_COLOR = { 1.0f, 0.8f, 0.2f, 1.0f };
	const Vector4 STATUS_NO_SHOT_COLOR = { 1.0f, 1.0f, 1.0f, 0.8f };
	const float STATUS_FADE_SPEED = 6.0f;
	const float STATUS_SLIDE = 30.0f;

	const float RING_SIZE = 170.0f;
	const float RING_SEGMENT_ANGLE = 36.0f;
	const Vector4 RING_FILLED_COLOR = { 0.45f, 0.8f, 1.0f, 1.0f };
	const Vector4 RING_EMPTY_COLOR = { 1.0f, 1.0f, 1.0f, 0.18f };
	const float RING_POP_TIME = 0.2f;
	const float RING_POP_SCALE = 0.12f;
	const Vector3 RELOAD_LABEL_POSITION = { -62.0f, -96.0f, 0.0f };
	const float RELOAD_LABEL_SCALE = 0.55f;
	const Vector3 RELOAD_COUNT_POSITION = { -60.0f, -122.0f, 0.0f };
	const float RELOAD_COUNT_SCALE = 0.55f;
	const float RELOAD_FADE_SPEED = 8.0f;

	const float POPUP_LIFE = 0.85f;
	const float POPUP_FADE_START = 0.45f;
	const float POPUP_RISE = 70.0f;
	const float POPUP_SCALE = 0.85f;
	const float POPUP_KILL_SCALE = 1.15f;
	const float POPUP_POP_TIME = 0.1f;
	const Vector4 POPUP_COLOR = { 1.0f, 0.86f, 0.35f, 1.0f };
	const Vector4 POPUP_KILL_COLOR = { 1.0f, 0.4f, 0.25f, 1.0f };
	const float POPUP_HEIGHT_OFFSET = 30.0f;

	Vector3 MakePosition(float x, float y)
	{
		return { x, y, 0.0f };
	}

	Vector4 WithAlpha(const Vector4& color, float alpha)
	{
		return { color.x, color.y, color.z, color.w * alpha };
	}

	float Damp(float current, float target, float speed, float deltaTime)
	{
		return target + (current - target) * expf(-speed * deltaTime);
	}

	float Saturate(float value)
	{
		return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
	}

	int CountDigits(int value)
	{
		int digits = 1;
		while (value >= 10) {
			value /= 10;
			digits++;
		}
		return digits;
	}

	void InitSprite(SpriteRender& sprite, const char* path, const Vector2& size, const Vector3& position)
	{
		sprite.Init(path, size.x, size.y);
		sprite.SetPosition(position);
		sprite.Update();
	}

	void InitText(FontRender& font, const wchar_t* text, const Vector3& position, float scale, const Vector4& color, enFontType fontType)
	{
		font.SetFontType(fontType);
		font.SetText(text);
		font.SetPosition(position);
		font.SetScale(scale);
		font.SetColor(color);
		font.SetShadowParam(true, TEXT_SHADOW_OFFSET, TEXT_SHADOW_COLOR);
	}

	void SetTextAlpha(FontRender& font, const Vector4& color, float alpha)
	{
		font.SetColor(WithAlpha(color, alpha));
		font.SetShadowParam(true, TEXT_SHADOW_OFFSET, WithAlpha(TEXT_SHADOW_COLOR, alpha));
	}
}

PlayerUI::PlayerUI()
{
}

PlayerUI::~PlayerUI()
{
}

bool PlayerUI::Start()
{
	InitSprite(m_vignette, "Assets/sprite/UI_Vignette.DDS", VIGNETTE_SIZE, VIGNETTE_POSITION);
	m_vignette.SetMulColor(WithAlpha(VIGNETTE_COLOR, 0.0f));
	m_vignette.Update();

	InitHp();
	InitAmmo();
	InitCrosshair();
	InitStatus();
	InitReload();

	for (auto& popup : m_damagePopups) {
		InitText(popup.font, L"", Vector3::Zero, POPUP_SCALE, POPUP_COLOR, enFontType::Bold);
	}
	return true;
}

void PlayerUI::InitHp()
{
	InitSprite(m_hpShade, "Assets/sprite/UI_Shade.DDS", SHADE_SIZE, HP_SHADE_POSITION);
	m_hpShade.SetMulColor(SHADE_COLOR);
	m_hpShade.Update();

	SpriteRender* bars[] = { &m_hpBarBack, &m_hpBarLag, &m_hpBar };
	const Vector4 colors[] = { HP_BAR_BACK_COLOR, HP_BAR_LAG_COLOR, HP_BAR_COLOR };
	for (int i = 0; i < 3; i++) {
		bars[i]->Init("Assets/sprite/UI_Bar.DDS", HP_BAR_SIZE.x, HP_BAR_SIZE.y);
		bars[i]->SetPivot({ 0.0f, 0.5f });
		bars[i]->SetPosition(HP_BAR_POSITION);
		bars[i]->SetMulColor(colors[i]);
		bars[i]->Update();
	}

	InitText(m_hpLabel, L"HP", HP_LABEL_POSITION, HP_LABEL_SCALE, HP_LABEL_COLOR, enFontType::Bold);
	InitText(m_hpValue, L"", HP_VALUE_POSITION, HP_VALUE_SCALE, WHITE, enFontType::Bold);
}

void PlayerUI::InitAmmo()
{
	InitSprite(m_ammoShade, "Assets/sprite/UI_Shade.DDS", SHADE_SIZE, AMMO_SHADE_POSITION);
	m_ammoShade.SetMulColor(SHADE_COLOR);
	m_ammoShade.Update();

	for (int i = 0; i < AMMO_PIP_NUM; i++) {
		float x = AMMO_PIP_RIGHT - (AMMO_PIP_NUM - 1 - i) * AMMO_PIP_SPACING;
		InitSprite(m_ammoPips[i], "Assets/sprite/UI_Pip.DDS", AMMO_PIP_SIZE, MakePosition(x, AMMO_PIP_Y));
	}

	InitText(m_ammoValue, L"", MakePosition(AMMO_VALUE_RIGHT, AMMO_VALUE_Y), AMMO_VALUE_SCALE, WHITE, enFontType::Bold);
	InitText(m_ammoMax, L"", AMMO_MAX_POSITION, AMMO_MAX_SCALE, AMMO_MAX_COLOR, enFontType::Bold);
	InitText(m_ammoHint, L"B で潜ってリロード", AMMO_HINT_POSITION, AMMO_HINT_SCALE, AMMO_EMPTY_COLOR, enFontType::Bold);
}

void PlayerUI::InitCrosshair()
{
	for (int i = 0; i < CROSS_LINE_NUM; i++) {
		Quaternion rotation;
		rotation.SetRotationDegZ(90.0f * i);
		m_crossLines[i].Init("Assets/sprite/UI_CrossLine.DDS", CROSS_LINE_SIZE.x, CROSS_LINE_SIZE.y);
		m_crossLines[i].SetRotation(rotation);

		Quaternion hitRotation;
		hitRotation.SetRotationDegZ(45.0f + 90.0f * i);
		m_hitLines[i].Init("Assets/sprite/UI_HitLine.DDS", HIT_LINE_SIZE.x, HIT_LINE_SIZE.y);
		m_hitLines[i].SetRotation(hitRotation);
	}
	InitSprite(m_crossDot, "Assets/sprite/UI_Dot.DDS", CROSS_DOT_SIZE, Vector3::Zero);
}

void PlayerUI::InitStatus()
{
	InitSprite(m_statusPill, "Assets/sprite/UI_Pill.DDS", STATUS_PILL_SIZE, STATUS_PILL_POSITION);
	InitText(m_statusDiveText, L"潜行中", STATUS_DIVE_POSITION, STATUS_TEXT_SCALE, STATUS_DIVE_COLOR, enFontType::Bold);
	InitText(m_statusNoShotText, L"射撃不可", STATUS_NO_SHOT_POSITION, STATUS_TEXT_SCALE, STATUS_NO_SHOT_COLOR, enFontType::Bold);
}

void PlayerUI::InitReload()
{
	InitSprite(m_ringBack, "Assets/sprite/UI_RingBack.DDS", { RING_SIZE, RING_SIZE }, Vector3::Zero);
	for (int i = 0; i < RELOAD_SEGMENT_NUM; i++) {
		Quaternion rotation;
		rotation.SetRotationDegZ(-RING_SEGMENT_ANGLE * i);
		m_ringSegments[i].Init("Assets/sprite/UI_RingSegment.DDS", RING_SIZE, RING_SIZE);
		m_ringSegments[i].SetRotation(rotation);
		m_ringSegments[i].SetMulColor(RING_EMPTY_COLOR);
		m_ringSegments[i].Update();
	}
	InitText(m_reloadLabel, L"RELOAD", RELOAD_LABEL_POSITION, RELOAD_LABEL_SCALE, RING_FILLED_COLOR, enFontType::Bold);
	InitText(m_reloadCount, L"", RELOAD_COUNT_POSITION, RELOAD_COUNT_SCALE, WHITE, enFontType::Bold);
}

void PlayerUI::NotifyHit(const Vector3& worldPosition, int damage, bool isKill)
{
	m_hitTimer = HIT_TIME;
	m_isHitKill = isKill;

	DamagePopup* target = &m_damagePopups[0];
	for (auto& popup : m_damagePopups) {
		if (!popup.isActive) {
			target = &popup;
			break;
		}
		if (popup.timer > target->timer) {
			target = &popup;
		}
	}

	wchar_t text[16];
	swprintf_s(text, L"%d", damage);
	target->font.SetText(text);
	target->worldPosition = worldPosition;
	target->worldPosition.y += POPUP_HEIGHT_OFFSET;
	target->timer = 0.0f;
	target->scale = isKill ? POPUP_KILL_SCALE : POPUP_SCALE;
	target->digits = CountDigits(damage);
	target->font.SetColor(isKill ? POPUP_KILL_COLOR : POPUP_COLOR);
	target->isActive = true;
}

void PlayerUI::Update()
{
	if (m_player == nullptr) {
		return;
	}
	float deltaTime = g_gameTime->GetFrameDeltaTime();

	UpdateVignette(deltaTime);
	UpdateHp(deltaTime);
	UpdateAmmo(deltaTime);
	UpdateCrosshair(deltaTime);
	UpdateStatus(deltaTime);
	UpdateReload(deltaTime);
	UpdateDamagePopups(deltaTime);
}

void PlayerUI::UpdateVignette(float deltaTime)
{
	m_damageFlash = Damp(m_damageFlash, 0.0f, DAMAGE_FLASH_DECAY, deltaTime);

	float alpha = m_damageFlash * DAMAGE_FLASH_ALPHA;
	float rate = static_cast<float>(m_player->GetHp()) / static_cast<float>(m_player->GetMaxHp());
	if (rate <= HP_LOW_RATE) {
		m_lowHpPulse += deltaTime * LOW_HP_PULSE_SPEED;
		alpha += LOW_HP_ALPHA * (0.6f + 0.4f * sinf(m_lowHpPulse));
	}
	m_vignette.SetMulColor(WithAlpha(VIGNETTE_COLOR, Saturate(alpha)));
	m_vignette.Update();
}

void PlayerUI::UpdateHp(float deltaTime)
{
	int hp = m_player->GetHp();
	float targetRate = static_cast<float>(hp) / static_cast<float>(m_player->GetMaxHp());

	if (m_lastHp >= 0 && hp < m_lastHp) {
		m_damageFlash = 1.0f;
		m_hpShake = 1.0f;
		m_hpLagDelay = HP_LAG_DELAY;
	}
	if (m_lastHp < 0) {
		m_hpRate = targetRate;
		m_hpLagRate = targetRate;
	}
	m_lastHp = hp;

	m_hpRate = Damp(m_hpRate, targetRate, HP_RATE_SPEED, deltaTime);
	if (m_hpLagDelay > 0.0f) {
		m_hpLagDelay -= deltaTime;
	}
	else if (m_hpLagRate > m_hpRate) {
		m_hpLagRate -= HP_LAG_SPEED * deltaTime;
	}
	if (m_hpLagRate < m_hpRate) {
		m_hpLagRate = m_hpRate;
	}
	m_hpShake = Damp(m_hpShake, 0.0f, HP_SHAKE_DECAY, deltaTime);

	float shakeX = sinf(m_hpShake * 60.0f) * m_hpShake * HP_SHAKE_POWER;
	Vector3 shake = MakePosition(shakeX, 0.0f);
	bool isLow = targetRate <= HP_LOW_RATE;

	m_hpBarBack.SetPosition(HP_BAR_POSITION + shake);
	m_hpBarBack.Update();
	m_hpBarLag.SetPosition(HP_BAR_POSITION + shake);
	m_hpBarLag.SetScale({ m_hpLagRate, 1.0f, 1.0f });
	m_hpBarLag.Update();
	m_hpBar.SetPosition(HP_BAR_POSITION + shake);
	m_hpBar.SetScale({ m_hpRate, 1.0f, 1.0f });
	m_hpBar.SetMulColor(isLow ? HP_BAR_LOW_COLOR : HP_BAR_COLOR);
	m_hpBar.Update();

	wchar_t text[16];
	swprintf_s(text, L"%d", hp);
	m_hpValue.SetText(text);
	m_hpValue.SetPosition(HP_VALUE_POSITION + shake);
	m_hpValue.SetColor(isLow ? HP_BAR_LOW_COLOR : WHITE);
	m_hpLabel.SetPosition(HP_LABEL_POSITION + shake);
}

void PlayerUI::UpdateAmmo(float deltaTime)
{
	int ammo = m_player->GetAmmo();
	if (m_lastAmmo >= 0 && ammo < m_lastAmmo) {
		for (int i = ammo; i < m_lastAmmo && i < AMMO_PIP_NUM; i++) {
			m_ammoPipPop[i] = AMMO_PIP_POP_TIME;
		}
		m_ammoPunch = 1.0f;
	}
	m_lastAmmo = ammo;
	m_ammoPunch = Damp(m_ammoPunch, 0.0f, AMMO_PUNCH_DECAY, deltaTime);

	for (int i = 0; i < AMMO_PIP_NUM; i++) {
		float x = AMMO_PIP_RIGHT - (AMMO_PIP_NUM - 1 - i) * AMMO_PIP_SPACING;
		float pop = 0.0f;
		if (m_ammoPipPop[i] > 0.0f) {
			m_ammoPipPop[i] -= deltaTime;
			pop = Saturate(m_ammoPipPop[i] / AMMO_PIP_POP_TIME);
		}
		bool isFilled = i < ammo;
		Vector4 color = isFilled ? AMMO_PIP_COLOR : AMMO_PIP_EMPTY_COLOR;
		if (pop > 0.0f) {
			color = WithAlpha(AMMO_PIP_COLOR, pop);
		}
		m_ammoPips[i].SetPosition(MakePosition(x, AMMO_PIP_Y + pop * 14.0f));
		m_ammoPips[i].SetScale({ 1.0f + pop * 0.3f, 1.0f + pop * 0.3f, 1.0f });
		m_ammoPips[i].SetMulColor(color);
		m_ammoPips[i].Update();
	}

	float scale = AMMO_VALUE_SCALE * (1.0f + m_ammoPunch * AMMO_PUNCH_SCALE);
	float width = CountDigits(ammo) * DIGIT_WIDTH * scale;
	float rise = (scale - AMMO_VALUE_SCALE) * 20.0f;
	wchar_t text[16];
	swprintf_s(text, L"%d", ammo);
	m_ammoValue.SetText(text);
	m_ammoValue.SetScale(scale);
	m_ammoValue.SetPosition(MakePosition(AMMO_VALUE_RIGHT - width, AMMO_VALUE_Y + rise));
	if (ammo == 0) {
		m_ammoValue.SetColor(AMMO_EMPTY_COLOR);
	}
	else if (ammo <= AMMO_LOW_NUM) {
		m_ammoValue.SetColor(AMMO_LOW_COLOR);
	}
	else {
		m_ammoValue.SetColor(WHITE);
	}

	swprintf_s(text, L"/%d", m_player->GetMaxAmmo());
	m_ammoMax.SetText(text);

	m_isHintVisible = ammo == 0 && !m_player->IsDiving() && !m_player->IsSwitching();
	if (m_isHintVisible) {
		m_hintBlink += deltaTime * AMMO_HINT_BLINK_SPEED;
		SetTextAlpha(m_ammoHint, AMMO_EMPTY_COLOR, 0.55f + 0.45f * sinf(m_hintBlink));
	}
	else {
		m_hintBlink = 0.0f;
	}
}

void PlayerUI::UpdateCrosshair(float deltaTime)
{
	m_isCrosshairVisible = !m_player->IsDiving() && !m_player->IsSwitching();

	float targetSpread = m_player->IsMoving() ? CROSS_MOVE_SPREAD : 0.0f;
	if (m_player->ConsumeShotFlag()) {
		m_spread = 1.0f;
	}
	m_spread = Damp(m_spread, targetSpread, CROSS_SPREAD_DECAY, deltaTime);

	float gap = CROSS_GAP + m_spread * CROSS_SPREAD_GAP;
	const Vector3 directions[] = { { 0.0f, 1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } };
	for (int i = 0; i < CROSS_LINE_NUM; i++) {
		m_crossLines[i].SetPosition(directions[i] * (gap + CROSS_LINE_SIZE.y * 0.5f));
		m_crossLines[i].Update();
	}
	m_crossDot.Update();

	if (m_hitTimer > 0.0f) {
		m_hitTimer -= deltaTime;
	}
	float hitRate = Saturate(m_hitTimer / HIT_TIME);
	const Vector3 hitDirections[] = { { -0.7071f, 0.7071f, 0.0f }, { -0.7071f, -0.7071f, 0.0f }, { 0.7071f, -0.7071f, 0.0f }, { 0.7071f, 0.7071f, 0.0f } };
	Vector4 hitColor = WithAlpha(m_isHitKill ? HIT_KILL_COLOR : HIT_COLOR, hitRate);
	for (int i = 0; i < CROSS_LINE_NUM; i++) {
		float distance = HIT_GAP + HIT_LINE_SIZE.y * 0.5f + hitRate * HIT_POP;
		m_hitLines[i].SetPosition(hitDirections[i] * distance);
		m_hitLines[i].SetMulColor(hitColor);
		m_hitLines[i].Update();
	}
}

void PlayerUI::UpdateStatus(float deltaTime)
{
	float target = m_player->IsDiving() ? 1.0f : 0.0f;
	m_statusAlpha = Damp(m_statusAlpha, target, STATUS_FADE_SPEED, deltaTime);

	Vector3 slide = MakePosition(0.0f, (1.0f - m_statusAlpha) * STATUS_SLIDE);
	m_statusPill.SetPosition(STATUS_PILL_POSITION + slide);
	m_statusPill.SetMulColor(WithAlpha(WHITE, m_statusAlpha));
	m_statusPill.Update();
	m_statusDiveText.SetPosition(STATUS_DIVE_POSITION + slide);
	SetTextAlpha(m_statusDiveText, STATUS_DIVE_COLOR, m_statusAlpha);
	m_statusNoShotText.SetPosition(STATUS_NO_SHOT_POSITION + slide);
	SetTextAlpha(m_statusNoShotText, STATUS_NO_SHOT_COLOR, m_statusAlpha);
}

void PlayerUI::UpdateReload(float deltaTime)
{
	float target = m_player->IsReloading() ? 1.0f : 0.0f;
	m_reloadAlpha = Damp(m_reloadAlpha, target, RELOAD_FADE_SPEED, deltaTime);

	int ammo = m_player->GetAmmo();
	if (m_lastReloadAmmo >= 0 && ammo > m_lastReloadAmmo && m_player->IsReloading()) {
		for (int i = m_lastReloadAmmo; i < ammo && i < RELOAD_SEGMENT_NUM; i++) {
			m_ringSegmentPop[i] = RING_POP_TIME;
		}
	}
	m_lastReloadAmmo = ammo;

	float ringScale = 0.85f + 0.15f * m_reloadAlpha;
	m_ringBack.SetScale({ ringScale, ringScale, 1.0f });
	m_ringBack.SetMulColor(WithAlpha(WHITE, m_reloadAlpha));
	m_ringBack.Update();

	for (int i = 0; i < RELOAD_SEGMENT_NUM; i++) {
		float pop = 0.0f;
		if (m_ringSegmentPop[i] > 0.0f) {
			m_ringSegmentPop[i] -= deltaTime;
			pop = Saturate(m_ringSegmentPop[i] / RING_POP_TIME);
		}
		float scale = ringScale * (1.0f + pop * RING_POP_SCALE);
		Vector4 color = i < ammo ? RING_FILLED_COLOR : RING_EMPTY_COLOR;
		if (pop > 0.0f) {
			color = WHITE;
		}
		m_ringSegments[i].SetScale({ scale, scale, 1.0f });
		m_ringSegments[i].SetMulColor(WithAlpha(color, m_reloadAlpha));
		m_ringSegments[i].Update();
	}

	wchar_t text[32];
	swprintf_s(text, L"%d / %d", ammo, m_player->GetMaxAmmo());
	m_reloadCount.SetText(text);
	SetTextAlpha(m_reloadLabel, RING_FILLED_COLOR, m_reloadAlpha);
	SetTextAlpha(m_reloadCount, WHITE, m_reloadAlpha);
}

void PlayerUI::UpdateDamagePopups(float deltaTime)
{
	Vector3 cameraPosition = g_camera3D->GetPosition();
	Vector3 cameraForward = g_camera3D->GetForward();

	for (auto& popup : m_damagePopups) {
		popup.isVisible = false;
		if (!popup.isActive) {
			continue;
		}
		popup.timer += deltaTime;
		if (popup.timer >= POPUP_LIFE) {
			popup.isActive = false;
			continue;
		}

		Vector3 toPopup = popup.worldPosition - cameraPosition;
		if (toPopup.Dot(cameraForward) <= 0.0f) {
			continue;
		}

		Vector2 screen;
		g_camera3D->CalcScreenPositionFromWorldPosition(screen, popup.worldPosition);

		float life = popup.timer / POPUP_LIFE;
		float pop = 1.0f + 0.5f * (1.0f - Saturate(popup.timer / POPUP_POP_TIME));
		float scale = popup.scale * pop;
		float rise = POPUP_RISE * (1.0f - (1.0f - life) * (1.0f - life));
		float alpha = 1.0f - Saturate((popup.timer - POPUP_FADE_START) / (POPUP_LIFE - POPUP_FADE_START));
		float width = popup.digits * DIGIT_WIDTH * scale;

		Vector4 color = popup.scale > POPUP_SCALE ? POPUP_KILL_COLOR : POPUP_COLOR;
		popup.font.SetScale(scale);
		popup.font.SetPosition(MakePosition(screen.x - width * 0.5f, screen.y + rise + 20.0f * scale));
		SetTextAlpha(popup.font, color, alpha);
		popup.isVisible = true;
	}
}

void PlayerUI::Render(RenderContext& rc)
{
	m_vignette.Draw(rc);

	m_hpShade.Draw(rc);
	m_ammoShade.Draw(rc);

	m_hpBarBack.Draw(rc);
	m_hpBarLag.Draw(rc);
	m_hpBar.Draw(rc);
	m_hpLabel.Draw(rc);
	m_hpValue.Draw(rc);

	for (auto& pip : m_ammoPips) {
		pip.Draw(rc);
	}
	m_ammoValue.Draw(rc);
	m_ammoMax.Draw(rc);

	if (m_isCrosshairVisible) {
		for (auto& line : m_crossLines) {
			line.Draw(rc);
		}
		m_crossDot.Draw(rc);
		if (m_hitTimer > 0.0f) {
			for (auto& line : m_hitLines) {
				line.Draw(rc);
			}
		}
	}
	if (m_isHintVisible) {
		m_ammoHint.Draw(rc);
	}

	if (m_statusAlpha > 0.01f) {
		m_statusPill.Draw(rc);
		m_statusDiveText.Draw(rc);
		m_statusNoShotText.Draw(rc);
	}

	if (m_reloadAlpha > 0.01f) {
		m_ringBack.Draw(rc);
		for (auto& segment : m_ringSegments) {
			segment.Draw(rc);
		}
		m_reloadLabel.Draw(rc);
		m_reloadCount.Draw(rc);
	}

	for (auto& popup : m_damagePopups) {
		if (popup.isVisible) {
			popup.font.Draw(rc);
		}
	}
}

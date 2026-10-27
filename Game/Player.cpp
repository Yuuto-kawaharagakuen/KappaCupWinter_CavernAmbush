#include "stdafx.h"
#include "Player.h"
#include "SandBullet.h"

namespace
{
	const float CAPSULE_RADIUS = 30.0f;
	const float CAPSULE_HEIGHT = 110.0f;
	const float EYE_HEIGHT = 160.0f;
	const float WALK_SPEED = 400.0f;
	const float DIVE_SPEED = 500.0f;
	const float GRAVITY = 980.0f;
	const float YAW_SPEED = 150.0f;
	const float PITCH_SPEED = 100.0f;
	const float PITCH_LIMIT = 80.0f;
	const float STICK_DEADZONE = 0.15f;
	const float TPS_TARGET_HEIGHT = 50.0f;
	const float TPS_DISTANCE = 300.0f;
	const float TPS_HEIGHT = 50.0f;
	const float CAMERA_BLEND_TIME = 0.8f;
	const float LOOK_DISTANCE = 100.0f;
	const float SHOT_INTERVAL = 0.2f;
	const float MUZZLE_OFFSET = 40.0f;
	const int MAX_AMMO = 10;
	const float RELOAD_INTERVAL = 0.15f;
}

Player::Player()
{
}

Player::~Player()
{
}

bool Player::Start()
{
	m_charaCon.Init(CAPSULE_RADIUS, CAPSULE_HEIGHT, m_position);
	m_ammo = MAX_AMMO;
	g_camera3D->SetNear(0.5f);
	g_camera3D->SetFar(20000.0f);
	g_k2Engine->SetDrawVectorEnable();
	UpdateCamera();
	return true;
}

void Player::Update()
{
	UpdateState();
	Rotate();
	Move();
	UpdateCamera();
	UpdateReload();
	Shoot();

	if (m_state == enState_Surface) {
		fontRender.SetText(L"FPS");
		fontRender.SetPosition({ -800.0f,450.0f,0.0f });
		fontRender.SetScale(2.2f);
	}
	else {
		fontRender.SetText(L"TPS");
		fontRender.SetPosition({ -800.0f,450.0f,0.0f });
		fontRender.SetScale(2.2f);
	}
}

Vector3 Player::GetLookDirection() const
{
	float pitchRad = Math::DegToRad(m_pitch);
	return m_forward * cosf(pitchRad) + Vector3::Up * sinf(pitchRad);
}

bool Player::CanShoot() const
{
	return !IsDiving() && !IsSwitching() && m_ammo > 0;
}

int Player::GetMaxAmmo() const
{
	return MAX_AMMO;
}

void Player::UpdateReload()
{
	if (m_isReloading) {
		if (!IsDiving()) {
			m_isReloading = false;
			m_reloadTimer = 0.0f;
			return;
		}
		m_reloadTimer += g_gameTime->GetFrameDeltaTime();
		while (m_reloadTimer >= RELOAD_INTERVAL && m_ammo < MAX_AMMO) {
			m_reloadTimer -= RELOAD_INTERVAL;
			m_ammo++;
		}
		if (m_ammo >= MAX_AMMO) {
			m_isReloading = false;
			m_reloadTimer = 0.0f;
		}
		return;
	}

	if (IsDiving() && !IsSwitching() && m_ammo < MAX_AMMO && g_pad[0]->IsTrigger(enButtonX)) {
		m_isReloading = true;
		m_reloadTimer = 0.0f;
	}
}

void Player::Shoot()
{
	if (m_shotCooldown > 0.0f) {
		m_shotCooldown -= g_gameTime->GetFrameDeltaTime();
	}
	if (!CanShoot() || m_shotCooldown > 0.0f) {
		return;
	}
	if (!g_pad[0]->IsPress(enButtonRB2)) {
		return;
	}

	Vector3 lookDir = GetLookDirection();
	Vector3 muzzlePos = m_position;
	muzzlePos.y += EYE_HEIGHT;
	muzzlePos += lookDir * MUZZLE_OFFSET;

	SandBullet* bullet = NewGO<SandBullet>(0, "sandBullet");
	bullet->Init(muzzlePos, lookDir);
	m_shotCooldown = SHOT_INTERVAL;
	m_ammo--;
}

void Player::UpdateState()
{
	if (IsSwitching()) {
		return;
	}
	if (g_pad[0]->IsTrigger(enButtonB)) {
		if (m_state == enState_Surface) {
			m_state = enState_Dive;
		}
		else {
			m_state = enState_Surface;
		}
	}
}

void Player::Rotate()
{
	float deltaTime = g_gameTime->GetFrameDeltaTime();
	float stickX = g_pad[0]->GetRStickXF();
	float stickY = g_pad[0]->GetRStickYF();
	if (fabsf(stickX) < STICK_DEADZONE) {
		stickX = 0.0f;
	}
	if (fabsf(stickY) < STICK_DEADZONE || IsDiving()) {
		stickY = 0.0f;
	}

	m_yaw += stickX * YAW_SPEED * deltaTime;
	m_pitch += stickY * PITCH_SPEED * deltaTime;
	if (m_pitch > PITCH_LIMIT) {
		m_pitch = PITCH_LIMIT;
	}
	if (m_pitch < -PITCH_LIMIT) {
		m_pitch = -PITCH_LIMIT;
	}

	float yawRad = Math::DegToRad(m_yaw);
	m_forward.Set(sinf(yawRad), 0.0f, cosf(yawRad));
	m_right.Set(cosf(yawRad), 0.0f, -sinf(yawRad));
}

void Player::Move()
{
	float deltaTime = g_gameTime->GetFrameDeltaTime();
	float stickX = g_pad[0]->GetLStickXF();
	float stickY = g_pad[0]->GetLStickYF();

	Vector3 inputDir = m_forward * stickY + m_right * stickX;
	if (IsSwitching()) {
		inputDir = Vector3::Zero;
	}
	if (inputDir.LengthSq() > 1.0f) {
		inputDir.Normalize();
	}

	float speed = IsDiving() ? DIVE_SPEED : WALK_SPEED;
	m_moveSpeed.x = inputDir.x * speed;
	m_moveSpeed.z = inputDir.z * speed;

	if (m_charaCon.IsOnGround()) {
		m_moveSpeed.y = 0.0f;
	}
	m_moveSpeed.y -= GRAVITY * deltaTime;

	m_position = m_charaCon.Execute(m_moveSpeed, deltaTime);
}

void Player::UpdateCamera()
{
	float deltaTime = g_gameTime->GetFrameDeltaTime();
	float blendTarget = IsDiving() ? 1.0f : 0.0f;
	float blendStep = deltaTime / CAMERA_BLEND_TIME;
	if (m_cameraBlend < blendTarget) {
		m_cameraBlend += blendStep;
		if (m_cameraBlend > blendTarget) {
			m_cameraBlend = blendTarget;
		}
	}
	else if (m_cameraBlend > blendTarget) {
		m_cameraBlend -= blendStep;
		if (m_cameraBlend < blendTarget) {
			m_cameraBlend = blendTarget;
		}
	}

	Vector3 lookDir = GetLookDirection();

	Vector3 fpsPos = m_position;
	fpsPos.y += EYE_HEIGHT;
	Vector3 fpsTarget = fpsPos + lookDir * LOOK_DISTANCE;

	Vector3 tpsTarget = m_position;
	tpsTarget.y += TPS_TARGET_HEIGHT;
	Vector3 tpsPos = tpsTarget - m_forward * TPS_DISTANCE + Vector3::Up * TPS_HEIGHT;

	float t = m_cameraBlend * m_cameraBlend * (3.0f - 2.0f * m_cameraBlend);
	Vector3 cameraPos;
	cameraPos.Lerp(t, fpsPos, tpsPos);
	Vector3 cameraTarget;
	cameraTarget.Lerp(t, fpsTarget, tpsTarget);

	g_camera3D->SetPosition(cameraPos);
	g_camera3D->SetTarget(cameraTarget);
	g_camera3D->SetUp(Vector3::Up);

	if (IsDiving()) {
		g_k2Engine->DrawVector(m_forward * 100.0f, m_position, "player");
	}
}

void Player::Render(RenderContext& rc)
{
	fontRender.Draw(rc);
}

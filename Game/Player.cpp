#include "stdafx.h"
#include "Player.h"
#include "SandBullet.h"
#include "HitUtil.h"

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
	const char* const GUN_MODEL_PATH = "Assets/modelData/Blaster.tkm";
	const Vector3 GUN_OFFSET = { 15.0f, -15.0f, 46.0f };
	const float GUN_SCALE = 0.75f;
	const float GUN_AIM_YAW = 3.0f;
	const float GUN_KICK_DECAY = 11.0f;
	const float GUN_KICK_BACK = 6.0f;
	const float GUN_KICK_UP = 1.5f;
	const float GUN_KICK_PITCH = 9.0f;
	const float GUN_BOB_SPEED = 9.0f;
	const float GUN_BOB_X = 0.9f;
	const float GUN_BOB_Y = 0.6f;
	const float GUN_BOB_BLEND = 8.0f;
	const float GUN_RAISE_SPEED = 4.0f;
	const float GUN_RAISE_DROP = 18.0f;
	const float GUN_RAISE_PITCH = 25.0f;
	const float GUN_SWAY_AMOUNT = 0.6f;
	const float GUN_SWAY_LIMIT = 6.0f;
	const float GUN_SWAY_DECAY = 10.0f;
	const float MOVE_THRESHOLD = 0.01f;
	const int MAX_HP = 100;
	const float INVINCIBLE_TIME = 0.5f;
	const int DEBUG_DAMAGE = 20;
	const float BODY_POINT_MARGIN = 5.0f;
	const char* const MOUND_MODEL_PATH = "Assets/modelData/SandMound.tkm";
	const float MOUND_WOBBLE_SPEED = 14.0f;
	const float MOUND_WOBBLE_HEIGHT = 0.12f;
	const float MOUND_WOBBLE_WIDTH = 0.05f;
	const float MOUND_MOVE_STRETCH = 0.2f;
	const float MOUND_MOVE_BLEND = 6.0f;
	const float MOUND_SINK = 25.0f;
	const float MOUND_SCALE = 1.4f;

	float ClampFloat(float value, float minValue, float maxValue)
	{
		if (value < minValue) {
			return minValue;
		}
		if (value > maxValue) {
			return maxValue;
		}
		return value;
	}
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
	m_hp = MAX_HP;
	g_camera3D->SetNear(0.5f);
	g_camera3D->SetFar(20000.0f);
	g_k2Engine->SetDrawVectorEnable();
	m_gunModel.Init(GUN_MODEL_PATH);
	m_gunModel.SetShadowCasterFlag(false);
	m_moundModel.Init(MOUND_MODEL_PATH);
	m_gunModel.SetScale({ GUN_SCALE, GUN_SCALE, GUN_SCALE });
	UpdateCamera();
	UpdateGun();
	return true;
}

void Player::Update()
{
	UpdateDamage();
	if (!IsDead()) {
		UpdateState();
		Rotate();
	}
	Move();
	UpdateCamera();
	UpdateGun();
	UpdateMound();
	if (!IsDead()) {
		UpdateReload();
		Shoot();
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

int Player::GetMaxHp() const
{
	return MAX_HP;
}

void Player::Damage(int amount)
{
	if (IsDead() || IsDiving() || m_invincibleTimer > 0.0f) {
		return;
	}
	m_hp -= amount;
	if (m_hp < 0) {
		m_hp = 0;
	}
	m_invincibleTimer = INVINCIBLE_TIME;
}

bool Player::HitTest(const Vector3& start, const Vector3& end, float radius) const
{
	if (IsDead() || IsDiving()) {
		return false;
	}
	float hitRate = 0.0f;
	return HitUtil::SegmentCapsule(start, end, radius, m_position, CAPSULE_RADIUS, CAPSULE_HEIGHT, hitRate);
}

bool Player::IsBodyPoint(const Vector3& point) const
{
	float hitRate = 0.0f;
	return HitUtil::SegmentCapsule(point, point, BODY_POINT_MARGIN, m_position, CAPSULE_RADIUS, CAPSULE_HEIGHT, hitRate);
}

void Player::UpdateDamage()
{
	if (m_invincibleTimer > 0.0f) {
		m_invincibleTimer -= g_gameTime->GetFrameDeltaTime();
	}
#ifdef _DEBUG
	if (g_pad[0]->IsTrigger(enButtonSelect)) {
		Damage(DEBUG_DAMAGE);
	}
#endif
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
	m_isShot = true;
	m_gunKick = 1.0f;
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
	if (IsSwitching() || IsDead()) {
		inputDir = Vector3::Zero;
	}
	if (inputDir.LengthSq() > 1.0f) {
		inputDir.Normalize();
	}
	m_isMoving = inputDir.LengthSq() > MOVE_THRESHOLD && m_charaCon.IsOnGround();

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

void Player::UpdateGun()
{
	float deltaTime = g_gameTime->GetFrameDeltaTime();
	float yawDelta = m_yaw - m_lastYaw;
	float pitchDelta = m_pitch - m_lastPitch;
	m_lastYaw = m_yaw;
	m_lastPitch = m_pitch;

	m_isGunVisible = !IsDiving() && !IsSwitching();
	if (!m_isGunVisible) {
		m_gunRaise = 0.0f;
		m_gunKick = 0.0f;
		return;
	}

	float decay = expf(-GUN_SWAY_DECAY * deltaTime);
	m_gunSwayYaw = ClampFloat(m_gunSwayYaw * decay - yawDelta * GUN_SWAY_AMOUNT, -GUN_SWAY_LIMIT, GUN_SWAY_LIMIT);
	m_gunSwayPitch = ClampFloat(m_gunSwayPitch * decay - pitchDelta * GUN_SWAY_AMOUNT, -GUN_SWAY_LIMIT, GUN_SWAY_LIMIT);
	m_gunKick *= expf(-GUN_KICK_DECAY * deltaTime);
	m_gunRaise = ClampFloat(m_gunRaise + GUN_RAISE_SPEED * deltaTime, 0.0f, 1.0f);

	float bobTarget = m_isMoving ? 1.0f : 0.0f;
	m_gunBobWeight += (bobTarget - m_gunBobWeight) * ClampFloat(GUN_BOB_BLEND * deltaTime, 0.0f, 1.0f);
	if (m_isMoving) {
		m_gunBobPhase += GUN_BOB_SPEED * deltaTime;
	}

	float raise = 1.0f - m_gunRaise;
	raise = raise * raise;

	Quaternion yawRot;
	yawRot.SetRotationDegY(m_yaw);
	Quaternion pitchRot;
	pitchRot.SetRotationDegX(-m_pitch);
	Quaternion rotation;
	rotation.Multiply(pitchRot, yawRot);

	Vector3 right = Vector3::AxisX;
	Vector3 up = Vector3::AxisY;
	Vector3 forward = Vector3::AxisZ;
	rotation.Apply(right);
	rotation.Apply(up);
	rotation.Apply(forward);

	Vector3 offset = GUN_OFFSET;
	offset.x += sinf(m_gunBobPhase) * GUN_BOB_X * m_gunBobWeight;
	offset.y += -fabsf(cosf(m_gunBobPhase)) * GUN_BOB_Y * m_gunBobWeight;
	offset.y += m_gunKick * GUN_KICK_UP - raise * GUN_RAISE_DROP;
	offset.z -= m_gunKick * GUN_KICK_BACK;

	Vector3 gunPos = m_position;
	gunPos.y += EYE_HEIGHT;
	gunPos += right * offset.x + up * offset.y + forward * offset.z;

	Quaternion localRot;
	localRot.SetRotationDegX(-(m_gunKick * GUN_KICK_PITCH) + raise * GUN_RAISE_PITCH + m_gunSwayPitch);
	Quaternion swayRot;
	swayRot.SetRotationDegY(m_gunSwayYaw + GUN_AIM_YAW);
	Quaternion gunRot;
	gunRot.Multiply(localRot, swayRot);
	Quaternion finalRot;
	finalRot.Multiply(gunRot, rotation);

	m_gunModel.SetPosition(gunPos);
	m_gunModel.SetRotation(finalRot);
	m_gunModel.Update();
}

void Player::UpdateMound()
{
	float deltaTime = g_gameTime->GetFrameDeltaTime();
	m_isMoundVisible = m_cameraBlend > 0.0f;
	if (!m_isMoundVisible) {
		m_moundMoveWeight = 0.0f;
		return;
	}

	float moveTarget = m_isMoving && IsDiving() ? 1.0f : 0.0f;
	m_moundMoveWeight += (moveTarget - m_moundMoveWeight) * ClampFloat(MOUND_MOVE_BLEND * deltaTime, 0.0f, 1.0f);
	m_moundPhase += MOUND_WOBBLE_SPEED * deltaTime * (0.3f + m_moundMoveWeight);

	float rise = m_cameraBlend * m_cameraBlend * (3.0f - 2.0f * m_cameraBlend) * MOUND_SCALE;
	float wobble = sinf(m_moundPhase);
	float width = rise * (1.0f + wobble * MOUND_WOBBLE_WIDTH);
	float height = rise * (1.0f - wobble * MOUND_WOBBLE_HEIGHT * (0.4f + m_moundMoveWeight));
	float length = rise * (1.0f + m_moundMoveWeight * MOUND_MOVE_STRETCH);

	Vector3 position = m_position;
	position.y -= (1.0f - rise / MOUND_SCALE) * MOUND_SINK;
	Quaternion rotation;
	rotation.SetRotationDegY(m_yaw);

	m_moundModel.SetPosition(position);
	m_moundModel.SetRotation(rotation);
	m_moundModel.SetScale({ width, height, length });
	m_moundModel.Update();
}

void Player::Render(RenderContext& rc)
{
	if (m_isMoundVisible) {
		m_moundModel.Draw(rc);
	}
	if (m_isGunVisible) {
		m_gunModel.Draw(rc);
	}
}

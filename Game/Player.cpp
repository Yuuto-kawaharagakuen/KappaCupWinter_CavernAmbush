#include "stdafx.h"
#include "Player.h"
#include "SandBullet.h"
#include "HitUtil.h"
#include "MathUtil.h"
#include "ObjectNames.h"

namespace
{
	const float START_YAW = 90.0f;
	const float CAPSULE_RADIUS = 30.0f;
	const float CAPSULE_HEIGHT = 110.0f;
	const float DIVE_CAPSULE_HEIGHT = 20.0f;
	const float HEADROOM_CHECK_RADIUS = 26.0f;
	const float DIVE_HEAD_CHECK_RADIUS = 10.0f;
	const float DIVE_HEAD_CHECK_TOP = 60.0f;
	const float CEILING_CHECK_START = 15.0f;
	const float CEILING_CHECK_LENGTH = 3000.0f;
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
	const char* const GUN_MODEL_PATH = "Assets/modelData/M4A4.tkm";
	const Vector3 GUN_OFFSET = { 13.0f, -11.0f, 16.0f };
	const float GUN_SCALE = 1.0f;
	const float GUN_AIM_YAW = 2.0f;
	const Vector3 ADS_OFFSET = { 0.0f, 0.0f, 9.0f };
	const float ADS_TIME = 0.18f;
	const float ADS_ZOOM = 1.25f;
	const float ADS_TURN_RATE = 0.4f;
	const float ADS_MOVE_RATE = 0.5f;
	const float ADS_KICK_RATE = 0.3f;
	const float ADS_SWAY_RATE = 0.15f;
	const float ADS_GUN_HIDE_RATE = 0.95f;
	const Vector3 GUN_MUZZLE_LOCAL = { -1.41f, -5.4f, 51.9f };
	const float AIM_RAY_LENGTH = 3000.0f;
	const float AIM_MIN_DISTANCE = 60.0f;
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
	const float CAMERA_COLLISION_RADIUS = 15.0f;
	const float CAMERA_COLLISION_BUFFER = 10.0f;
	const float CAMERA_MIN_DISTANCE = 20.0f;

	struct CameraSweepResult : public btCollisionWorld::ConvexResultCallback
	{
		bool isHit = false;
		float hitFraction = 1.0f;
		btScalar addSingleResult(btCollisionWorld::LocalConvexResult& convexResult, bool normalInWorldSpace) override
		{
			const btCollisionObject* object = convexResult.m_hitCollisionObject;
			if (object->getUserIndex() == enCollisionAttr_Character
				|| object->getInternalType() == btCollisionObject::CO_GHOST_OBJECT) {
				return 1.0f;
			}
			if (convexResult.m_hitFraction < hitFraction) {
				hitFraction = convexResult.m_hitFraction;
				isHit = true;
			}
			return convexResult.m_hitFraction;
		}
	};
}

Player::Player()
{
}

Player::~Player()
{
	if (m_baseViewAngle > 0.0f) {
		g_camera3D->SetViewAngle(m_baseViewAngle);
	}
}

bool Player::Start()
{
	m_charaCon.Init(CAPSULE_RADIUS, CAPSULE_HEIGHT, m_position);
	m_diveCharaCon.Init(CAPSULE_RADIUS, DIVE_CAPSULE_HEIGHT, m_position);
	m_diveCharaCon.RemoveRigidBoby();
	m_headroomCollider.Create(HEADROOM_CHECK_RADIUS);
	m_diveHeadCollider.Create(DIVE_HEAD_CHECK_RADIUS);
	m_safePosition = m_position;
	m_ammo = MAX_AMMO;
	m_hp = MAX_HP;
	m_yaw = START_YAW;
	m_lastYaw = START_YAW;
	UpdateAxes();
	g_camera3D->SetNear(0.5f);
	g_camera3D->SetFar(20000.0f);
	m_baseViewAngle = g_camera3D->GetViewAngle();
	g_k2Engine->SetDrawVectorEnable();
	m_gunModel.Init(GUN_MODEL_PATH);
	m_gunModel.SetShadowCasterFlag(false);
	m_moundModel.Init(MOUND_MODEL_PATH);
	m_sandSplash.Init();
	m_cameraCollider.Create(CAMERA_COLLISION_RADIUS);
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
		UpdateAim();
		Rotate();
	}
	Move();
	UpdateCamera();
	UpdateGun();
	UpdateMound();
	m_sandSplash.Update(g_gameTime->GetFrameDeltaTime());
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
	Vector3 eyePos = m_position;
	eyePos.y += EYE_HEIGHT;
	Vector3 aimPoint = eyePos + lookDir * AIM_RAY_LENGTH;
	Vector3 hitPos;
	if (PhysicsWorld::GetInstance()->RayTest(eyePos + lookDir * MUZZLE_OFFSET, aimPoint, hitPos)) {
		aimPoint = hitPos;
	}

	Vector3 muzzlePos = GetMuzzlePosition();
	Vector3 shotDir = aimPoint - muzzlePos;
	if (shotDir.Length() < AIM_MIN_DISTANCE || shotDir.Dot(lookDir) <= 0.0f) {
		shotDir = lookDir;
	}
	shotDir.Normalize();

	SandBullet* bullet = NewGO<SandBullet>(0, ObjectName::SAND_BULLET);
	bullet->Init(muzzlePos, shotDir);
	m_shotCooldown = SHOT_INTERVAL;
	m_ammo--;
	m_isShot = true;
	m_gunKick = 1.0f;
}

void Player::UpdateAim()
{
	bool isAiming = !IsDiving() && !IsSwitching() && g_pad[0]->IsPress(enButtonLB2);
	float step = g_gameTime->GetFrameDeltaTime() / ADS_TIME;
	m_aimBlend = MathUtil::Clamp(m_aimBlend + (isAiming ? step : -step), 0.0f, 1.0f);
}

Vector3 Player::GetMuzzlePosition() const
{
	Vector3 muzzle = GUN_MUZZLE_LOCAL * GUN_SCALE;
	m_gunRotation.Apply(muzzle);
	return m_gunPosition + muzzle;
}

float Player::GetAimRate() const
{
	return MathUtil::SmoothStep(m_aimBlend);
}

void Player::UpdateState()
{
	if (IsSwitching()) {
		return;
	}
	if (!g_pad[0]->IsTrigger(enButtonB)) {
		return;
	}
	if (m_state == enState_Surface) {
		m_state = enState_Dive;
		SetDiveBody(true);
		m_sandSplash.Play(m_position, true);
		return;
	}
	if (!HasHeadroom()) {
		m_isSurfaceBlocked = true;
		return;
	}
	m_state = enState_Surface;
	SetDiveBody(false);
	m_sandSplash.Play(m_position, false);
}

CharacterController& Player::GetCharaCon()
{
	return m_isDiveBody ? m_diveCharaCon : m_charaCon;
}

void Player::SetDiveBody(bool isDive)
{
	if (m_isDiveBody == isDive) {
		return;
	}
	GetCharaCon().RemoveRigidBoby();
	m_isDiveBody = isDive;
	CharacterController& next = GetCharaCon();
	next.SetPosition(m_position);
	PhysicsWorld::GetInstance()->AddRigidBody(*next.GetRigidBody());
}

bool Player::HasHeadroom() const
{
	float diveTop = DIVE_CAPSULE_HEIGHT + CAPSULE_RADIUS * 2.0f;
	float surfaceTop = CAPSULE_HEIGHT + CAPSULE_RADIUS * 2.0f;
	btTransform start, end;
	start.setIdentity();
	start.setOrigin(btVector3(m_position.x, m_position.y + diveTop - HEADROOM_CHECK_RADIUS, m_position.z));
	end.setIdentity();
	end.setOrigin(btVector3(m_position.x, m_position.y + surfaceTop - HEADROOM_CHECK_RADIUS, m_position.z));
	CameraSweepResult result;
	PhysicsWorld::GetInstance()->ConvexSweepTest(static_cast<const btConvexShape*>(m_headroomCollider.GetBody()), start, end, result);
	return !result.isHit;
}

bool Player::IsDiveHeadBlocked(const Vector3& from, const Vector3& to) const
{
	Vector3 move = to - from;
	move.y = 0.0f;
	if (move.LengthSq() < FLT_EPSILON) {
		return false;
	}
	float headOffset = DIVE_HEAD_CHECK_TOP - DIVE_HEAD_CHECK_RADIUS;
	btTransform start, end;
	start.setIdentity();
	start.setOrigin(btVector3(from.x, from.y + headOffset, from.z));
	end.setIdentity();
	end.setOrigin(btVector3(to.x, to.y + headOffset, to.z));
	CameraSweepResult result;
	PhysicsWorld::GetInstance()->ConvexSweepTest(static_cast<const btConvexShape*>(m_diveHeadCollider.GetBody()), start, end, result);
	return result.isHit;
}

bool Player::HasCeilingAbove(const Vector3& position) const
{
	btTransform start, end;
	start.setIdentity();
	start.setOrigin(btVector3(position.x, position.y + CEILING_CHECK_START, position.z));
	end.setIdentity();
	end.setOrigin(btVector3(position.x, position.y + CEILING_CHECK_LENGTH, position.z));
	CameraSweepResult result;
	PhysicsWorld::GetInstance()->ConvexSweepTest(static_cast<const btConvexShape*>(m_diveHeadCollider.GetBody()), start, end, result);
	return result.isHit;
}

Vector3 Player::LimitDiveMove(const Vector3& from, const Vector3& to) const
{
	if (!IsDiveHeadBlocked(from, to)) {
		return to;
	}
	Vector3 slideX = { to.x, to.y, from.z };
	if (!IsDiveHeadBlocked(from, slideX)) {
		return slideX;
	}
	Vector3 slideZ = { from.x, to.y, to.z };
	if (!IsDiveHeadBlocked(from, slideZ)) {
		return slideZ;
	}
	return { from.x, to.y, from.z };
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

	float turnRate = 1.0f - (1.0f - ADS_TURN_RATE) * GetAimRate();
	m_yaw += stickX * YAW_SPEED * turnRate * deltaTime;
	m_pitch += stickY * PITCH_SPEED * turnRate * deltaTime;
	if (m_pitch > PITCH_LIMIT) {
		m_pitch = PITCH_LIMIT;
	}
	if (m_pitch < -PITCH_LIMIT) {
		m_pitch = -PITCH_LIMIT;
	}

	UpdateAxes();
}

void Player::UpdateAxes()
{
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
	CharacterController& charaCon = GetCharaCon();
	m_isMoving = inputDir.LengthSq() > MOVE_THRESHOLD && charaCon.IsOnGround();

	float speed = IsDiving() ? DIVE_SPEED : WALK_SPEED * (1.0f - (1.0f - ADS_MOVE_RATE) * GetAimRate());
	m_moveSpeed.x = inputDir.x * speed;
	m_moveSpeed.z = inputDir.z * speed;

	if (charaCon.IsOnGround()) {
		m_moveSpeed.y = 0.0f;
	}
	m_moveSpeed.y -= GRAVITY * deltaTime;

	Vector3 prevPosition = m_position;
	m_position = charaCon.Execute(m_moveSpeed, deltaTime);
	if (m_isDiveBody) {
		Vector3 limited = LimitDiveMove(prevPosition, m_position);
		if (limited.x != m_position.x || limited.z != m_position.z) {
			m_position = limited;
			charaCon.SetPosition(m_position);
		}
	}
	if (HasCeilingAbove(m_position)) {
		m_safePosition = m_position;
	}
	else {
		m_position = m_safePosition;
		m_moveSpeed = Vector3::Zero;
		charaCon.SetPosition(m_position);
	}
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
	tpsPos = ResolveCameraCollision(tpsTarget, tpsPos);

	float t = MathUtil::SmoothStep(m_cameraBlend);
	Vector3 cameraPos;
	cameraPos.Lerp(t, fpsPos, tpsPos);
	Vector3 cameraTarget;
	cameraTarget.Lerp(t, fpsTarget, tpsTarget);

	g_camera3D->SetPosition(cameraPos);
	g_camera3D->SetTarget(cameraTarget);
	g_camera3D->SetUp(Vector3::Up);
	float aimRate = GetAimRate();
	g_camera3D->SetViewAngle(m_baseViewAngle / (1.0f + (ADS_ZOOM - 1.0f) * aimRate));

	if (IsDiving()) {
		g_k2Engine->DrawVector(m_forward * 100.0f, m_position, "player");
	}
}

Vector3 Player::ResolveCameraCollision(const Vector3& target, const Vector3& desired) const
{
	Vector3 toCamera = desired - target;
	float distance = toCamera.Length();
	if (distance < 0.01f) {
		return desired;
	}

	btTransform start;
	start.setIdentity();
	start.setOrigin(btVector3(target.x, target.y, target.z));
	btTransform end;
	end.setIdentity();
	end.setOrigin(btVector3(desired.x, desired.y, desired.z));
	CameraSweepResult result;
	PhysicsWorld::GetInstance()->ConvexSweepTest(static_cast<const btConvexShape*>(m_cameraCollider.GetBody()), start, end, result);
	if (!result.isHit) {
		return desired;
	}

	toCamera.Normalize();
	float safeDistance = distance * result.hitFraction - CAMERA_COLLISION_BUFFER;
	if (safeDistance < CAMERA_MIN_DISTANCE) {
		safeDistance = CAMERA_MIN_DISTANCE;
	}
	return target + toCamera * safeDistance;
}

void Player::UpdateGun()
{
	float deltaTime = g_gameTime->GetFrameDeltaTime();
	float yawDelta = m_yaw - m_lastYaw;
	float pitchDelta = m_pitch - m_lastPitch;
	m_lastYaw = m_yaw;
	m_lastPitch = m_pitch;

	float aimRate = GetAimRate();
	m_isGunVisible = !IsDiving() && !IsSwitching() && aimRate < ADS_GUN_HIDE_RATE;
	if (IsDiving() || IsSwitching()) {
		m_gunRaise = 0.0f;
		m_gunKick = 0.0f;
		return;
	}

	float decay = expf(-GUN_SWAY_DECAY * deltaTime);
	m_gunSwayYaw = MathUtil::Clamp(m_gunSwayYaw * decay - yawDelta * GUN_SWAY_AMOUNT, -GUN_SWAY_LIMIT, GUN_SWAY_LIMIT);
	m_gunSwayPitch = MathUtil::Clamp(m_gunSwayPitch * decay - pitchDelta * GUN_SWAY_AMOUNT, -GUN_SWAY_LIMIT, GUN_SWAY_LIMIT);
	m_gunKick *= expf(-GUN_KICK_DECAY * deltaTime);
	m_gunRaise = MathUtil::Clamp(m_gunRaise + GUN_RAISE_SPEED * deltaTime, 0.0f, 1.0f);

	float bobTarget = m_isMoving ? 1.0f : 0.0f;
	m_gunBobWeight += (bobTarget - m_gunBobWeight) * MathUtil::Clamp(GUN_BOB_BLEND * deltaTime, 0.0f, 1.0f);
	if (m_isMoving) {
		m_gunBobPhase += GUN_BOB_SPEED * deltaTime;
	}

	float raise = 1.0f - m_gunRaise;
	raise = raise * raise;
	float hipRate = 1.0f - aimRate;
	float swayRate = 1.0f - (1.0f - ADS_SWAY_RATE) * aimRate;
	float kick = m_gunKick * (1.0f - (1.0f - ADS_KICK_RATE) * aimRate);

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

	Vector3 offset;
	offset.Lerp(aimRate, GUN_OFFSET, ADS_OFFSET);
	offset.x += sinf(m_gunBobPhase) * GUN_BOB_X * m_gunBobWeight * swayRate;
	offset.y += -fabsf(cosf(m_gunBobPhase)) * GUN_BOB_Y * m_gunBobWeight * swayRate;
	offset.y += kick * GUN_KICK_UP - raise * GUN_RAISE_DROP;
	offset.z -= kick * GUN_KICK_BACK;

	Vector3 gunPos = m_position;
	gunPos.y += EYE_HEIGHT;
	gunPos += right * offset.x + up * offset.y + forward * offset.z;

	Quaternion localRot;
	localRot.SetRotationDegX(-(kick * GUN_KICK_PITCH) + raise * GUN_RAISE_PITCH + m_gunSwayPitch * swayRate);
	Quaternion swayRot;
	swayRot.SetRotationDegY(m_gunSwayYaw * swayRate + GUN_AIM_YAW * hipRate);
	Quaternion gunRot;
	gunRot.Multiply(localRot, swayRot);
	Quaternion finalRot;
	finalRot.Multiply(gunRot, rotation);

	m_gunPosition = gunPos;
	m_gunRotation = finalRot;
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
	m_moundMoveWeight += (moveTarget - m_moundMoveWeight) * MathUtil::Clamp(MOUND_MOVE_BLEND * deltaTime, 0.0f, 1.0f);
	m_moundPhase += MOUND_WOBBLE_SPEED * deltaTime * (0.3f + m_moundMoveWeight);

	float rise = MathUtil::SmoothStep(m_cameraBlend) * MOUND_SCALE;
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
	m_sandSplash.Draw(rc);
}

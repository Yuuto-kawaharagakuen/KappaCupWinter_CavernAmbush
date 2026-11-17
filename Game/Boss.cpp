#include "stdafx.h"
#include "Boss.h"
#include "Player.h"
#include "EnemyBullet.h"
#include "Grenade.h"
#include "EffectManager.h"
#include "SoundList.h"
#include "MathUtil.h"
#include "ObjectNames.h"

namespace
{
	const char* const MODEL_PATH = "Assets/modelData/Boss.tkm";
	const char* const ANIMATION_PATHS[] = {
		"Assets/animData/Boss_Idle.tka",
		"Assets/animData/Boss_Shoot.tka",
		"Assets/animData/Boss_Throw.tka",
		"Assets/animData/Boss_Death.tka",
		"Assets/animData/Boss_Hit.tka",
	};
	const char* const FLAME_MODEL_PATH = "Assets/modelData/MuzzleFlash.tkm";
	const char* const EMISSIVE_SHADER_PATH = "Assets/shader/Emissive.fx";
	const wchar_t* const MUZZLE_BONE = L"BossMuzzle";
	const wchar_t* const RIGHT_HAND_BONE = L"mixamorig:RightHand";
	const wchar_t* const JET_BONES[] = { L"BossJetL", L"BossJetR" };

	const float MODEL_SCALE = 1.25f;
	const Vector3 JET_FALLBACK_OFFSETS[] = { { 12.0f, 135.0f, -48.0f }, { -12.0f, 135.0f, -48.0f } };
	const Vector3 MUZZLE_FALLBACK_OFFSET = { -8.0f, 175.0f, 88.0f };
	const Vector3 HAND_FALLBACK_OFFSET = { -22.0f, 150.0f, 38.0f };

	const int MAX_HP = 300;
	const float CAPSULE_RADIUS = 44.0f;
	const float CAPSULE_HEIGHT = 150.0f;
	const float BODY_HEIGHT = 245.0f;
	const float GRAVITY = 980.0f;

	const float HOVER_HEIGHT = 280.0f;
	const float MIN_HOVER_HEIGHT = 60.0f;
	const float CEILING_MARGIN = 40.0f;
	const float BOB_HEIGHT = 18.0f;
	const float BOB_SPEED = 1.8f;
	const float HEIGHT_GAIN = 2.5f;
	const float MAX_VERTICAL_SPEED = 260.0f;
	const float FLOOR_RAY_LENGTH = 2000.0f;
	const float CEILING_RAY_LENGTH = 2000.0f;

	const float MOVE_SPEED = 240.0f;
	const float MOVE_ACCEL = 2.5f;
	const float MOVE_SLOW_DISTANCE = 200.0f;
	const float MOVE_ARRIVE_DISTANCE = 40.0f;
	const float MOVE_INTERVAL_MIN = 2.5f;
	const float MOVE_INTERVAL_RANGE = 2.0f;
	const float ORBIT_MIN = 420.0f;
	const float ORBIT_MAX = 650.0f;
	const int MOVE_TRY_NUM = 8;
	const float MOVE_PATH_HEIGHT = 100.0f;
	const float TURN_SPEED = 200.0f;

	const float ACTIVATE_RANGE = 1500.0f;
	const float SIGHT_RANGE = 2200.0f;
	const float EYE_HEIGHT = 212.0f;
	const float PLAYER_AIM_HEIGHT = 120.0f;
	const float DIVING_AIM_HEIGHT = 20.0f;
	const float SIGHT_RAY_MARGIN = 50.0f;
	const float SIGHT_CHARACTER_MARGIN = 5.0f;
	const float SIGHT_CHARACTER_SKIP = 100.0f;
	const int SIGHT_RAY_RETRY_NUM = 4;

	const float FIRST_ATTACK_DELAY = 1.2f;
	const int RIFLE_BURST = 4;
	const float RIFLE_INTERVAL = 0.22f;
	const float RIFLE_REST_TIME = 1.3f;
	const float NO_SIGHT_RETRY_TIME = 0.5f;
	const int SHOT_DAMAGE = 8;
	const float SHOT_SPREAD = 0.045f;
	const float SHOT_VOLUME = 0.6f;
	const float SHOT_SOUND_RANGE = 4000.0f;
	const float FLASH_SCALE = 1.5f;
	const int GRENADE_EVERY = 3;
	const int GRENADE_COUNT = 3;
	const int GRENADE_DAMAGE = 30;
	const float THROW_ANIMATION_SPEED = 1.6f;
	const float THROW_RELEASE_TIME = 38.0f / 30.0f / THROW_ANIMATION_SPEED;
	const float THROW_TIME = 66.0f / 30.0f / THROW_ANIMATION_SPEED;
	const float GRENADE_REST_TIME = 1.8f;
	const float GRENADE_TARGET_SPREAD = 70.0f;
	const float GRENADE_FLIGHT_SPEED = 650.0f;
	const float GRENADE_FLIGHT_MIN = 0.8f;
	const float GRENADE_FLIGHT_MAX = 1.6f;

	const float HIT_ANIMATION_SPEED = 1.5f;
	const float HIT_COOLDOWN = 2.5f;
	const float ANIMATION_BLEND_TIME = 0.2f;
	const float SHOOT_BLEND_TIME = 0.05f;
	const float DEATH_BLEND_TIME = 0.1f;
	const float DEFEAT_DELAY = 3.5f;

	const float JET_EMISSION = 2.4f;
	const float JET_SCALE = 1.9f;
	const float JET_LENGTH_RATE = 1.8f;
	const float JET_FLICKER_SPEED = 30.0f;
	const float JET_FLICKER_RATE = 0.2f;
	const Vector3 JET_LIGHT_COLOR = { 5.0f, 3.0f, 1.2f };
	const float JET_LIGHT_RANGE = 550.0f;
	const float JET_LIGHT_POW = 1.5f;
	const float JET_LIGHT_DOWN = 50.0f;
	const float JET_SOUND_VOLUME = 0.6f;
	const float JET_SOUND_RANGE = 2500.0f;

	float AngleDiff(float from, float to)
	{
		return fmodf(to - from + 540.0f, 360.0f) - 180.0f;
	}

	float Random01()
	{
		return static_cast<float>(rand()) / RAND_MAX;
	}

	float RandomRange(float range)
	{
		return (Random01() * 2.0f - 1.0f) * range;
	}
}

Boss::Boss()
{
}

Boss::~Boss()
{
	SoundList::StopLoop(m_jetSound);
}

void Boss::OnStart()
{
	for (int i = 0; i < enAnimationClip_Num; i++) {
		m_animationClips[i].Load(ANIMATION_PATHS[i]);
		m_animationClips[i].SetLoopFlag(i == enAnimationClip_Idle);
	}
	m_modelScale = MODEL_SCALE;
	InitBase(MODEL_PATH, MAX_HP, CAPSULE_RADIUS, CAPSULE_HEIGHT, m_animationClips, enAnimationClip_Num);
	m_player = FindGO<Player>(ObjectName::PLAYER);
	m_rotation.SetRotationDegY(m_yaw);
	m_muzzleBoneId = m_model.FindBoneID(MUZZLE_BONE);
	m_rightHandBoneId = m_model.FindBoneID(RIGHT_HAND_BONE);
	PlayClip(enAnimationClip_Idle, 1.0f, 0.0f);
	InitJets();
	m_jetSound = SoundList::PlayLoop(SoundId::BossJet, 0.0f);
}

void Boss::InitJets()
{
	m_jetEmission = JET_EMISSION;
	for (int i = 0; i < 2; i++) {
		Jet& jet = m_jets[i];
		ModelInitData initData;
		initData.m_tkmFilePath = FLAME_MODEL_PATH;
		initData.m_fxFilePath = EMISSIVE_SHADER_PATH;
		initData.m_vsEntryPointFunc = "VSMain";
		initData.m_psEntryPointFunc = "PSMain";
		initData.m_expandConstantBuffer = &m_jetEmission;
		initData.m_expandConstantBufferSize = sizeof(m_jetEmission);
		initData.m_alphaBlendMode = AlphaBlendMode_Add;
		initData.m_isDepthWrite = false;
		initData.m_cullMode = D3D12_CULL_MODE_NONE;
		jet.flame.InitForwardRendering(initData);
		jet.flame.SetShadowCasterFlag(false);
		jet.boneId = m_model.FindBoneID(JET_BONES[i]);
		jet.fallbackOffset = JET_FALLBACK_OFFSETS[i];
	}
	m_jetLight = std::make_unique<PointLight>();
	m_jetLight->Init();
	m_jetLight->SetColor(JET_LIGHT_COLOR);
	m_jetLight->SetRange(JET_LIGHT_RANGE);
	m_jetLight->SetAffectPowParam(JET_LIGHT_POW);
	m_jetLight->SetPosition(m_position);
	m_jetLight->Update();
}

void Boss::UpdateAlive()
{
	float deltaTime = g_gameTime->GetFrameDeltaTime();
	if (m_hitCooldown > 0.0f) {
		m_hitCooldown -= deltaTime;
	}

	UpdateHover(deltaTime);
	if (!m_isBattle) {
		UpdateWait();
		m_moveSpeed.x = 0.0f;
		m_moveSpeed.z = 0.0f;
	}
	else {
		UpdateMove(deltaTime);
		TurnToPlayer(deltaTime);
		UpdateAttack(deltaTime);
	}
	m_rotation.SetRotationDegY(m_yaw);
	UpdateAnimation();
	UpdateJets(deltaTime);
	UpdateJetSound();
}

void Boss::UpdateWait()
{
	if (m_player == nullptr || !CanSeePlayer()) {
		return;
	}
	Vector3 toPlayer = m_player->GetPosition() - m_position;
	toPlayer.y = 0.0f;
	if (toPlayer.Length() <= ACTIVATE_RANGE) {
		m_isBattle = true;
		StartRest(FIRST_ATTACK_DELAY);
	}
}

void Boss::UpdateHover(float deltaTime)
{
	m_hoverPhase += BOB_SPEED * deltaTime;
	bool isFloorFound = false;
	float floorY = FindFloorY(m_position, isFloorFound);
	if (!isFloorFound) {
		floorY = m_position.y - HOVER_HEIGHT;
	}
	float targetY = floorY + HOVER_HEIGHT + sinf(m_hoverPhase) * BOB_HEIGHT;
	bool isCeilingFound = false;
	float ceilingY = FindCeilingY(m_position, isCeilingFound);
	if (isCeilingFound) {
		targetY = min(targetY, ceilingY - BODY_HEIGHT - CEILING_MARGIN);
	}
	targetY = max(targetY, floorY + MIN_HOVER_HEIGHT);
	m_moveSpeed.y = MathUtil::Clamp((targetY - m_position.y) * HEIGHT_GAIN, -MAX_VERTICAL_SPEED, MAX_VERTICAL_SPEED);
}

void Boss::UpdateMove(float deltaTime)
{
	m_moveTimer -= deltaTime;
	if (m_moveTimer <= 0.0f || !m_hasMoveTarget) {
		m_hasMoveTarget = ChooseMoveTarget();
		m_moveTimer = MOVE_INTERVAL_MIN + Random01() * MOVE_INTERVAL_RANGE;
	}

	Vector3 desired = Vector3::Zero;
	if (m_hasMoveTarget) {
		Vector3 toTarget = m_moveTarget - m_position;
		toTarget.y = 0.0f;
		float distance = toTarget.Length();
		if (distance > MOVE_ARRIVE_DISTANCE) {
			toTarget.Normalize();
			desired = toTarget * MOVE_SPEED * MathUtil::Clamp01(distance / MOVE_SLOW_DISTANCE);
		}
	}
	float rate = 1.0f - expf(-MOVE_ACCEL * deltaTime);
	m_horizontalSpeed.x += (desired.x - m_horizontalSpeed.x) * rate;
	m_horizontalSpeed.z += (desired.z - m_horizontalSpeed.z) * rate;
	m_moveSpeed.x = m_horizontalSpeed.x;
	m_moveSpeed.z = m_horizontalSpeed.z;
}

bool Boss::ChooseMoveTarget()
{
	if (m_player == nullptr) {
		return false;
	}
	Vector3 center = m_player->GetPosition();
	for (int i = 0; i < MOVE_TRY_NUM; i++) {
		float angle = Random01() * Math::PI2;
		float distance = ORBIT_MIN + Random01() * (ORBIT_MAX - ORBIT_MIN);
		Vector3 candidate = { center.x + sinf(angle) * distance, m_position.y, center.z + cosf(angle) * distance };
		bool isFloorFound = false;
		FindFloorY(candidate, isFloorFound);
		if (!isFloorFound) {
			continue;
		}
		Vector3 from = m_position;
		from.y += MOVE_PATH_HEIGHT;
		Vector3 to = candidate;
		to.y += MOVE_PATH_HEIGHT;
		Vector3 hitPos;
		if (PhysicsWorld::GetInstance()->RayTest(from, to, hitPos)) {
			continue;
		}
		m_moveTarget = candidate;
		return true;
	}
	return false;
}

void Boss::UpdateAttack(float deltaTime)
{
	m_attackTimer -= deltaTime;
	switch (m_attack) {
	case enAttack_Rest:
		if (m_attackTimer > 0.0f) {
			break;
		}
		if ((m_attackCount + 1) % GRENADE_EVERY == 0) {
			m_attackCount++;
			m_attack = enAttack_Grenade;
			m_throwsLeft = GRENADE_COUNT;
			StartThrow();
		}
		else if (CanSeePlayer()) {
			m_attackCount++;
			m_attack = enAttack_Rifle;
			m_shotsLeft = RIFLE_BURST;
			m_attackTimer = 0.0f;
		}
		else {
			m_attackTimer = NO_SIGHT_RETRY_TIME;
		}
		break;
	case enAttack_Rifle:
		if (m_attackTimer > 0.0f) {
			break;
		}
		if (m_shotsLeft <= 0 || !CanSeePlayer()) {
			StartRest(RIFLE_REST_TIME);
			break;
		}
		ShootRifle();
		m_shotsLeft--;
		m_attackTimer = RIFLE_INTERVAL;
		break;
	case enAttack_Grenade:
		if (!m_isGrenadeReleased && m_attackTimer <= THROW_TIME - THROW_RELEASE_TIME) {
			ReleaseGrenade();
		}
		if (m_attackTimer > 0.0f) {
			break;
		}
		m_throwsLeft--;
		if (m_throwsLeft > 0) {
			StartThrow();
		}
		else {
			StartRest(GRENADE_REST_TIME);
		}
		break;
	}
}

void Boss::StartRest(float time)
{
	m_attack = enAttack_Rest;
	m_attackTimer = time;
}

void Boss::ShootRifle()
{
	Vector3 muzzle = GetBonePosition(m_muzzleBoneId, MUZZLE_FALLBACK_OFFSET);
	Vector3 direction = GetPlayerAimPosition() - muzzle;
	direction.Normalize();
	direction.x += RandomRange(SHOT_SPREAD);
	direction.y += RandomRange(SHOT_SPREAD);
	direction.z += RandomRange(SHOT_SPREAD);
	direction.Normalize();

	PlayClip(enAnimationClip_Shoot, 1.0f, SHOOT_BLEND_TIME);
	m_isAnimationLocked = true;

	EnemyBullet* bullet = NewGO<EnemyBullet>(0, ObjectName::ENEMY_BULLET);
	bullet->Init(muzzle, direction, SHOT_DAMAGE);
	EffectManager* effect = EffectManager::Get();
	if (effect != nullptr) {
		effect->PlayMuzzleFlash(muzzle, direction, FLASH_SCALE);
	}
	SoundList::PlayAt(SoundId::EnemyShot, SHOT_VOLUME, muzzle, SHOT_SOUND_RANGE);
}

void Boss::StartThrow()
{
	m_attackTimer = THROW_TIME;
	m_isGrenadeReleased = false;
	PlayClip(enAnimationClip_Throw, THROW_ANIMATION_SPEED, ANIMATION_BLEND_TIME);
	m_isAnimationLocked = true;
}

void Boss::ReleaseGrenade()
{
	m_isGrenadeReleased = true;
	if (m_player == nullptr) {
		return;
	}
	Vector3 start = GetBonePosition(m_rightHandBoneId, HAND_FALLBACK_OFFSET);
	Vector3 target = m_player->GetPosition();
	target.x += RandomRange(GRENADE_TARGET_SPREAD);
	target.z += RandomRange(GRENADE_TARGET_SPREAD);
	Vector3 toTarget = target - start;
	float horizontal = sqrtf(toTarget.x * toTarget.x + toTarget.z * toTarget.z);
	float flightTime = MathUtil::Clamp(horizontal / GRENADE_FLIGHT_SPEED, GRENADE_FLIGHT_MIN, GRENADE_FLIGHT_MAX);
	Vector3 velocity = toTarget / flightTime;
	velocity.y += 0.5f * GRAVITY * flightTime;

	Grenade* grenade = NewGO<Grenade>(0, ObjectName::GRENADE);
	grenade->Init(start, velocity, GRENADE_DAMAGE);
}

void Boss::TurnToPlayer(float deltaTime)
{
	if (m_player == nullptr) {
		return;
	}
	Vector3 toPlayer = m_player->GetPosition() - m_position;
	toPlayer.y = 0.0f;
	if (toPlayer.LengthSq() < 1.0f) {
		return;
	}
	float diff = AngleDiff(m_yaw, MathUtil::YawFromDirection(toPlayer));
	float step = TURN_SPEED * deltaTime;
	m_yaw += fabsf(diff) <= step ? diff : (diff > 0.0f ? step : -step);
}

void Boss::PlayClip(EnAnimationClip clip, float speed, float blendTime)
{
	m_model.PlayAnimation(clip, blendTime);
	m_model.SetAnimationSpeed(speed);
	m_currentAnimation = clip;
}

void Boss::UpdateAnimation()
{
	if (m_isAnimationLocked) {
		if (m_model.IsPlayingAnimation()) {
			return;
		}
		m_isAnimationLocked = false;
	}
	if (m_currentAnimation != enAnimationClip_Idle) {
		PlayClip(enAnimationClip_Idle, 1.0f, ANIMATION_BLEND_TIME);
	}
}

void Boss::UpdateJets(float deltaTime)
{
	if (!m_isJetOn) {
		return;
	}
	m_jetFlicker += JET_FLICKER_SPEED * deltaTime;
	Quaternion down;
	down.SetRotation(Vector3::AxisZ, Vector3::Down);
	Vector3 lightPosition = Vector3::Zero;
	for (int i = 0; i < 2; i++) {
		Jet& jet = m_jets[i];
		Vector3 position = GetBonePosition(jet.boneId, jet.fallbackOffset);
		float flicker = 1.0f + sinf(m_jetFlicker + i * 1.7f) * JET_FLICKER_RATE + RandomRange(JET_FLICKER_RATE * 0.5f);
		float scale = JET_SCALE * flicker;
		jet.flame.SetPosition(position);
		jet.flame.SetRotation(down);
		jet.flame.SetScale({ scale, scale, scale * JET_LENGTH_RATE });
		jet.flame.Update();
		lightPosition += position * 0.5f;
	}
	lightPosition.y -= JET_LIGHT_DOWN;
	float flicker = 1.0f + sinf(m_jetFlicker * 0.7f) * JET_FLICKER_RATE;
	m_jetLight->SetPosition(lightPosition);
	m_jetLight->SetColor(JET_LIGHT_COLOR * flicker);
	m_jetLight->Update();
}

void Boss::UpdateJetSound()
{
	if (m_jetSound == nullptr) {
		return;
	}
	float distance = (m_position - g_camera3D->GetPosition()).Length();
	float rate = MathUtil::Clamp01(1.0f - distance / JET_SOUND_RANGE);
	m_jetSound->SetVolume(JET_SOUND_VOLUME * rate * rate);
}

void Boss::StopJets()
{
	m_isJetOn = false;
	if (m_jetLight != nullptr) {
		m_jetLight->SetColor(Vector3::Zero);
		m_jetLight->Update();
	}
	SoundList::StopLoop(m_jetSound);
}

bool Boss::CanSeePlayer() const
{
	if (m_player == nullptr || m_player->IsDead() || m_player->IsUnderground()) {
		return false;
	}
	Vector3 eye = m_position;
	eye.y += EYE_HEIGHT;
	Vector3 target = GetPlayerAimPosition();
	if ((target - eye).Length() > SIGHT_RANGE) {
		return false;
	}
	Vector3 direction = target - eye;
	direction.Normalize();
	return !IsSightBlocked(eye, target - direction * SIGHT_RAY_MARGIN);
}

bool Boss::IsSightBlocked(const Vector3& start, const Vector3& end) const
{
	Vector3 direction = end - start;
	direction.Normalize();
	Vector3 rayStart = start;
	for (int i = 0; i < SIGHT_RAY_RETRY_NUM; i++) {
		Vector3 hitPos;
		if (!PhysicsWorld::GetInstance()->RayTest(rayStart, end, hitPos)) {
			return false;
		}
		bool isCharacter = false;
		QueryGOs<EnemyBase>(ObjectName::ENEMY, [&](EnemyBase* enemy) {
			float rate = 0.0f;
			if (enemy->HitTest(hitPos, hitPos, SIGHT_CHARACTER_MARGIN, rate)) {
				isCharacter = true;
				return false;
			}
			return true;
		});
		if (!isCharacter) {
			return true;
		}
		rayStart = hitPos + direction * SIGHT_CHARACTER_SKIP;
		if ((end - rayStart).Dot(direction) <= 0.0f) {
			return false;
		}
	}
	return true;
}

float Boss::FindFloorY(const Vector3& position, bool& isFound) const
{
	Vector3 start = position;
	start.y -= 2.0f;
	Vector3 end = position;
	end.y -= FLOOR_RAY_LENGTH;
	Vector3 hitPos;
	isFound = PhysicsWorld::GetInstance()->RayTest(start, end, hitPos);
	return isFound ? hitPos.y : end.y;
}

float Boss::FindCeilingY(const Vector3& position, bool& isFound) const
{
	Vector3 start = position;
	start.y += BODY_HEIGHT + 5.0f;
	Vector3 end = position;
	end.y += CEILING_RAY_LENGTH;
	Vector3 hitPos;
	isFound = PhysicsWorld::GetInstance()->RayTest(start, end, hitPos);
	return isFound ? hitPos.y : end.y;
}

Vector3 Boss::GetBonePosition(int boneId, const Vector3& fallbackOffset) const
{
	if (boneId >= 0) {
		const Matrix& world = m_model.GetBone(boneId)->GetWorldMatrix();
		return { world.m[3][0], world.m[3][1], world.m[3][2] };
	}
	Vector3 offset = fallbackOffset;
	m_rotation.Apply(offset);
	return m_position + offset;
}

Vector3 Boss::GetPlayerAimPosition() const
{
	Vector3 target = m_player->GetPosition();
	target.y += m_player->IsDiving() ? DIVING_AIM_HEIGHT : PLAYER_AIM_HEIGHT;
	return target;
}

void Boss::OnDamage()
{
	if (!m_isBattle) {
		m_isBattle = true;
		StartRest(FIRST_ATTACK_DELAY);
	}
	if (m_hitCooldown > 0.0f || m_attack == enAttack_Grenade) {
		return;
	}
	m_hitCooldown = HIT_COOLDOWN;
	PlayClip(enAnimationClip_Hit, HIT_ANIMATION_SPEED, SHOOT_BLEND_TIME);
	m_isAnimationLocked = true;
}

void Boss::OnDead()
{
	m_isAnimationLocked = true;
	PlayClip(enAnimationClip_Death, 1.0f, DEATH_BLEND_TIME);
	StopJets();
	bool isFound = false;
	m_deathFloorY = FindFloorY(m_position, isFound);
	if (!isFound) {
		m_deathFloorY = m_position.y;
	}
	m_deathSpeedY = 0.0f;
	m_deathTimer = 0.0f;
}

void Boss::UpdateDeath()
{
	float deltaTime = g_gameTime->GetFrameDeltaTime();
	m_deathTimer += deltaTime;
	if (m_position.y > m_deathFloorY) {
		m_deathSpeedY -= GRAVITY * deltaTime;
		m_position.y = max(m_deathFloorY, m_position.y + m_deathSpeedY * deltaTime);
	}
	m_model.SetPosition(m_position);
	m_model.SetRotation(m_rotation);
	m_model.SetScale({ m_modelScale, m_modelScale, m_modelScale });
	m_model.Update();
	if (m_deathTimer >= DEFEAT_DELAY) {
		m_isDefeated = true;
	}
}

void Boss::Render(RenderContext& rc)
{
	EnemyBase::Render(rc);
	if (IsDead() || !m_isJetOn) {
		return;
	}
	for (auto& jet : m_jets) {
		jet.flame.Draw(rc);
	}
}

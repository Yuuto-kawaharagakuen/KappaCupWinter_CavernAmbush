#include "stdafx.h"
#include "EnemyBase.h"
#include "HitUtil.h"
#include "SoundList.h"
#include "Player.h"
#include "ObjectNames.h"

namespace
{
	const float GRAVITY = 980.0f;
	const float DAMAGE_FLASH_TIME = 0.12f;
	const float DAMAGE_SCALE = 1.08f;
	const float DEATH_FALL_TIME = 0.4f;
	const float DEATH_SINK_TIME = 0.8f;
	const float DEATH_SINK_DEPTH = 120.0f;
	const float DEATH_LIE_TIME = 1.0f;
	const float HIT_VOLUME = 0.8f;
	const float DOWN_VOLUME = 1.0f;
	const float DOWN_SOUND_RANGE = 3000.0f;
}

EnemyBase::EnemyBase()
{
}

EnemyBase::~EnemyBase()
{
}

void EnemyBase::InitBase(const char* modelPath, int maxHp, float radius, float height, AnimationClip* animationClips, int numAnimationClips)
{
	m_maxHp = maxHp;
	m_hp = maxHp;
	m_radius = radius;
	m_height = height;
	m_model.SetRaytracingWorld(false);
	m_model.Init(modelPath, animationClips, numAnimationClips);
	m_charaCon.Init(radius, height, m_position);
	m_isInitialized = true;
}

bool EnemyBase::Start()
{
	m_worldPlayer = FindGO<Player>(ObjectName::PLAYER);
	OnStart();
	UpdateModel();
	return true;
}

void EnemyBase::Update()
{
	if (!m_isInitialized) {
		return;
	}
	if (m_worldPlayer != nullptr && m_worldPlayer->IsWorldPaused()) {
		return;
	}

	if (IsDead()) {
		UpdateDeath();
		return;
	}

	if (m_damageFlashTimer > 0.0f) {
		m_damageFlashTimer -= g_gameTime->GetFrameDeltaTime();
	}

	UpdateAlive();
	UpdateModel();
}

Vector3 EnemyBase::GetForward() const
{
	Vector3 forward = Vector3::AxisZ;
	m_rotation.Apply(forward);
	return forward;
}

bool EnemyBase::HitTest(const Vector3& start, const Vector3& end, float radius, float& hitRate) const
{
	if (!m_isInitialized || IsDead()) {
		return false;
	}

	return HitUtil::SegmentCapsule(start, end, radius, m_position, m_radius, m_height, hitRate);
}

bool EnemyBase::Damage(int amount, const Vector3& direction)
{
	if (IsDead()) {
		return false;
	}
	if (!CanTakeDamage(direction)) {
		return false;
	}

	m_hp -= amount;
	SoundList::Play(SoundId::EnemyHit, HIT_VOLUME);
	if (m_hp <= 0) {
		m_hp = 0;
		SoundList::PlayAt(SoundId::EnemyDown, DOWN_VOLUME, m_position, DOWN_SOUND_RANGE);
		m_deathTimer = 0.0f;
		m_charaCon.RemoveRigidBoby();
		OnDead();
		return true;
	}
	m_damageFlashTimer = DAMAGE_FLASH_TIME;
	OnDamage();
	return true;
}

void EnemyBase::UpdateDeath()
{
	m_deathTimer += g_gameTime->GetFrameDeltaTime();

	float fallTime = m_deathAnimationTime > 0.0f ? m_deathAnimationTime + DEATH_LIE_TIME : DEATH_FALL_TIME;
	float fallRate = m_deathAnimationTime > 0.0f ? 0.0f : MathUtil::Clamp01(m_deathTimer / DEATH_FALL_TIME);
	float sinkRate = MathUtil::Clamp01((m_deathTimer - fallTime) / DEATH_SINK_TIME);

	Quaternion fall;
	fall.SetRotationDegX(-90.0f * fallRate * fallRate);
	Quaternion rotation;
	rotation.Multiply(fall, m_rotation);

	Vector3 position = m_position;
	position.y -= DEATH_SINK_DEPTH * sinkRate;

	m_model.SetPosition(position);
	m_model.SetRotation(rotation);
	m_model.SetScale(Vector3::One);
	m_model.Update();

	if (sinkRate >= 1.0f) {
		DeleteGO(this);
	}
}

void EnemyBase::UpdateModel()
{
	float deltaTime = g_gameTime->GetFrameDeltaTime();
	if (m_charaCon.IsOnGround()) {
		m_moveSpeed.y = 0.0f;
	}
	m_moveSpeed.y -= GRAVITY * deltaTime;
	m_position = m_charaCon.Execute(m_moveSpeed, deltaTime);

	float scale = m_damageFlashTimer > 0.0f ? DAMAGE_SCALE : 1.0f;
	m_model.SetPosition(m_position);
	m_model.SetRotation(m_rotation);
	m_model.SetScale({ scale, scale, scale });
	m_model.Update();
}

void EnemyBase::Render(RenderContext& rc)
{
	if (!m_isInitialized) {
		return;
	}
	m_model.Draw(rc);
}

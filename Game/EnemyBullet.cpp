#include "stdafx.h"
#include "EnemyBullet.h"
#include "Player.h"
#include "EffectManager.h"
#include "SoundList.h"
#include "ObjectNames.h"

namespace
{
	const char* const MODEL_PATH = "Assets/modelData/EnemyBullet.tkm";
	const float BULLET_SPEED = 1800.0f;
	const float BULLET_RANGE = 3000.0f;
	const float BULLET_RADIUS = 8.0f;
	const float BULLET_SCALE = 1.0f;
	const float OPPOSITE_DOT = -0.999f;
	const float RICOCHET_VOLUME = 0.7f;
	const float RICOCHET_RANGE = 700.0f;
}

EnemyBullet::EnemyBullet()
{
}

EnemyBullet::~EnemyBullet()
{
}

void EnemyBullet::Init(const Vector3& position, const Vector3& direction, int damage)
{
	m_position = position;
	m_direction = direction;
	m_direction.Normalize();
	m_damage = damage;
}

bool EnemyBullet::Start()
{
	m_player = FindGO<Player>(ObjectName::PLAYER);
	m_model.Init(MODEL_PATH);
	m_model.SetShadowCasterFlag(false);
	m_model.SetScale({ BULLET_SCALE, BULLET_SCALE, BULLET_SCALE });
	Quaternion rotation;
	if (m_direction.Dot(Vector3::AxisZ) < OPPOSITE_DOT) {
		rotation.SetRotationDegY(180.0f);
	}
	else {
		rotation.SetRotation(Vector3::AxisZ, m_direction);
	}
	m_model.SetRotation(rotation);
	m_model.SetPosition(m_position);
	m_model.Update();
	return true;
}

void EnemyBullet::Update()
{
	if (m_isDead) {
		return;
	}
	if (m_player != nullptr && m_player->IsWorldPaused()) {
		return;
	}

	float step = BULLET_SPEED * g_gameTime->GetFrameDeltaTime();
	Vector3 next = m_position + m_direction * step;

	Vector3 hitPos;
	bool isWallHit = PhysicsWorld::GetInstance()->RayTest(m_position, next, hitPos);
	if (isWallHit && m_player != nullptr && m_player->IsBodyPoint(hitPos)) {
		isWallHit = false;
	}
	Vector3 end = isWallHit ? hitPos : next;

	if (m_player != nullptr && m_player->HitTest(m_position, end, BULLET_RADIUS)) {
		m_player->Damage(m_damage);
		m_isDead = true;
		DeleteGO(this);
		return;
	}

	m_position = next;
	m_traveled += step;
	m_model.SetPosition(m_position);
	m_model.Update();

	if (isWallHit) {
		EffectManager* effect = EffectManager::Get();
		if (effect != nullptr) {
			effect->PlayEnemyBulletImpact(hitPos, m_direction);
		}
		SoundList::PlayAt(SoundId::BulletRicochet, RICOCHET_VOLUME, hitPos, RICOCHET_RANGE);
	}
	if (m_traveled >= BULLET_RANGE || isWallHit) {
		m_isDead = true;
		DeleteGO(this);
	}
}

void EnemyBullet::Render(RenderContext& rc)
{
	m_model.Draw(rc);
}

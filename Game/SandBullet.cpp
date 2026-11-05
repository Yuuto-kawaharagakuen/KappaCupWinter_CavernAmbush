#include "stdafx.h"
#include "SandBullet.h"
#include "EnemyBase.h"
#include "PlayerUI.h"
#include "ObjectNames.h"

namespace
{
	const float BULLET_SPEED = 3000.0f;
	const float BULLET_RANGE = 800.0f;
	const float BULLET_RADIUS = 10.0f;
	const char* const MODEL_PATH = "Assets/modelData/SandBullet.tkm";
	const int BULLET_DAMAGE = 10;
}

SandBullet::SandBullet()
{
}

SandBullet::~SandBullet()
{
}

void SandBullet::Init(const Vector3& position, const Vector3& direction)
{
	m_position = position;
	m_direction = direction;
	m_direction.Normalize();
}

bool SandBullet::Start()
{
	m_model.Init(MODEL_PATH);
	m_model.SetShadowCasterFlag(false);
	m_model.SetPosition(m_position);
	m_model.Update();
	return true;
}

void SandBullet::Update()
{
	if (m_isDead) {
		return;
	}

	float step = BULLET_SPEED * g_gameTime->GetFrameDeltaTime();
	if (m_traveled + step > BULLET_RANGE) {
		step = BULLET_RANGE - m_traveled;
	}
	Vector3 next = m_position + m_direction * step;

	Vector3 hitPos;
	bool isWallHit = PhysicsWorld::GetInstance()->RayTest(m_position, next, hitPos);
	Vector3 end = isWallHit ? hitPos : next;

	EnemyBase* hitEnemy = nullptr;
	float nearestRate = 1.0f;
	QueryGOs<EnemyBase>(ObjectName::ENEMY, [&](EnemyBase* enemy) {
		float rate = 0.0f;
		if (enemy->HitTest(m_position, end, BULLET_RADIUS, rate) && rate <= nearestRate) {
			nearestRate = rate;
			hitEnemy = enemy;
		}
		return true;
	});

	if (hitEnemy != nullptr) {
		if (hitEnemy->Damage(BULLET_DAMAGE, m_direction)) {
			PlayerUI* playerUI = FindGO<PlayerUI>(ObjectName::PLAYER_UI);
			if (playerUI != nullptr) {
				playerUI->NotifyHit(m_position + (end - m_position) * nearestRate, BULLET_DAMAGE, hitEnemy->IsDead());
			}
		}
		OnHit();
		return;
	}
	if (isWallHit) {
		OnHit();
		return;
	}

	m_position = next;
	m_traveled += step;
	m_model.SetPosition(m_position);
	m_model.Update();

	if (m_traveled >= BULLET_RANGE) {
		OnHit();
	}
}

void SandBullet::OnHit()
{
	if (m_isDead) {
		return;
	}
	m_isDead = true;
	DeleteGO(this);
}

void SandBullet::Render(RenderContext& rc)
{
	m_model.Draw(rc);
}

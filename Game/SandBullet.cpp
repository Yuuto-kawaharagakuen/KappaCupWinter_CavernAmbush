#include "stdafx.h"
#include "SandBullet.h"

namespace
{
	const float BULLET_SPEED = 3000.0f;
	const float BULLET_RANGE = 800.0f;
	const float BULLET_RADIUS = 10.0f;
	const char* const COLLISION_NAME = "player_bullet";
}

SandBullet::SandBullet()
{
}

SandBullet::~SandBullet()
{
	if (m_collision != nullptr) {
		DeleteGO(m_collision);
	}
}

void SandBullet::Init(const Vector3& position, const Vector3& direction)
{
	m_position = position;
	m_direction = direction;
	m_direction.Normalize();
}

bool SandBullet::Start()
{
	m_collision = NewGO<CollisionObject>(0);
	m_collision->CreateSphere(m_position, Quaternion::Identity, BULLET_RADIUS);
	m_collision->SetName(COLLISION_NAME);
	m_collision->SetIsEnableAutoDelete(false);

	m_model.Init("Assets/modelData/SandBullet.tkm");
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
	if (PhysicsWorld::GetInstance()->RayTest(m_position, next, hitPos)) {
		OnHit();
		return;
	}

	m_position = next;
	m_traveled += step;
	m_collision->SetPosition(m_position);
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

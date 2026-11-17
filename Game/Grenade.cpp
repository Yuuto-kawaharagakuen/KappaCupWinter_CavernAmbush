#include "stdafx.h"
#include "Grenade.h"
#include "Player.h"
#include "EffectManager.h"
#include "SoundList.h"
#include "MathUtil.h"
#include "ObjectNames.h"

namespace
{
	const char* const MODEL_PATH = "Assets/modelData/Grenade.tkm";
	const float MODEL_SCALE = 1.6f;
	const float GRAVITY = 980.0f;
	const float FUSE_TIME = 2.0f;
	const float BOUNCE_RATE = 0.35f;
	const float SLIDE_RATE = 0.6f;
	const float WALL_BOUNCE_RATE = 0.4f;
	const float LAND_SPEED = 80.0f;
	const float HIT_BACK = 4.0f;
	const float SPIN_SPEED = 720.0f;
	const float EXPLODE_RADIUS = 300.0f;
	const float EXPLODE_MIN_RATE = 0.4f;
	const float PLAYER_AIM_HEIGHT = 100.0f;
	const float SIGHT_START_HEIGHT = 20.0f;
	const float FLASH_SCALE = 7.0f;
	const int SAND_BURST_COUNT = 3;
	const float EXPLODE_VOLUME = 1.0f;
	const float EXPLODE_SOUND_RANGE = 4500.0f;
}

Grenade::Grenade()
{
}

Grenade::~Grenade()
{
}

void Grenade::Init(const Vector3& position, const Vector3& velocity, int damage)
{
	m_position = position;
	m_velocity = velocity;
	m_damage = damage;
	m_spinAxis = { velocity.z, 0.0f, -velocity.x };
	if (m_spinAxis.LengthSq() < 0.01f) {
		m_spinAxis = Vector3::AxisX;
	}
	m_spinAxis.Normalize();
}

bool Grenade::Start()
{
	m_player = FindGO<Player>(ObjectName::PLAYER);
	m_model.Init(MODEL_PATH);
	m_model.SetScale({ MODEL_SCALE, MODEL_SCALE, MODEL_SCALE });
	m_model.SetPosition(m_position);
	m_model.Update();
	return true;
}

void Grenade::Update()
{
	if (m_player != nullptr && m_player->IsWorldPaused()) {
		return;
	}
	float deltaTime = g_gameTime->GetFrameDeltaTime();
	m_timer += deltaTime;
	if (!m_isLanded) {
		Move(deltaTime);
	}
	if (m_timer >= FUSE_TIME) {
		Explode();
		return;
	}
	m_model.SetPosition(m_position);
	m_model.SetRotation(m_rotation);
	m_model.Update();
}

void Grenade::Move(float deltaTime)
{
	m_velocity.y -= GRAVITY * deltaTime;
	Vector3 next = m_position + m_velocity * deltaTime;
	Vector3 hitPos;
	if (PhysicsWorld::GetInstance()->RayTest(m_position, next, hitPos)) {
		Vector3 back = m_velocity;
		back.Normalize();
		m_position = hitPos - back * HIT_BACK;
		if (m_velocity.y < 0.0f && fabsf(m_velocity.y) > sqrtf(m_velocity.x * m_velocity.x + m_velocity.z * m_velocity.z)) {
			m_velocity.y = -m_velocity.y * BOUNCE_RATE;
			m_velocity.x *= SLIDE_RATE;
			m_velocity.z *= SLIDE_RATE;
		}
		else {
			m_velocity.x = -m_velocity.x * WALL_BOUNCE_RATE;
			m_velocity.z = -m_velocity.z * WALL_BOUNCE_RATE;
		}
		if (m_velocity.Length() < LAND_SPEED) {
			m_velocity = Vector3::Zero;
			m_isLanded = true;
		}
	}
	else {
		m_position = next;
	}

	Quaternion spin;
	spin.SetRotationDeg(m_spinAxis, SPIN_SPEED * deltaTime * MathUtil::Clamp01(m_velocity.Length() / 300.0f));
	m_rotation.Multiply(spin);
}

void Grenade::Explode()
{
	Vector3 up = Vector3::AxisY;
	EffectManager* effect = EffectManager::Get();
	if (effect != nullptr) {
		effect->PlayMuzzleFlash(m_position, up, FLASH_SCALE);
		for (int i = 0; i < SAND_BURST_COUNT; i++) {
			effect->PlayBulletImpact(m_position, up, true);
		}
	}
	SoundList::PlayAt(SoundId::GrenadeExplode, EXPLODE_VOLUME, m_position, EXPLODE_SOUND_RANGE);

	if (m_player != nullptr && !m_player->IsDead()) {
		Vector3 target = m_player->GetPosition();
		target.y += PLAYER_AIM_HEIGHT;
		float distance = (target - m_position).Length();
		if (distance < EXPLODE_RADIUS) {
			Vector3 start = m_position;
			start.y += SIGHT_START_HEIGHT;
			Vector3 hitPos;
			Vector3 toTarget = target - start;
			toTarget.Normalize();
			Vector3 end = target - toTarget * 40.0f;
			if (!PhysicsWorld::GetInstance()->RayTest(start, end, hitPos)) {
				float rate = 1.0f - distance / EXPLODE_RADIUS * (1.0f - EXPLODE_MIN_RATE);
				m_player->Damage(static_cast<int>(m_damage * rate));
			}
		}
	}
	DeleteGO(this);
}

void Grenade::Render(RenderContext& rc)
{
	m_model.Draw(rc);
}

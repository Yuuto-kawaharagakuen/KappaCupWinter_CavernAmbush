#include "stdafx.h"
#include "SandSplash.h"

namespace
{
	const char* const GRAIN_MODEL_PATH = "Assets/modelData/SandGrain.tkm";
	const char* const BUMP_MODEL_PATH = "Assets/modelData/SandMound.tkm";
	const int DIVE_SPAWN_COUNT = 22;
	const float DIVE_SPAWN_RADIUS = 30.0f;
	const float DIVE_SIDE_SPEED_MIN = 120.0f;
	const float DIVE_SIDE_SPEED_MAX = 300.0f;
	const float DIVE_UP_SPEED_MIN = 220.0f;
	const float DIVE_UP_SPEED_MAX = 460.0f;
	const float SPAWN_HEIGHT = 4.0f;
	const int SURFACE_BUMP_COUNT = 7;
	const float BUMP_RADIUS_MIN = 15.0f;
	const float BUMP_RADIUS_MAX = 65.0f;
	const float BUMP_DELAY_MAX = 0.35f;
	const float BUMP_TIME = 0.45f;
	const float BUMP_SIZE_MIN = 0.22f;
	const float BUMP_SIZE_MAX = 0.4f;
	const float BUMP_HEIGHT_RATE = 1.6f;
	const float BUMP_SINK = 6.0f;
	const int BUMP_GRAIN_COUNT = 4;
	const float BUMP_GRAIN_DELAY = 0.12f;
	const float BUMP_GRAIN_SIDE_SPEED_MIN = 30.0f;
	const float BUMP_GRAIN_SIDE_SPEED_MAX = 110.0f;
	const float BUMP_GRAIN_UP_SPEED_MIN = 90.0f;
	const float BUMP_GRAIN_UP_SPEED_MAX = 220.0f;
	const float BUMP_GRAIN_SIZE_RATE = 0.7f;
	const float IMPACT_SPEED_MIN = 120.0f;
	const float IMPACT_SPEED_MAX = 320.0f;
	const float IMPACT_SPREAD = 0.75f;
	const float IMPACT_LIFT = 0.35f;
	const float IMPACT_OFFSET = 3.0f;
	const float GRAVITY = 1200.0f;
	const float AIR_DRAG = 1.2f;
	const float GRAIN_SIZE_MIN = 0.6f;
	const float GRAIN_SIZE_MAX = 1.7f;
	const float LIFE_MIN = 0.7f;
	const float LIFE_MAX = 1.1f;
	const float SHRINK_TIME = 0.35f;
	const float SPIN_SPEED_MAX = 720.0f;
	const float HIDDEN_Y = -100000.0f;

	float RandomRange(float minValue, float maxValue)
	{
		return minValue + (maxValue - minValue) * (static_cast<float>(rand()) / RAND_MAX);
	}

	Vector3 RandomSide()
	{
		float angle = RandomRange(0.0f, Math::PI2);
		return { sinf(angle), 0.0f, cosf(angle) };
	}

	void HideModel(ModelRender& model)
	{
		model.SetPosition({ 0.0f, HIDDEN_Y, 0.0f });
		model.Update();
	}
}

void SandSplash::Init()
{
	for (auto& particle : m_particles) {
		particle.model.Init(GRAIN_MODEL_PATH);
		particle.model.SetShadowCasterFlag(false);
		HideModel(particle.model);
	}
	for (auto& bump : m_bumps) {
		bump.model.Init(BUMP_MODEL_PATH);
		bump.model.SetShadowCasterFlag(false);
		HideModel(bump.model);
	}
}

void SandSplash::Play(const Vector3& position, bool isDive)
{
	if (isDive) {
		for (int i = 0; i < DIVE_SPAWN_COUNT; i++) {
			SpawnDiveGrain(position);
		}
		return;
	}
	for (int i = 0; i < SURFACE_BUMP_COUNT; i++) {
		SpawnBump(position);
	}
}

void SandSplash::PlayImpact(const Vector3& position, const Vector3& direction, int count, float sizeRate, float groundY)
{
	Vector3 back = direction * -1.0f;
	back.Normalize();
	for (int i = 0; i < count; i++) {
		Vector3 spread = { RandomRange(-1.0f, 1.0f), RandomRange(-1.0f, 1.0f), RandomRange(-1.0f, 1.0f) };
		Vector3 flyDirection = back + spread * IMPACT_SPREAD;
		flyDirection.y += IMPACT_LIFT;
		flyDirection.Normalize();
		Vector3 velocity = flyDirection * RandomRange(IMPACT_SPEED_MIN, IMPACT_SPEED_MAX);
		ResetParticle(NextParticle(), position + back * IMPACT_OFFSET, velocity, groundY, 0.0f, sizeRate);
	}
}

SandSplash::Particle& SandSplash::NextParticle()
{
	Particle& particle = m_particles[m_nextParticle];
	m_nextParticle = (m_nextParticle + 1) % PARTICLE_COUNT;
	return particle;
}

void SandSplash::ResetParticle(Particle& particle, const Vector3& position, const Vector3& velocity, float groundY, float delay, float sizeRate)
{
	particle.position = position;
	particle.velocity = velocity;
	particle.groundY = groundY;
	particle.delay = delay;
	particle.spinAxis = { RandomRange(-1.0f, 1.0f), RandomRange(-1.0f, 1.0f), RandomRange(-1.0f, 1.0f) };
	if (particle.spinAxis.LengthSq() < 0.01f) {
		particle.spinAxis = Vector3::Up;
	}
	particle.spinAxis.Normalize();
	particle.spinSpeed = RandomRange(-SPIN_SPEED_MAX, SPIN_SPEED_MAX);
	particle.rotation.SetRotationDeg(particle.spinAxis, RandomRange(0.0f, 360.0f));
	particle.size = RandomRange(GRAIN_SIZE_MIN, GRAIN_SIZE_MAX) * sizeRate;
	particle.life = RandomRange(LIFE_MIN, LIFE_MAX);
	particle.isLanded = false;
	HideModel(particle.model);
}

void SandSplash::SpawnDiveGrain(const Vector3& origin)
{
	Vector3 side = RandomSide();
	Vector3 position = origin + side * RandomRange(0.0f, DIVE_SPAWN_RADIUS);
	position.y += SPAWN_HEIGHT;
	Vector3 velocity = side * RandomRange(DIVE_SIDE_SPEED_MIN, DIVE_SIDE_SPEED_MAX);
	velocity.y = RandomRange(DIVE_UP_SPEED_MIN, DIVE_UP_SPEED_MAX);
	ResetParticle(NextParticle(), position, velocity, origin.y, 0.0f, 1.0f);
}

void SandSplash::SpawnBump(const Vector3& origin)
{
	Bump& bump = m_bumps[m_nextBump];
	m_nextBump = (m_nextBump + 1) % BUMP_COUNT;

	bump.position = origin + RandomSide() * RandomRange(BUMP_RADIUS_MIN, BUMP_RADIUS_MAX);
	bump.position.y = origin.y;
	bump.rotation.SetRotationDegY(RandomRange(0.0f, 360.0f));
	bump.size = RandomRange(BUMP_SIZE_MIN, BUMP_SIZE_MAX);
	bump.delay = RandomRange(0.0f, BUMP_DELAY_MAX);
	bump.time = 0.0f;
	bump.isActive = true;
	HideModel(bump.model);

	SpawnBumpGrains(bump.position, bump.delay + BUMP_GRAIN_DELAY);
}

void SandSplash::SpawnBumpGrains(const Vector3& center, float delay)
{
	for (int i = 0; i < BUMP_GRAIN_COUNT; i++) {
		Vector3 side = RandomSide();
		Vector3 position = center;
		position.y += SPAWN_HEIGHT;
		Vector3 velocity = side * RandomRange(BUMP_GRAIN_SIDE_SPEED_MIN, BUMP_GRAIN_SIDE_SPEED_MAX);
		velocity.y = RandomRange(BUMP_GRAIN_UP_SPEED_MIN, BUMP_GRAIN_UP_SPEED_MAX);
		ResetParticle(NextParticle(), position, velocity, center.y, delay, BUMP_GRAIN_SIZE_RATE);
	}
}

void SandSplash::Update(float deltaTime)
{
	for (auto& particle : m_particles) {
		UpdateParticle(particle, deltaTime);
	}
	for (auto& bump : m_bumps) {
		UpdateBump(bump, deltaTime);
	}
}

void SandSplash::UpdateParticle(Particle& particle, float deltaTime)
{
	if (particle.life <= 0.0f) {
		return;
	}
	if (particle.delay > 0.0f) {
		particle.delay -= deltaTime;
		return;
	}
	particle.life -= deltaTime;
	if (particle.life <= 0.0f) {
		HideModel(particle.model);
		return;
	}

	if (!particle.isLanded) {
		particle.velocity.y -= GRAVITY * deltaTime;
		float drag = 1.0f - AIR_DRAG * deltaTime;
		particle.velocity.x *= drag;
		particle.velocity.z *= drag;
		particle.position += particle.velocity * deltaTime;
		if (particle.position.y < particle.groundY && particle.velocity.y < 0.0f) {
			particle.position.y = particle.groundY;
			particle.isLanded = true;
		}
		Quaternion spin;
		spin.SetRotationDeg(particle.spinAxis, particle.spinSpeed * deltaTime);
		particle.rotation.Multiply(spin);
	}

	float shrinkRate = particle.life < SHRINK_TIME ? particle.life / SHRINK_TIME : 1.0f;
	float scale = particle.size * shrinkRate;
	particle.model.SetPosition(particle.position);
	particle.model.SetRotation(particle.rotation);
	particle.model.SetScale({ scale, scale, scale });
	particle.model.Update();
}

void SandSplash::UpdateBump(Bump& bump, float deltaTime)
{
	if (!bump.isActive) {
		return;
	}
	if (bump.delay > 0.0f) {
		bump.delay -= deltaTime;
		return;
	}
	bump.time += deltaTime;
	float rate = bump.time / BUMP_TIME;
	if (rate >= 1.0f) {
		bump.isActive = false;
		HideModel(bump.model);
		return;
	}

	float rise = sinf(rate * Math::PI);
	Vector3 position = bump.position;
	position.y -= (1.0f - rise) * BUMP_SINK;
	float width = bump.size * (0.7f + 0.3f * rise);
	float height = bump.size * BUMP_HEIGHT_RATE * rise;
	bump.model.SetPosition(position);
	bump.model.SetRotation(bump.rotation);
	bump.model.SetScale({ width, height, width });
	bump.model.Update();
}

void SandSplash::Draw(RenderContext& rc)
{
	for (auto& particle : m_particles) {
		if (particle.life > 0.0f && particle.delay <= 0.0f) {
			particle.model.Draw(rc);
		}
	}
	for (auto& bump : m_bumps) {
		if (bump.isActive && bump.delay <= 0.0f) {
			bump.model.Draw(rc);
		}
	}
}

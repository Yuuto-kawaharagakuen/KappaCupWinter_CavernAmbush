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
	const char* const DUST_MODEL_PATH = "Assets/modelData/SandDust.tkm";
	const char* const DUST_SHADER_PATH = "Assets/shader/SandDust.fx";
	const int SURFACE_DUST_COUNT = 32;
	const int TRAIL_DUST_COUNT = 4;
	const float TRAIL_BUMP_BACK = 45.0f;
	const float TRAIL_BUMP_SIDE = 15.0f;
	const float TRAIL_BUMP_SIZE_MIN = 0.25f;
	const float TRAIL_BUMP_SIZE_MAX = 0.35f;
	const float DUST_DRAG = 1.6f;
	const float DUST_BUOYANCY = 12.0f;
	const float DUST_FADE_IN_TIME = 0.15f;
	const Vector3 DUST_COLOR = { 0.42f, 0.38f, 0.32f };
	const float DUST_ROLL_SPEED_MAX = 40.0f;
	const float OPPOSITE_DOT = -0.999f;

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

const SandSplash::DustParam SandSplash::SURFACE_DUST = {
	90.0f, 5.0f, 45.0f, 25.0f, 110.0f, 35.0f, 130.0f, 1.2f, 2.2f, 2.2f, 1.1f, 2.0f, 0.35f, 0.55f
};

const SandSplash::DustParam SandSplash::TRAIL_DUST = {
	25.0f, 0.0f, 15.0f, 10.0f, 40.0f, 20.0f, 60.0f, 0.8f, 1.3f, 1.9f, 0.7f, 1.1f, 0.0f, 0.35f
};

void SandSplash::Init(bool hasDust)
{
	m_hasDust = hasDust;
	if (m_hasDust) {
		for (auto& dust : m_dusts) {
			ModelInitData initData;
			initData.m_tkmFilePath = DUST_MODEL_PATH;
			initData.m_fxFilePath = DUST_SHADER_PATH;
			initData.m_vsEntryPointFunc = "VSMain";
			initData.m_psEntryPointFunc = "PSMain";
			initData.m_expandConstantBuffer = &dust.color;
			initData.m_expandConstantBufferSize = sizeof(dust.color);
			initData.m_alphaBlendMode = AlphaBlendMode_Trans;
			initData.m_isDepthWrite = false;
			initData.m_cullMode = D3D12_CULL_MODE_NONE;
			dust.model.InitForwardRendering(initData);
			dust.model.SetShadowCasterFlag(false);
			HideModel(dust.model);
		}
	}
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
		Vector3 bumpPosition = position + RandomSide() * RandomRange(BUMP_RADIUS_MIN, BUMP_RADIUS_MAX);
		bumpPosition.y = position.y;
		Bump& bump = SpawnBump(bumpPosition, RandomRange(BUMP_SIZE_MIN, BUMP_SIZE_MAX), RandomRange(0.0f, BUMP_DELAY_MAX));
		SpawnBumpGrains(bump.position, bump.delay + BUMP_GRAIN_DELAY);
	}
	if (m_hasDust) {
		for (int i = 0; i < SURFACE_DUST_COUNT; i++) {
			SpawnDust(position, SURFACE_DUST);
		}
	}
}

void SandSplash::PlayTrail(const Vector3& position, const Vector3& backward)
{
	Vector3 side = Cross(Vector3::Up, backward);
	Vector3 bumpPosition = position + backward * TRAIL_BUMP_BACK + side * RandomRange(-TRAIL_BUMP_SIDE, TRAIL_BUMP_SIDE);
	bumpPosition.y = position.y;
	SpawnBump(bumpPosition, RandomRange(TRAIL_BUMP_SIZE_MIN, TRAIL_BUMP_SIZE_MAX), 0.0f);
	if (!m_hasDust) {
		return;
	}
	for (int i = 0; i < TRAIL_DUST_COUNT; i++) {
		SpawnDust(position + backward * TRAIL_BUMP_BACK, TRAIL_DUST);
	}
}

void SandSplash::SpawnDust(const Vector3& origin, const DustParam& param)
{
	Dust& dust = m_dusts[m_nextDust];
	m_nextDust = (m_nextDust + 1) % DUST_COUNT;

	Vector3 side = RandomSide();
	dust.position = origin + side * RandomRange(0.0f, param.radius);
	dust.position.y += RandomRange(param.heightMin, param.heightMax);
	dust.velocity = side * RandomRange(param.sideSpeedMin, param.sideSpeedMax);
	dust.velocity.y = RandomRange(param.upSpeedMin, param.upSpeedMax);
	dust.roll = RandomRange(0.0f, 360.0f);
	dust.rollSpeed = RandomRange(-DUST_ROLL_SPEED_MAX, DUST_ROLL_SPEED_MAX);
	dust.sizeStart = RandomRange(param.sizeMin, param.sizeMax);
	dust.sizeEnd = dust.sizeStart * param.growth;
	dust.alpha = param.alpha;
	dust.delay = RandomRange(0.0f, param.delayMax);
	dust.time = 0.0f;
	dust.life = RandomRange(param.lifeMin, param.lifeMax);
	dust.isActive = true;
	dust.color = { DUST_COLOR.x, DUST_COLOR.y, DUST_COLOR.z, 0.0f };
	HideModel(dust.model);
}

void SandSplash::UpdateDust(Dust& dust, float deltaTime)
{
	if (!dust.isActive) {
		return;
	}
	if (dust.delay > 0.0f) {
		dust.delay -= deltaTime;
		return;
	}
	dust.time += deltaTime;
	float rate = dust.time / dust.life;
	if (rate >= 1.0f) {
		dust.isActive = false;
		HideModel(dust.model);
		return;
	}

	float drag = 1.0f - DUST_DRAG * deltaTime;
	dust.velocity.x *= drag;
	dust.velocity.z *= drag;
	dust.velocity.y = dust.velocity.y * drag + DUST_BUOYANCY * deltaTime;
	dust.position += dust.velocity * deltaTime;
	dust.roll += dust.rollSpeed * deltaTime;

	float fadeIn = dust.time < DUST_FADE_IN_TIME ? dust.time / DUST_FADE_IN_TIME : 1.0f;
	float fadeOut = 1.0f - rate;
	dust.color.w = dust.alpha * fadeIn * fadeOut * fadeOut;

	Vector3 toCamera = g_camera3D->GetPosition() - dust.position;
	toCamera.Normalize();
	Quaternion face;
	if (toCamera.Dot(Vector3::AxisZ) < OPPOSITE_DOT) {
		face.SetRotationDegY(180.0f);
	}
	else {
		face.SetRotation(Vector3::AxisZ, toCamera);
	}
	Quaternion rotation;
	rotation.SetRotationDeg(Vector3::AxisZ, dust.roll);
	rotation.Multiply(face);

	float scale = dust.sizeStart + (dust.sizeEnd - dust.sizeStart) * sqrtf(rate);
	dust.model.SetPosition(dust.position);
	dust.model.SetRotation(rotation);
	dust.model.SetScale({ scale, scale, scale });
	dust.model.Update();
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

SandSplash::Bump& SandSplash::SpawnBump(const Vector3& position, float size, float delay)
{
	Bump& bump = m_bumps[m_nextBump];
	m_nextBump = (m_nextBump + 1) % BUMP_COUNT;

	bump.position = position;
	bump.rotation.SetRotationDegY(RandomRange(0.0f, 360.0f));
	bump.size = size;
	bump.delay = delay;
	bump.time = 0.0f;
	bump.isActive = true;
	HideModel(bump.model);
	return bump;
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
	if (m_hasDust) {
		for (auto& dust : m_dusts) {
			UpdateDust(dust, deltaTime);
		}
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
	if (m_hasDust) {
		for (auto& dust : m_dusts) {
			if (dust.isActive && dust.delay <= 0.0f) {
				dust.model.Draw(rc);
			}
		}
	}
}

#pragma once

class SandSplash
{
public:
	void Init(bool hasDust);
	void Play(const Vector3& position, bool isDive);
	void PlayTrail(const Vector3& position, const Vector3& backward);
	void PlayImpact(const Vector3& position, const Vector3& direction, int count, float sizeRate, float groundY);
	void Update(float deltaTime);
	void Draw(RenderContext& rc);

private:
	static const int PARTICLE_COUNT = 48;
	static const int BUMP_COUNT = 16;
	static const int DUST_COUNT = 90;

	struct Particle
	{
		ModelRender model;
		Vector3 position = Vector3::Zero;
		Vector3 velocity = Vector3::Zero;
		Vector3 spinAxis = Vector3::Up;
		Quaternion rotation = Quaternion::Identity;
		float spinSpeed = 0.0f;
		float size = 1.0f;
		float delay = 0.0f;
		float life = 0.0f;
		float groundY = 0.0f;
		bool isLanded = false;
	};

	struct Bump
	{
		ModelRender model;
		Vector3 position = Vector3::Zero;
		Quaternion rotation = Quaternion::Identity;
		float size = 1.0f;
		float delay = 0.0f;
		float time = 0.0f;
		bool isActive = false;
	};

	struct Dust
	{
		ModelRender model;
		Vector4 color = Vector4::White;
		Vector3 position = Vector3::Zero;
		Vector3 velocity = Vector3::Zero;
		float roll = 0.0f;
		float rollSpeed = 0.0f;
		float sizeStart = 1.0f;
		float sizeEnd = 1.0f;
		float alpha = 1.0f;
		float delay = 0.0f;
		float time = 0.0f;
		float life = 0.0f;
		bool isActive = false;
	};

	struct DustParam
	{
		float radius;
		float heightMin;
		float heightMax;
		float sideSpeedMin;
		float sideSpeedMax;
		float upSpeedMin;
		float upSpeedMax;
		float sizeMin;
		float sizeMax;
		float growth;
		float lifeMin;
		float lifeMax;
		float delayMax;
		float alpha;
	};

	static const DustParam SURFACE_DUST;
	static const DustParam TRAIL_DUST;

	void SpawnDust(const Vector3& origin, const DustParam& param);
	void UpdateDust(Dust& dust, float deltaTime);
	void SpawnDiveGrain(const Vector3& origin);
	Bump& SpawnBump(const Vector3& position, float size, float delay);
	void SpawnBumpGrains(const Vector3& center, float delay);
	Particle& NextParticle();
	void ResetParticle(Particle& particle, const Vector3& position, const Vector3& velocity, float groundY, float delay, float sizeRate);
	void UpdateParticle(Particle& particle, float deltaTime);
	void UpdateBump(Bump& bump, float deltaTime);

	Particle m_particles[PARTICLE_COUNT];
	Bump m_bumps[BUMP_COUNT];
	Dust m_dusts[DUST_COUNT];
	int m_nextParticle = 0;
	int m_nextBump = 0;
	int m_nextDust = 0;
	bool m_hasDust = false;
};

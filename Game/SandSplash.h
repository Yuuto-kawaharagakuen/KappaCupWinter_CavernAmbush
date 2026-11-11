#pragma once

class SandSplash
{
public:
	void Init();
	void Play(const Vector3& position, bool isDive);
	void Update(float deltaTime);
	void Draw(RenderContext& rc);

private:
	static const int PARTICLE_COUNT = 48;
	static const int BUMP_COUNT = 8;

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

	void SpawnDiveGrain(const Vector3& origin);
	void SpawnBump(const Vector3& origin);
	void SpawnBumpGrains(const Vector3& center, float delay);
	Particle& NextParticle();
	void ResetParticle(Particle& particle, const Vector3& position, const Vector3& velocity, float groundY, float delay, float sizeRate);
	void UpdateParticle(Particle& particle, float deltaTime);
	void UpdateBump(Bump& bump, float deltaTime);

	Particle m_particles[PARTICLE_COUNT];
	Bump m_bumps[BUMP_COUNT];
	int m_nextParticle = 0;
	int m_nextBump = 0;
};

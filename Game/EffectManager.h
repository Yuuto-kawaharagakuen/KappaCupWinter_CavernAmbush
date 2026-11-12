#pragma once

#include "SandSplash.h"

class EffectManager : public IGameObject
{
public:
	static EffectManager* Get();

	EffectManager();
	~EffectManager();
	bool Start() override;
	void Update() override;
	void Render(RenderContext& rc) override;

	void PlayMuzzleFlash(const Vector3& position, const Vector3& direction, float scale);
	void PlayBulletImpact(const Vector3& position, const Vector3& direction, bool isEnemyHit);
	void PlayEnemyBulletImpact(const Vector3& position, const Vector3& direction);

private:
	static const int FLASH_COUNT = 6;

	struct Flash
	{
		ModelRender model;
		std::unique_ptr<PointLight> light;
		float time = 0.0f;
		float scale = 1.0f;
		bool isActive = false;
	};

	float FindGroundY(const Vector3& position) const;
	void UpdateFlash(Flash& flash, float deltaTime);

	Flash m_flashes[FLASH_COUNT];
	int m_nextFlash = 0;
	float m_flashEmission = 1.0f;
	SandSplash m_impactSand;
};

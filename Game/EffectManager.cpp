#include "stdafx.h"
#include "EffectManager.h"
#include "ObjectNames.h"

namespace
{
	const char* const FLASH_MODEL_PATH = "Assets/modelData/MuzzleFlash.tkm";
	const char* const EMISSIVE_SHADER_PATH = "Assets/shader/Emissive.fx";
	const float FLASH_EMISSION = 2.2f;
	const float FLASH_TIME = 0.06f;
	const float FLASH_SCALE_MIN = 0.8f;
	const float FLASH_SCALE_MAX = 1.2f;
	const Vector3 FLASH_LIGHT_COLOR = { 9.0f, 6.0f, 2.5f };
	const float FLASH_LIGHT_RANGE = 450.0f;
	const float FLASH_LIGHT_POW = 1.5f;
	const float HIDDEN_Y = -100000.0f;
	const float OPPOSITE_DOT = -0.999f;
	const int SAND_HIT_COUNT = 7;
	const float SAND_HIT_SIZE = 0.6f;
	const int ENEMY_HIT_COUNT = 12;
	const float ENEMY_HIT_SIZE = 0.85f;
	const int ENEMY_BULLET_HIT_COUNT = 5;
	const float ENEMY_BULLET_HIT_SIZE = 0.45f;
	const float GROUND_RAY_START = 5.0f;
	const float GROUND_RAY_LENGTH = 600.0f;

	float RandomRange(float minValue, float maxValue)
	{
		return minValue + (maxValue - minValue) * (static_cast<float>(rand()) / RAND_MAX);
	}
}

EffectManager* EffectManager::Get()
{
	return FindGO<EffectManager>(ObjectName::EFFECT_MANAGER);
}

EffectManager::EffectManager()
{
}

EffectManager::~EffectManager()
{
}

bool EffectManager::Start()
{
	m_flashEmission = FLASH_EMISSION;
	for (auto& flash : m_flashes) {
		ModelInitData initData;
		initData.m_tkmFilePath = FLASH_MODEL_PATH;
		initData.m_fxFilePath = EMISSIVE_SHADER_PATH;
		initData.m_vsEntryPointFunc = "VSMain";
		initData.m_psEntryPointFunc = "PSMain";
		initData.m_expandConstantBuffer = &m_flashEmission;
		initData.m_expandConstantBufferSize = sizeof(m_flashEmission);
		initData.m_alphaBlendMode = AlphaBlendMode_Add;
		initData.m_isDepthWrite = false;
		initData.m_cullMode = D3D12_CULL_MODE_NONE;
		flash.model.InitForwardRendering(initData);
		flash.model.SetShadowCasterFlag(false);
		flash.model.SetPosition({ 0.0f, HIDDEN_Y, 0.0f });
		flash.model.Update();

		flash.light = std::make_unique<PointLight>();
		flash.light->Init();
		flash.light->SetColor(Vector3::Zero);
		flash.light->SetRange(FLASH_LIGHT_RANGE);
		flash.light->SetAffectPowParam(FLASH_LIGHT_POW);
		flash.light->SetPosition({ 0.0f, HIDDEN_Y, 0.0f });
		flash.light->Update();
	}
	m_impactSand.Init();
	return true;
}

void EffectManager::PlayMuzzleFlash(const Vector3& position, const Vector3& direction, float scale)
{
	Flash& flash = m_flashes[m_nextFlash];
	m_nextFlash = (m_nextFlash + 1) % FLASH_COUNT;

	Vector3 forward = direction;
	forward.Normalize();
	Quaternion rotation;
	if (forward.Dot(Vector3::AxisZ) < OPPOSITE_DOT) {
		rotation.SetRotationDegY(180.0f);
	}
	else {
		rotation.SetRotation(Vector3::AxisZ, forward);
	}
	Quaternion roll;
	roll.SetRotationDeg(Vector3::AxisZ, RandomRange(0.0f, 360.0f));
	roll.Multiply(rotation);

	flash.scale = scale * RandomRange(FLASH_SCALE_MIN, FLASH_SCALE_MAX);
	flash.time = 0.0f;
	flash.isActive = true;
	flash.model.SetPosition(position);
	flash.model.SetRotation(roll);
	flash.model.SetScale({ flash.scale, flash.scale, flash.scale });
	flash.model.Update();
	flash.light->SetPosition(position);
	flash.light->SetColor(FLASH_LIGHT_COLOR);
	flash.light->Update();
}

float EffectManager::FindGroundY(const Vector3& position) const
{
	Vector3 start = position;
	start.y += GROUND_RAY_START;
	Vector3 end = position;
	end.y -= GROUND_RAY_LENGTH;
	Vector3 hitPosition;
	if (PhysicsWorld::GetInstance()->RayTest(start, end, hitPosition)) {
		return hitPosition.y;
	}
	return end.y;
}

void EffectManager::PlayBulletImpact(const Vector3& position, const Vector3& direction, bool isEnemyHit)
{
	int count = isEnemyHit ? ENEMY_HIT_COUNT : SAND_HIT_COUNT;
	float size = isEnemyHit ? ENEMY_HIT_SIZE : SAND_HIT_SIZE;
	m_impactSand.PlayImpact(position, direction, count, size, FindGroundY(position));
}

void EffectManager::PlayEnemyBulletImpact(const Vector3& position, const Vector3& direction)
{
	m_impactSand.PlayImpact(position, direction, ENEMY_BULLET_HIT_COUNT, ENEMY_BULLET_HIT_SIZE, FindGroundY(position));
}

void EffectManager::Update()
{
	float deltaTime = g_gameTime->GetFrameDeltaTime();
	for (auto& flash : m_flashes) {
		UpdateFlash(flash, deltaTime);
	}
	m_impactSand.Update(deltaTime);
}

void EffectManager::UpdateFlash(Flash& flash, float deltaTime)
{
	if (!flash.isActive) {
		flash.light->Update();
		return;
	}
	flash.time += deltaTime;
	float rate = flash.time / FLASH_TIME;
	if (rate >= 1.0f) {
		flash.isActive = false;
		flash.model.SetPosition({ 0.0f, HIDDEN_Y, 0.0f });
		flash.model.Update();
		flash.light->SetColor(Vector3::Zero);
		flash.light->Update();
		return;
	}
	float fade = 1.0f - rate;
	float scale = flash.scale * (0.7f + 0.3f * fade);
	flash.model.SetScale({ scale, scale, scale });
	flash.model.Update();
	flash.light->SetColor(FLASH_LIGHT_COLOR * fade);
	flash.light->Update();
}

void EffectManager::Render(RenderContext& rc)
{
	for (auto& flash : m_flashes) {
		if (flash.isActive) {
			flash.model.Draw(rc);
		}
	}
	m_impactSand.Draw(rc);
}

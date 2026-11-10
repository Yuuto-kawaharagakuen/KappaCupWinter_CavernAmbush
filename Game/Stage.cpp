#include "stdafx.h"
#include "Stage.h"

namespace
{
	const Vector3 CAVE_AMBIENT = { 0.16f, 0.13f, 0.10f };
	const int SUN_LIGHT_NO = 0;
	const int OUTDOOR_PARAM_LIGHT_NO = 3;
	const Vector3 OUTDOOR_SUN_DIRECTION = { -0.6f, -0.75f, -0.3f };
	const Vector3 OUTDOOR_SUN_COLOR = { 2.4f, 2.15f, 1.8f };
	const Vector3 OUTDOOR_AMBIENT = { 0.55f, 0.53f, 0.5f };
	const float OUTDOOR_BLEND_LENGTH = 1400.0f;
	const Vector3 DAYLIGHT_COLOR = { 5.5f, 5.0f, 4.2f };
	const float DAYLIGHT_RANGE = 1700.0f;
	const float DAYLIGHT_POW = 1.6f;
	const Vector3 LAMP_COLOR = { 7.0f, 4.2f, 1.8f };
	const float LAMP_RANGE = 850.0f;
	const float LAMP_POW = 1.8f;
	const char* const LANTERN_FRAME_PATH = "Assets/modelData/Prop_Lantern.tkm";
	const char* const LANTERN_GLASS_PATH = "Assets/modelData/Prop_LanternGlass.tkm";
	const char* const LANTERN_ROPE_PATH = "Assets/modelData/Prop_LanternRope.tkm";
	const char* const EMISSIVE_SHADER_PATH = "Assets/shader/Emissive.fx";
	const float LANTERN_EMISSION = 1.6f;
	const float LANTERN_TOP = 35.0f;
	const float LANTERN_ROPE_MODEL_LENGTH = 100.0f;
	const float LANTERN_ROPE_DEFAULT_LENGTH = 200.0f;
	const float LANTERN_CEILING_RAY_LENGTH = 1200.0f;
	const char* const HOLE_MODEL_PATH = "Assets/modelData/StageHole.tkm";
	const float HOLE_RAY_TOP = 280.0f;
	const float HOLE_RAY_BOTTOM = -600.0f;
	const float HOLE_FLOOR_OFFSET = 8.0f;
	const Vector3 BARRIER_SIZE = { 40.0f, 1200.0f, 1200.0f };
	const char* const SKY_MODEL_PATH = "Assets/modelData/preset/sky.tkm";
	const char* const SKY_SHADER_PATH = "Assets/shader/SkyCubeMap.fx";
	const wchar_t* const SKY_TEXTURE_PATH = L"Assets/modelData/preset/skyCubeMapWild.dds";
	const float SKY_SCALE = 1800.0f;
	const float SKY_LUMINANCE = 1.0f;
}

Stage::Stage()
{
}

Stage::~Stage()
{
}

void Stage::Load(const char* levelFilePath)
{
	m_levelRender.Init(levelFilePath, [&](LevelObjectData& objData) {
		if (objData.EqualObjectName(L"PlayerStart")) {
			m_playerStartPosition = objData.position;
			return true;
		}
		if (objData.ForwardMatchName(L"Hole_")) {
			m_holePosition = objData.position;
			return true;
		}
		if (objData.EqualObjectName(L"EntranceBarrier")) {
			InitBarrier(objData.position);
			return true;
		}
		if (objData.ForwardMatchName(L"Desert_")) {
			InitOutdoorModel(objData);
			return true;
		}
		if (objData.ForwardMatchName(L"Light_")) {
			AddPointLight(objData);
			return true;
		}
		return false;
	});
	InitSky();
	InitLighting();
	for (const Vector3& lampPosition : m_lampPositions) {
		AddLantern(lampPosition);
	}

	Vector3 rayStart = { m_holePosition.x, HOLE_RAY_TOP, m_holePosition.z };
	Vector3 rayEnd = { m_holePosition.x, HOLE_RAY_BOTTOM, m_holePosition.z };
	Vector3 hitPosition;
	if (PhysicsWorld::GetInstance()->RayTest(rayStart, rayEnd, hitPosition)) {
		m_holePosition.y = hitPosition.y;
	}

	Vector3 holeModelPosition = m_holePosition;
	holeModelPosition.y += HOLE_FLOOR_OFFSET;
	m_holeModel.SetRaytracingWorld(false);
	m_holeModel.Init(HOLE_MODEL_PATH);
	m_holeModel.SetShadowCasterFlag(false);
	m_holeModel.SetPosition(holeModelPosition);
	m_holeModel.Update();
}

void Stage::InitLighting()
{
	g_renderingEngine->SetAmbient(CAVE_AMBIENT);
	if (!m_hasOutdoorModel) {
		ResetLighting();
		return;
	}
	Vector3 sunDirection = OUTDOOR_SUN_DIRECTION;
	sunDirection.Normalize();
	g_renderingEngine->SetDirectionLight(SUN_LIGHT_NO, sunDirection, OUTDOOR_SUN_COLOR);
	Vector3 outdoorParam = { m_outdoorStartX, OUTDOOR_BLEND_LENGTH, 1.0f };
	g_renderingEngine->SetDirectionLight(OUTDOOR_PARAM_LIGHT_NO, outdoorParam, OUTDOOR_AMBIENT);
}

void Stage::ResetLighting()
{
	g_renderingEngine->SetDirectionLight(SUN_LIGHT_NO, OUTDOOR_SUN_DIRECTION, Vector3::Zero);
	g_renderingEngine->SetDirectionLight(OUTDOOR_PARAM_LIGHT_NO, Vector3::Zero, Vector3::Zero);
}

void Stage::InitOutdoorModel(const LevelObjectData& objData)
{
	char modelPath[256];
	sprintf_s(modelPath, "Assets/modelData/%ls.tkm", objData.name);
	m_outdoorModel.Init(modelPath);
	m_outdoorModel.SetTRS(objData.position, objData.rotation, objData.scale);
	m_outdoorModel.Update();
	m_hasOutdoorModel = true;
}

void Stage::AddPointLight(const LevelObjectData& objData)
{
	auto light = std::make_unique<PointLight>();
	light->Init();
	light->SetPosition(objData.position);
	if (wcscmp(objData.name, L"Light_Daylight") == 0) {
		light->SetColor(DAYLIGHT_COLOR);
		light->SetRange(DAYLIGHT_RANGE);
		light->SetAffectPowParam(DAYLIGHT_POW);
	}
	else {
		light->SetColor(LAMP_COLOR);
		light->SetRange(LAMP_RANGE);
		light->SetAffectPowParam(LAMP_POW);
		m_lampPositions.push_back(objData.position);
	}
	light->Update();
	m_pointLights.push_back(std::move(light));
}

void Stage::AddLantern(const Vector3& position)
{
	auto lantern = std::make_unique<Lantern>();

	lantern->frame.Init(LANTERN_FRAME_PATH);
	lantern->frame.SetShadowCasterFlag(false);
	lantern->frame.SetPosition(position);
	lantern->frame.Update();

	m_lanternEmission = LANTERN_EMISSION;
	ModelInitData glassData;
	glassData.m_tkmFilePath = LANTERN_GLASS_PATH;
	glassData.m_fxFilePath = EMISSIVE_SHADER_PATH;
	glassData.m_vsEntryPointFunc = "VSMain";
	glassData.m_psEntryPointFunc = "PSMain";
	glassData.m_expandConstantBuffer = &m_lanternEmission;
	glassData.m_expandConstantBufferSize = sizeof(m_lanternEmission);
	lantern->glass.InitForwardRendering(glassData);
	lantern->glass.SetShadowCasterFlag(false);
	lantern->glass.SetPosition(position);
	lantern->glass.Update();

	Vector3 ropeBottom = position;
	ropeBottom.y += LANTERN_TOP;
	Vector3 rayEnd = ropeBottom;
	rayEnd.y += LANTERN_CEILING_RAY_LENGTH;
	float ropeLength = LANTERN_ROPE_DEFAULT_LENGTH;
	Vector3 hitPosition;
	if (PhysicsWorld::GetInstance()->RayTest(ropeBottom, rayEnd, hitPosition)) {
		ropeLength = hitPosition.y - ropeBottom.y;
	}
	lantern->rope.Init(LANTERN_ROPE_PATH);
	lantern->rope.SetShadowCasterFlag(false);
	lantern->rope.SetPosition(ropeBottom);
	lantern->rope.SetScale({ 1.0f, ropeLength / LANTERN_ROPE_MODEL_LENGTH, 1.0f });
	lantern->rope.Update();

	m_lanterns.push_back(std::move(lantern));
}

void Stage::InitBarrier(const Vector3& position)
{
	m_outdoorStartX = position.x;
	m_barrierCollider.Create(BARRIER_SIZE);
	RigidBodyInitData initData;
	initData.collider = &m_barrierCollider;
	initData.mass = 0.0f;
	initData.pos = position;
	initData.pos.y += BARRIER_SIZE.y * 0.5f;
	m_barrierRigidBody.Init(initData);
}

void Stage::InitSky()
{
	m_skyTexture.InitFromDDSFile(SKY_TEXTURE_PATH);
	m_skyLuminance = SKY_LUMINANCE;

	ModelInitData initData;
	initData.m_tkmFilePath = SKY_MODEL_PATH;
	initData.m_fxFilePath = SKY_SHADER_PATH;
	initData.m_vsEntryPointFunc = "VSMain";
	initData.m_psEntryPointFunc = "PSMain";
	initData.m_expandShaderResoruceView[0] = &m_skyTexture;
	initData.m_expandConstantBuffer = &m_skyLuminance;
	initData.m_expandConstantBufferSize = sizeof(m_skyLuminance);
	m_skyModel.InitForwardRendering(initData);
	m_skyModel.SetShadowCasterFlag(false);
	m_skyModel.SetScale({ SKY_SCALE, SKY_SCALE, SKY_SCALE });
	m_skyModel.Update();
}

void Stage::Update()
{
	m_levelRender.Update();
	m_skyModel.SetPosition(g_camera3D->GetPosition());
	m_skyModel.Update();
	for (auto& light : m_pointLights) {
		light->Update();
	}
}

void Stage::Render(RenderContext& rc)
{
	m_levelRender.Draw(rc);
	m_holeModel.Draw(rc);
	for (auto& lantern : m_lanterns) {
		lantern->frame.Draw(rc);
		lantern->glass.Draw(rc);
		lantern->rope.Draw(rc);
	}
	if (m_hasOutdoorModel) {
		g_renderingEngine->AddRenderObject(&m_outdoorModel);
	}
	m_skyModel.Draw(rc);
}

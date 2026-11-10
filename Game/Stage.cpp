#include "stdafx.h"
#include "Stage.h"

namespace
{
	Vector3 CAVE_AMBIENT = { 0.6f, 0.5f, 0.4f };
	const char* const HOLE_MODEL_PATH = "Assets/modelData/StageHole.tkm";
	const float HOLE_RAY_TOP = 280.0f;
	const float HOLE_RAY_BOTTOM = -600.0f;
	const float HOLE_FLOOR_OFFSET = 8.0f;
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
		return false;
	});

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

	g_renderingEngine->SetAmbient(CAVE_AMBIENT);
}

void Stage::Update()
{
	m_levelRender.Update();
}

void Stage::Render(RenderContext& rc)
{
	m_levelRender.Draw(rc);
	m_holeModel.Draw(rc);
}

#include "stdafx.h"
#include "Stage.h"

namespace
{
	const Vector3 CAVE_AMBIENT = { 0.6f, 0.5f, 0.4f };
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

	g_renderingEngine->SetAmbient(CAVE_AMBIENT);
}

void Stage::Update()
{
	m_levelRender.Update();
}

void Stage::Render(RenderContext& rc)
{
	m_levelRender.Draw(rc);
}

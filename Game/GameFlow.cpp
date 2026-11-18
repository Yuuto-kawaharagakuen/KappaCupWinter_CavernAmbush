#include "stdafx.h"
#include "GameFlow.h"
#include "ObjectNames.h"

namespace
{
	const char* const STAGE_LEVEL_PATHS[] = {
		"Assets/level3D/Stage1.tkl",
		"Assets/level3D/Stage2.tkl",
		"Assets/level3D/BossRoom.tkl",
	};
	const int STAGE_COUNT = sizeof(STAGE_LEVEL_PATHS) / sizeof(STAGE_LEVEL_PATHS[0]);
	const int BOSS_STAGE = STAGE_COUNT - 1;
}

GameFlow* GameFlow::Get()
{
	GameFlow* flow = FindGO<GameFlow>(ObjectName::GAME_FLOW);
	if (flow == nullptr) {
		flow = NewGO<GameFlow>(0, ObjectName::GAME_FLOW);
	}
	return flow;
}

void GameFlow::StartNewGame()
{
	m_currentStage = 0;
	m_retryStage = 0;
	m_hasKey = false;
	m_totalTime = 0.0f;
	m_retryCount = 0;
}

void GameFlow::Retry()
{
	m_currentStage = m_retryStage;
	m_hasKey = false;
	m_retryCount++;
}

void GameFlow::AddPlayTime(float time)
{
	m_totalTime += time;
}

bool GameFlow::ClearCurrentStage(float stageTime)
{
	AddPlayTime(stageTime);
	m_hasKey = false;
	if (m_currentStage + 1 >= STAGE_COUNT) {
		return true;
	}
	m_currentStage++;
	m_retryStage = m_currentStage;
	return false;
}

bool GameFlow::CanRetry() const
{
	return m_currentStage != BOSS_STAGE;
}

const char* GameFlow::GetStageLevelPath() const
{
	return STAGE_LEVEL_PATHS[m_currentStage];
}

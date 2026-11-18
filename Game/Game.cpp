#include "stdafx.h"
#include "Game.h"
#include "Player.h"
#include "Stage.h"
#include "PlayerUI.h"
#include "GameOver.h"
#include "GameClear.h"
#include "GameFlow.h"
#include "Fade.h"
#include "Soldier.h"
#include "EnemyBullet.h"
#include "Boss.h"
#include "Grenade.h"
#include "EffectManager.h"
#include "SoundList.h"
#include "ObjectNames.h"

namespace
{
	const float GAME_OVER_DELAY = 1.0f;
	const float FLOOR_RAY_TOP = 280.0f;
	const float FLOOR_RAY_BOTTOM = -600.0f;
	const float SPAWN_HEIGHT_OFFSET = 5.0f;
	const float EXIT_RADIUS = 120.0f;
	const float BGM_VOLUME = 0.35f;
	const float BGM_FADE_IN_TIME = 1.5f;
	const float BGM_FADE_OUT_TIME = 1.0f;
	const int SOLDIER_STAGE = 0;
	const float BOSS_START_YAW = -90.0f;
	const float BOSS_DOOR_RADIUS = 220.0f;
	struct SoldierData
	{
		float x;
		float z;
		float patrolX;
		float patrolZ;
		float yaw;
	};
	const SoldierData SOLDIERS[] = {
		{ -2356.0f, 0.0f, -2356.0f, 0.0f, -65.0f },
		{ -857.0f, -536.0f, -428.0f, -804.0f, 0.0f },
		{ -321.0f, 321.0f, 321.0f, 214.0f, 90.0f },
		{ 1339.0f, 161.0f, 1768.0f, -54.0f, -90.0f },
		{ 2463.0f, -482.0f, 2946.0f, -536.0f, -90.0f },
		{ 2946.0f, 161.0f, 2946.0f, 161.0f, -90.0f },
	};

	Vector3 FindFloorPosition(float x, float z, float fallbackY)
	{
		Vector3 top = { x, FLOOR_RAY_TOP, z };
		Vector3 bottom = { x, FLOOR_RAY_BOTTOM, z };
		Vector3 hitPos;
		if (PhysicsWorld::GetInstance()->RayTest(top, bottom, hitPos)) {
			hitPos.y += SPAWN_HEIGHT_OFFSET;
			return hitPos;
		}
		return { x, fallbackY, z };
	}}

Game::Game()
{
	

}

Game::~Game()
{
	SoundList::StopLoop(m_bgm);
	DeleteGO(m_player);
	DeleteGO(m_stage);
	DeleteGO(m_playerUI);
	DeleteGO(m_effectManager);
	QueryGOs<EnemyBase>(ObjectName::ENEMY, [](EnemyBase* enemy) {
		DeleteGO(enemy);
		return true;
	});
	QueryGOs<EnemyBullet>(ObjectName::ENEMY_BULLET, [](EnemyBullet* bullet) {
		DeleteGO(bullet);
		return true;
	});
	QueryGOs<Grenade>(ObjectName::GRENADE, [](Grenade* grenade) {
		DeleteGO(grenade);
		return true;
	});
}

bool Game::Start()
{
	m_fade = Fade::Get();
	m_flow = GameFlow::Get();
	SoundList::Init();
	m_bgm = SoundList::PlayLoop(SoundId::GameBGM, 0.0f);

	m_stage = NewGO<Stage>(0, ObjectName::STAGE);
	m_stage->Load(m_flow->GetStageLevelPath());
	m_effectManager = NewGO<EffectManager>(0, ObjectName::EFFECT_MANAGER);

	m_player = NewGO<Player>(0, ObjectName::PLAYER);
	m_player->SetPosition(m_stage->GetPlayerStartPosition());
	m_player->SetStartDiving(m_stage->IsPlayerStartDiving());

	m_playerUI = NewGO<PlayerUI>(1, ObjectName::PLAYER_UI);
	m_playerUI->SetPlayer(m_player);

	float fallbackY = m_stage->GetPlayerStartPosition().y;
	if (m_flow->GetCurrentStage() == SOLDIER_STAGE) {
		SpawnSoldiers(fallbackY);
	}
	if (m_stage->HasBossStart()) {
		m_boss = NewGO<Boss>(0, ObjectName::ENEMY);
		m_boss->SetPosition(m_stage->GetBossStartPosition());
		m_boss->SetYaw(BOSS_START_YAW);
	}

	return true;
}

void Game::SpawnSoldiers(float fallbackY)
{
	for (const auto& data : SOLDIERS) {
		Vector3 start = FindFloorPosition(data.x, data.z, fallbackY);
		Vector3 patrol = FindFloorPosition(data.patrolX, data.patrolZ, fallbackY);
		Soldier* soldier = NewGO<Soldier>(0, ObjectName::ENEMY);
		soldier->SetPosition(start);
		soldier->SetYaw(data.yaw);
		soldier->AddPatrolPoint(start);
		soldier->AddPatrolPoint(patrol);
	}
}

void Game::Update()
{
	UpdateBgm();
	if (m_isStageClear) {
		UpdateStageClear();
		return;
	}
	if (m_player->IsDead()) {
		UpdateGameOver();
		return;
	}

	if (!m_player->IsWorldPaused()) {
		m_clearTime += g_gameTime->GetFrameDeltaTime();
	}
	if (m_boss != nullptr && m_boss->IsDefeated()) {
		m_isStageClear = true;
		m_fade->StartFadeOut();
		return;
	}
	UpdateExit();
}

void Game::UpdateBgm()
{
	if (m_bgm == nullptr) {
		return;
	}
	float deltaTime = g_gameTime->GetFrameDeltaTime();
	if (m_isStageClear || m_isGameOver) {
		m_bgmVolume = max(0.0f, m_bgmVolume - BGM_VOLUME * deltaTime / BGM_FADE_OUT_TIME);
	}
	else {
		m_bgmVolume = min(BGM_VOLUME, m_bgmVolume + BGM_VOLUME * deltaTime / BGM_FADE_IN_TIME);
	}
	m_bgm->SetVolume(m_bgmVolume);
}

void Game::UpdateExit()
{
	if (m_player->IsSwitching()) {
		return;
	}
	bool isExit = false;
	if (m_stage->HasExit() && m_player->IsDiving()) {
		isExit = IsNear(m_stage->GetExitPosition(), EXIT_RADIUS);
	}
	if (m_stage->HasBossDoor() && !m_player->IsDiving()) {
		isExit = isExit || IsNear(m_stage->GetBossDoorPosition(), BOSS_DOOR_RADIUS);
	}
	if (!isExit) {
		return;
	}

	m_isStageClear = true;
	m_fade->StartFadeOut();
}

bool Game::IsNear(const Vector3& position, float radius) const
{
	Vector3 diff = position - m_player->GetPosition();
	diff.y = 0.0f;
	return diff.Length() <= radius;
}

void Game::UpdateStageClear()
{
	if (m_fade->IsBlack()) {
		if (m_flow->ClearCurrentStage(m_clearTime)) {
			NewGO<GameClear>(0, ObjectName::GAME_CLEAR);
		}
		else {
			NewGO<Game>(0, ObjectName::GAME);
		}
		m_fade->StartFadeIn();
		DeleteGO(this);
	}
}

void Game::UpdateGameOver()
{
	if (!m_isGameOver) {
		m_gameOverTimer += g_gameTime->GetFrameDeltaTime();
		if (m_gameOverTimer >= GAME_OVER_DELAY) {
			m_isGameOver = true;
			m_fade->StartFadeOut();
		}
		return;
	}

	if (m_fade->IsBlack()) {
		m_flow->AddPlayTime(m_clearTime);
		NewGO<GameOver>(0, ObjectName::GAME_OVER);
		m_fade->StartFadeIn();
		DeleteGO(this);
	}
}

void Game::Render(RenderContext& rc)
{
}

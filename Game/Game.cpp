#include "stdafx.h"
#include "Game.h"
#include "Player.h"
#include "Stage.h"
#include "PlayerUI.h"
#include "GameOver.h"
#include "Fade.h"
#include "Soldier.h"
#include "EnemyBullet.h"

namespace
{
	const float GAME_OVER_DELAY = 1.0f;
	const float FLOOR_RAY_TOP = 400.0f;
	const float FLOOR_RAY_BOTTOM = -600.0f;
	const float SPAWN_HEIGHT_OFFSET = 5.0f;
	struct SoldierData
	{
		float x;
		float z;
		float patrolX;
		float patrolZ;
		float yaw;
	};
	const SoldierData SOLDIERS[] = {
		{ -3366.0f, 0.0f, -3443.0f, 689.0f, 90.0f },
		{ -2601.0f, 0.0f, -2984.0f, -230.0f, -90.0f },
		{ -1224.0f, -765.0f, -612.0f, -1148.0f, 0.0f },
		{ -459.0f, 459.0f, 459.0f, 306.0f, 90.0f },
		{ 612.0f, -995.0f, 612.0f, -995.0f, -90.0f },
		{ 1148.0f, -153.0f, 383.0f, -536.0f, -90.0f },
		{ 1913.0f, 230.0f, 2525.0f, -77.0f, -90.0f },
		{ 3519.0f, -689.0f, 4208.0f, -765.0f, -90.0f },
		{ 4208.0f, 230.0f, 4208.0f, 230.0f, -90.0f },
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
	//PhysicsWorld::GetInstance()->EnableDrawDebugWireFrame();

}

Game::~Game()
{
	DeleteGO(m_player);
	DeleteGO(m_stage);
	DeleteGO(m_playerUI);
	QueryGOs<EnemyBase>("enemy", [](EnemyBase* enemy) {
		DeleteGO(enemy);
		return true;
	});
	QueryGOs<EnemyBullet>("enemyBullet", [](EnemyBullet* bullet) {
		DeleteGO(bullet);
		return true;
	});
}

bool Game::Start()
{
	m_fade = FindGO<Fade>("fade");
	if (m_fade == nullptr) {
		m_fade = NewGO<Fade>(10, "fade");
	}

	m_stage = NewGO<Stage>(0, "stage");
	m_stage->Load("Assets/level3D/Stage1.tkl");

	m_player = NewGO<Player>(0, "player");
	m_player->SetPosition(m_stage->GetPlayerStartPosition());

	m_playerUI = NewGO<PlayerUI>(1, "playerUI");
	m_playerUI->SetPlayer(m_player);

	float fallbackY = m_stage->GetPlayerStartPosition().y;
	for (const auto& data : SOLDIERS) {
		Vector3 start = FindFloorPosition(data.x, data.z, fallbackY);
		Vector3 patrol = FindFloorPosition(data.patrolX, data.patrolZ, fallbackY);
		Soldier* soldier = NewGO<Soldier>(0, "enemy");
		soldier->SetPosition(start);
		soldier->SetYaw(data.yaw);
		soldier->AddPatrolPoint(start);
		soldier->AddPatrolPoint(patrol);
	}

	return true;
}

void Game::Update()
{
	if (!m_player->IsDead()) {
		return;
	}

	if (!m_isGameOver) {
		m_gameOverTimer += g_gameTime->GetFrameDeltaTime();
		if (m_gameOverTimer >= GAME_OVER_DELAY) {
			m_isGameOver = true;
			m_fade->StartFadeOut();
		}
		return;
	}

	if (m_fade->IsBlack()) {
		NewGO<GameOver>(0, "gameOver");
		m_fade->StartFadeIn();
		DeleteGO(this);
	}
}

void Game::Render(RenderContext& rc)
{
}

#include "stdafx.h"
#include "Game.h"
#include "Player.h"
#include "Stage.h"
#include "PlayerUI.h"

Game::Game()
{
	//PhysicsWorld::GetInstance()->EnableDrawDebugWireFrame();

}

Game::~Game()
{
	DeleteGO(m_player);
	DeleteGO(m_stage);
	DeleteGO(m_playerUI);
}

bool Game::Start()
{
	m_stage = NewGO<Stage>(0, "stage");
	m_stage->Load("Assets/level3D/Stage1.tkl");

	m_player = NewGO<Player>(0, "player");
	m_player->SetPosition(m_stage->GetPlayerStartPosition());

	m_playerUI = NewGO<PlayerUI>(1, "playerUI");
	m_playerUI->SetPlayer(m_player);

	return true;
}

void Game::Update()
{
}

void Game::Render(RenderContext& rc)
{
}

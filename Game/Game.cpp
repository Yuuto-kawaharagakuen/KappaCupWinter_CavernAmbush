#include "stdafx.h"
#include "Game.h"
#include "Player.h"
#include "Stage.h"

Game::Game()
{
}

Game::~Game()
{
	DeleteGO(m_player);
	DeleteGO(m_stage);
}

bool Game::Start()
{
	m_stage = NewGO<Stage>(0, "stage");
	m_stage->Load("Assets/level3D/Stage1.tkl");

	m_player = NewGO<Player>(0, "player");
	m_player->SetPosition(m_stage->GetPlayerStartPosition());

	return true;
}

void Game::Update()
{
}

void Game::Render(RenderContext& rc)
{
}

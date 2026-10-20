#include "stdafx.h"
#include "Game.h"
#include "Player.h"
#include "BackGround.h"

Game::Game()
{
}

Game::~Game()
{
	DeleteGO(m_player);
	DeleteGO(m_backGround);
}

bool Game::Start()
{
	m_backGround = NewGO<BackGround>(0, "backGround");
	m_player = NewGO<Player>(0, "player");

	OutputDebugStringW(L"---- CAVERN AMBUSH : Game Start ----\n");
	return true;
}

void Game::Update()
{
}

void Game::Render(RenderContext& rc)
{
}

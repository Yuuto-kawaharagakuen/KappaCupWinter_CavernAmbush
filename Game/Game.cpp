#include "stdafx.h"
#include "Game.h"

Game::Game()
{
}

Game::~Game()
{
}

bool Game::Start()
{
	g_camera3D->SetPosition(Vector3(0.0f, 200.0f, -400.0f));
	g_camera3D->SetTarget(Vector3(0.0f, 100.0f, 0.0f));
	g_camera3D->SetUp(Vector3(0.0f, 1.0f, 0.0f));
	g_camera3D->SetNear(1.0f);
	g_camera3D->SetFar(20000.0f);

	OutputDebugStringW(L"---- CAVERN AMBUSH : Game Start ----\n");
	return true;
}

void Game::Update()
{
}

void Game::Render(RenderContext& rc)
{
}
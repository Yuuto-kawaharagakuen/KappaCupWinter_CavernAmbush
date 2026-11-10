#pragma once
using namespace std;

class Player;
class Stage;
class PlayerUI;
class Fade;

class Game : public IGameObject
{
public:
	Game();
	~Game();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	float GetClearTime() const
	{
		return m_clearTime;
	}

private:
	void UpdateGameOver();
	void UpdateStageClear();
	void UpdateHole();
	int CountAliveEnemies() const;

	Player* m_player = nullptr;
	Stage* m_stage = nullptr;
	PlayerUI* m_playerUI = nullptr;
	Fade* m_fade = nullptr;
	bool m_isGameOver = false;
	float m_gameOverTimer = 0.0f;
	float m_clearTime = 0.0f;
	bool m_isHoleOpen = false;
	bool m_isStageClear = false;
};

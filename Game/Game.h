#pragma once
using namespace std;

class Player;
class Stage;
class PlayerUI;

class Game : public IGameObject
{
public:
	Game();
	~Game();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

private:
	Player* m_player = nullptr;
	Stage* m_stage = nullptr;
	PlayerUI* m_playerUI = nullptr;
};

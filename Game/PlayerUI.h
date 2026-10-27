#pragma once

class Player;

class PlayerUI : public IGameObject
{
public:
	PlayerUI();
	~PlayerUI();
	bool Start() override;
	void Update() override;
	void Render(RenderContext& rc) override;

	void SetPlayer(Player* player)
	{
		m_player = player;
	}

private:
	Player* m_player = nullptr;
	SpriteRender m_crosshair;
	FontRender m_ammoText;
	bool m_isCrosshairVisible = false;
};

#pragma once

class Fade;

class GameClear : public IGameObject
{
public:
	GameClear();
	~GameClear();
	bool Start() override;
	void Update() override;
	void Render(RenderContext& rc) override;

private:
	Fade* m_fade = nullptr;
	SpriteRender m_background;
	FontRender m_titleText;
	FontRender m_timeText;
	FontRender m_retryCountText;
	FontRender m_nextText;
	float m_timer = 0.0f;
	bool m_isDecided = false;
};

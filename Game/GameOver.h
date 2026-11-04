#pragma once

class Fade;

class GameOver : public IGameObject
{
public:
	GameOver();
	~GameOver();
	bool Start() override;
	void Update() override;
	void Render(RenderContext& rc) override;

private:
	Fade* m_fade = nullptr;
	SpriteRender m_background;
	FontRender m_titleText;
	FontRender m_retryText;
	float m_timer = 0.0f;
	bool m_isRetrying = false;
};

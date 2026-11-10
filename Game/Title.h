#pragma once

class Fade;

class Title : public IGameObject
{
public:
	Title();
	~Title();
	bool Start() override;
	void Update() override;
	void Render(RenderContext& rc) override;

private:
	Fade* m_fade = nullptr;
	SpriteRender m_background;
	FontRender m_titleText;
	FontRender m_startText;
	float m_timer = 0.0f;
	bool m_isStarting = false;
};

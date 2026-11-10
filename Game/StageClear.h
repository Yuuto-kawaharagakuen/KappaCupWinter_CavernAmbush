#pragma once

class Fade;

class StageClear : public IGameObject
{
public:
	StageClear();
	~StageClear();
	bool Start() override;
	void Update() override;
	void Render(RenderContext& rc) override;

	void SetClearTime(float clearTime)
	{
		m_clearTime = clearTime;
	}

private:
	Fade* m_fade = nullptr;
	SpriteRender m_background;
	FontRender m_titleText;
	FontRender m_timeText;
	FontRender m_retryText;
	float m_clearTime = 0.0f;
	float m_timer = 0.0f;
	bool m_isRetrying = false;
};

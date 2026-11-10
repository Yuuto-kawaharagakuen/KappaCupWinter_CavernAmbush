#pragma once

class Fade : public IGameObject
{
public:
	static Fade* Get();

	Fade();
	~Fade();
	bool Start() override;
	void Update() override;
	void Render(RenderContext& rc) override;

	void StartFadeOut();
	void StartFadeIn();
	bool IsFading() const
	{
		return m_state != enState_Idle;
	}
	bool IsBlack() const
	{
		return m_alpha >= 1.0f;
	}

private:
	enum EnState
	{
		enState_Idle,
		enState_FadeOut,
		enState_FadeIn,
	};

	SpriteRender m_sprite;
	EnState m_state = enState_Idle;
	float m_alpha = 0.0f;
};

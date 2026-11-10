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
	enum EnMenu
	{
		enMenu_Retry,
		enMenu_Title,
		enMenu_Num,
	};

	void UpdateSelect();
	void UpdateMenuText();

	Fade* m_fade = nullptr;
	SpriteRender m_background;
	FontRender m_titleText;
	FontRender m_menuText[enMenu_Num];
	FontRender m_cursorText;
	float m_timer = 0.0f;
	int m_select = enMenu_Retry;
	bool m_isStickTilted = false;
	bool m_isDecided = false;
};

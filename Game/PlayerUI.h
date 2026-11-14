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
	void NotifyHit(const Vector3& worldPosition, int damage, bool isKill);
	void ShowMessage(const wchar_t* text, float duration);

private:
	static const int AMMO_PIP_NUM = 10;
	static const int RELOAD_SEGMENT_NUM = 10;
	static const int CROSS_LINE_NUM = 4;
	static const int DAMAGE_POPUP_NUM = 16;
	static const int GUIDE_ROW_NUM = 3;

	enum EnGuideButton
	{
		enGuideButton_B,
		enGuideButton_X,
		enGuideButton_LT,
		enGuideButton_RT,
		enGuideButton_Num,
	};

	struct GuideRow
	{
		SpriteRender icons[enGuideButton_Num];
		FontRender label;
		EnGuideButton button = enGuideButton_B;
		bool isVisible = false;
		bool isDim = false;
	};

	struct GuideEntry
	{
		EnGuideButton button;
		const wchar_t* text;
		bool isDim;
	};

	struct DamagePopup
	{
		FontRender font;
		Vector3 worldPosition = Vector3::Zero;
		float timer = 0.0f;
		float scale = 1.0f;
		int digits = 1;
		bool isActive = false;
		bool isVisible = false;
	};

	void InitHp();
	void InitAmmo();
	void InitCrosshair();
	void InitStatus();
	void InitReload();
	void InitGuide();
	void UpdateGuide(float deltaTime);
	void UpdateHp(float deltaTime);
	void UpdateAmmo(float deltaTime);
	void UpdateCrosshair(float deltaTime);
	void UpdateStatus(float deltaTime);
	void UpdateReload(float deltaTime);
	void UpdateDamagePopups(float deltaTime);
	void UpdateVignette(float deltaTime);
	void UpdateSwitchFade();
	void UpdateScope();
	void UpdateMessage(float deltaTime);

	Player* m_player = nullptr;

	FontRender m_messageText;
	float m_messageTimer = 0.0f;

	SpriteRender m_vignette;
	SpriteRender m_switchFade;
	bool m_isSwitchFadeVisible = false;
	SpriteRender m_scope;
	float m_scopeAlpha = 0.0f;
	float m_damageFlash = 0.0f;
	float m_lowHpPulse = 0.0f;

	SpriteRender m_hpShade;
	SpriteRender m_hpBarBack;
	SpriteRender m_hpBarLag;
	SpriteRender m_hpBar;
	FontRender m_hpLabel;
	FontRender m_hpValue;
	float m_hpRate = 1.0f;
	float m_hpLagRate = 1.0f;
	float m_hpLagDelay = 0.0f;
	float m_hpShake = 0.0f;
	int m_lastHp = -1;

	SpriteRender m_ammoShade;
	SpriteRender m_ammoPips[AMMO_PIP_NUM];
	float m_ammoPipPop[AMMO_PIP_NUM] = {};
	FontRender m_ammoValue;
	FontRender m_ammoMax;
	FontRender m_ammoHint;
	float m_ammoPunch = 0.0f;
	float m_hintBlink = 0.0f;
	int m_lastAmmo = -1;
	bool m_isHintVisible = false;

	SpriteRender m_crossLines[CROSS_LINE_NUM];
	SpriteRender m_crossDot;
	SpriteRender m_hitLines[CROSS_LINE_NUM];
	float m_spread = 0.0f;
	float m_hitTimer = 0.0f;
	bool m_isHitKill = false;
	bool m_isCrosshairVisible = false;

	SpriteRender m_statusPill;
	FontRender m_statusDiveText;
	FontRender m_statusNoShotText;
	float m_statusAlpha = 0.0f;

	SpriteRender m_ringBack;
	SpriteRender m_ringSegments[RELOAD_SEGMENT_NUM];
	float m_ringSegmentPop[RELOAD_SEGMENT_NUM] = {};
	FontRender m_reloadLabel;
	FontRender m_reloadCount;
	float m_reloadAlpha = 0.0f;
	int m_lastReloadAmmo = -1;

	DamagePopup m_damagePopups[DAMAGE_POPUP_NUM];

	GuideRow m_guideRows[GUIDE_ROW_NUM];
	float m_guideAlpha = 0.0f;
};

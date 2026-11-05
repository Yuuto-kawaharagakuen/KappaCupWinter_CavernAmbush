#pragma once

class EnemyBase : public IGameObject
{
public:
	EnemyBase();
	virtual ~EnemyBase();
	bool Start() override;
	void Update() override;
	void Render(RenderContext& rc) override;

	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}
	const Vector3& GetPosition() const
	{
		return m_position;
	}
	bool IsDead() const
	{
		return m_hp <= 0;
	}
	bool HitTest(const Vector3& start, const Vector3& end, float radius, float& hitRate) const;
	bool Damage(int amount, const Vector3& direction);

protected:
	void InitBase(const char* modelPath, int maxHp, float radius, float height, AnimationClip* animationClips = nullptr, int numAnimationClips = 0);
	virtual void OnStart() {}
	virtual void UpdateAlive() {}
	virtual bool CanTakeDamage(const Vector3& direction) const
	{
		return true;
	}
	virtual void OnDamage() {}
	virtual void OnDead() {}
	Vector3 GetForward() const;

	CharacterController m_charaCon;
	ModelRender m_model;
	Vector3 m_position = Vector3::Zero;
	Quaternion m_rotation = Quaternion::Identity;
	Vector3 m_moveSpeed = Vector3::Zero;
	float m_deathAnimationTime = 0.0f;

private:
	void UpdateDeath();
	void UpdateModel();

	int m_hp = 1;
	int m_maxHp = 1;
	float m_radius = 0.0f;
	float m_height = 0.0f;
	float m_damageFlashTimer = 0.0f;
	float m_deathTimer = 0.0f;
	bool m_isInitialized = false;
};

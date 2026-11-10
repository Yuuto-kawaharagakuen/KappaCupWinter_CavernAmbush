#pragma once

class Player : public IGameObject
{
public:
	enum EnState
	{
		enState_Surface,
		enState_Dive,
	};

	Player();
	~Player();
	bool Start() override;
	void Update() override;
	void Render(RenderContext& rc) override;

	const Vector3& GetPosition() const
	{
		return m_position;
	}
	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}
	const Vector3& GetForward() const
	{
		return m_forward;
	}
	EnState GetState() const
	{
		return m_state;
	}
	bool IsDiving() const
	{
		return m_state == enState_Dive;
	}
	bool IsSwitching() const
	{
		return m_cameraBlend != (IsDiving() ? 1.0f : 0.0f);
	}
	Vector3 GetLookDirection() const;
	bool CanShoot() const;
	int GetAmmo() const
	{
		return m_ammo;
	}
	int GetMaxAmmo() const;
	bool IsReloading() const
	{
		return m_isReloading;
	}
	int GetHp() const
	{
		return m_hp;
	}
	int GetMaxHp() const;
	bool IsDead() const
	{
		return m_hp <= 0;
	}
	void Damage(int amount);
	bool HitTest(const Vector3& start, const Vector3& end, float radius) const;
	bool IsBodyPoint(const Vector3& point) const;
	bool IsMoving() const
	{
		return m_isMoving;
	}
	bool IsAiming() const
	{
		return m_aimBlend > 0.0f;
	}
	float GetAimRate() const;
	Vector3 GetMuzzlePosition() const;
	bool ConsumeShotFlag()
	{
		bool isShot = m_isShot;
		m_isShot = false;
		return isShot;
	}

private:
	void UpdateDamage();
	void UpdateState();
	void UpdateAim();
	void UpdateReload();
	void Shoot();
	void Rotate();
	void UpdateAxes();
	void Move();
	void UpdateCamera();
	Vector3 ResolveCameraCollision(const Vector3& target, const Vector3& desired) const;
	void UpdateGun();
	void UpdateMound();

	CharacterController m_charaCon;
	SphereCollider m_cameraCollider;
	ModelRender m_gunModel;
	ModelRender m_moundModel;
	EnState m_state = enState_Surface;
	Vector3 m_position = Vector3::Zero;
	Vector3 m_moveSpeed = Vector3::Zero;
	Vector3 m_forward = Vector3::AxisZ;
	Vector3 m_right = Vector3::AxisX;
	float m_yaw = 0.0f;
	float m_pitch = 0.0f;
	float m_cameraBlend = 0.0f;
	float m_shotCooldown = 0.0f;
	int m_ammo = 0;
	bool m_isReloading = false;
	float m_reloadTimer = 0.0f;
	bool m_isGunVisible = true;
	int m_hp = 1;
	float m_invincibleTimer = 0.0f;
	bool m_isMoving = false;
	bool m_isShot = false;
	float m_gunKick = 0.0f;
	float m_gunBobPhase = 0.0f;
	float m_gunBobWeight = 0.0f;
	float m_gunRaise = 0.0f;
	float m_gunSwayYaw = 0.0f;
	float m_gunSwayPitch = 0.0f;
	float m_lastYaw = 0.0f;
	float m_lastPitch = 0.0f;
	float m_moundPhase = 0.0f;
	float m_moundMoveWeight = 0.0f;
	bool m_isMoundVisible = false;
	float m_aimBlend = 0.0f;
	float m_baseViewAngle = 0.0f;
	Vector3 m_gunPosition = Vector3::Zero;
	Quaternion m_gunRotation = Quaternion::Identity;

};

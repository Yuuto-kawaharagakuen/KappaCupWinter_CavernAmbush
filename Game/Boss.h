#pragma once
#include "EnemyBase.h"

class Player;

class Boss : public EnemyBase
{
public:
	Boss();
	~Boss();

	void SetYaw(float yaw)
	{
		m_yaw = yaw;
	}
	bool IsDefeated() const
	{
		return m_isDefeated;
	}

protected:
	void OnStart() override;
	void UpdateAlive() override;
	void OnDamage() override;
	void OnDead() override;
	bool UseGravity() const override
	{
		return false;
	}
	void UpdateDeath() override;
	void Render(RenderContext& rc) override;

private:
	enum EnAnimationClip
	{
		enAnimationClip_Idle,
		enAnimationClip_Shoot,
		enAnimationClip_Throw,
		enAnimationClip_Death,
		enAnimationClip_Hit,
		enAnimationClip_Num,
	};
	enum EnAttack
	{
		enAttack_Rest,
		enAttack_Rifle,
		enAttack_Grenade,
	};

	void InitJets();
	void UpdateWait();
	void UpdateHover(float deltaTime);
	void UpdateMove(float deltaTime);
	bool ChooseMoveTarget();
	void UpdateAttack(float deltaTime);
	void StartRest(float time);
	void ShootRifle();
	void StartThrow();
	void ReleaseGrenade();
	void TurnToPlayer(float deltaTime);
	void PlayClip(EnAnimationClip clip, float speed, float blendTime);
	void UpdateAnimation();
	void UpdateJets(float deltaTime);
	void UpdateJetSound();
	void StopJets();
	bool CanSeePlayer() const;
	bool IsSightBlocked(const Vector3& start, const Vector3& end) const;
	float FindFloorY(const Vector3& position, bool& isFound) const;
	float FindCeilingY(const Vector3& position, bool& isFound) const;
	Vector3 GetBonePosition(int boneId, const Vector3& fallbackOffset) const;
	Vector3 GetPlayerAimPosition() const;

	struct Jet
	{
		ModelRender flame;
		int boneId = -1;
		Vector3 fallbackOffset = Vector3::Zero;
	};

	Player* m_player = nullptr;
	AnimationClip m_animationClips[enAnimationClip_Num];
	EnAnimationClip m_currentAnimation = enAnimationClip_Num;
	bool m_isAnimationLocked = false;
	bool m_isBattle = false;
	bool m_isDefeated = false;
	float m_yaw = 0.0f;
	float m_hoverPhase = 0.0f;
	Vector3 m_horizontalSpeed = Vector3::Zero;
	Vector3 m_moveTarget = Vector3::Zero;
	bool m_hasMoveTarget = false;
	float m_moveTimer = 0.0f;
	EnAttack m_attack = enAttack_Rest;
	float m_attackTimer = 0.0f;
	int m_attackCount = 0;
	int m_shotsLeft = 0;
	int m_throwsLeft = 0;
	bool m_isGrenadeReleased = false;
	float m_hitCooldown = 0.0f;
	int m_muzzleBoneId = -1;
	int m_rightHandBoneId = -1;
	Jet m_jets[2];
	bool m_isJetOn = true;
	std::unique_ptr<PointLight> m_jetLight;
	float m_jetEmission = 1.0f;
	float m_jetFlicker = 0.0f;
	SoundSource* m_jetSound = nullptr;
	float m_deathTimer = 0.0f;
	float m_deathSpeedY = 0.0f;
	float m_deathFloorY = 0.0f;
};

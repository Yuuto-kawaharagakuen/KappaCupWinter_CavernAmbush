#pragma once
#include "EnemyBase.h"

class Player;

class Soldier : public EnemyBase
{
public:
	enum EnState
	{
		enState_Patrol,
		enState_Alert,
		enState_Attack,
	};

	Soldier();
	~Soldier();

	void SetYaw(float yaw)
	{
		m_yaw = yaw;
	}
	void AddPatrolPoint(const Vector3& point)
	{
		m_patrolPoints.push_back(point);
	}
	EnState GetState() const
	{
		return m_state;
	}

protected:
	void OnStart() override;
	void UpdateAlive() override;
	void OnDamage() override;
	void OnDead() override;
	void Render(RenderContext& rc) override;

private:
	enum EnAnimationClip
	{
		enAnimationClip_Idle,
		enAnimationClip_Walk,
		enAnimationClip_Shoot,
		enAnimationClip_Death,
		enAnimationClip_Look,
		enAnimationClip_Aim,
		enAnimationClip_Num,
	};

	void UpdateStuck(float deltaTime);
	void UpdateAvoid(float deltaTime);
	void GiveUpMove();
	void AdvancePatrolPoint();
	void UpdatePatrol(float deltaTime);
	void UpdateAlert(float deltaTime);
	void UpdateAttack(float deltaTime);
	void ChangeState(EnState state);
	bool CanSeePlayer() const;
	bool CanSensePlayer() const;
	bool IsSightBlocked(const Vector3& start, const Vector3& end) const;
	bool MoveTo(const Vector3& target, float speed);
	void TurnTo(const Vector3& target, float deltaTime);
	void TurnToYaw(float targetYaw, float deltaTime);
	void ApproachLookingAround(const Vector3& target, float deltaTime);
	void Shoot();
	void UpdateMark(float deltaTime);
	void UpdateAnimation();
	Vector3 GetEyePosition() const;
	Vector3 GetPlayerAimPosition() const;

	Player* m_player = nullptr;
	AnimationClip m_animationClips[enAnimationClip_Num];
	EnAnimationClip m_currentAnimation = enAnimationClip_Num;
	bool m_isShooting = false;
	EnState m_state = enState_Patrol;
	std::vector<Vector3> m_patrolPoints;
	int m_patrolIndex = 0;
	float m_waitTimer = 0.0f;
	float m_yaw = 0.0f;
	Vector3 m_lastKnownPosition = Vector3::Zero;
	float m_lostTimer = 0.0f;
	float m_alertTimer = 0.0f;
	float m_suspicion = 0.0f;
	Vector3 m_prevPosition = Vector3::Zero;
	Vector3 m_avoidDirection = Vector3::Zero;
	float m_lastMoveIntent = 0.0f;
	float m_stuckTimer = 0.0f;
	float m_freeTimer = 0.0f;
	float m_avoidTimer = 0.0f;
	int m_avoidCount = 0;
	float m_lookPhase = 0.0f;
	float m_shotTimer = 0.0f;
	float m_reactionTimer = 0.0f;

	FontRender m_mark;
	float m_markTimer = 0.0f;
	bool m_isMarkVisible = false;
};

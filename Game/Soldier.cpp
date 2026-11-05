#include "stdafx.h"
#include "Soldier.h"
#include "Player.h"
#include "EnemyBullet.h"
#include "MathUtil.h"
#include "ObjectNames.h"

namespace
{
	const char* const MODEL_PATH = "Assets/modelData/SwatSoldier.tkm";
	const char* const ANIMATION_PATHS[] = {
		"Assets/animData/Swat_Idle.tka",
		"Assets/animData/Swat_Walk.tka",
		"Assets/animData/Swat_Shoot.tka",
		"Assets/animData/Swat_Death.tka",
		"Assets/animData/Swat_Look.tka",
		"Assets/animData/Swat_Aim.tka",
	};
	const float DEATH_ANIMATION_TIME = 36.0f / 30.0f;
	const float DEATH_BLEND_TIME = 0.1f;
	const float STUCK_MOVE_RATE = 0.25f;
	const float STUCK_TIME = 0.5f;
	const float AVOID_TIME = 0.7f;
	const float AVOID_SPEED = 160.0f;
	const float AVOID_BACK_RATE = 0.35f;
	const int AVOID_GIVE_UP_COUNT = 3;
	const float AVOID_COUNT_RESET_TIME = 3.0f;
	const float MOVE_INTENT_THRESHOLD = 10.0f;
	const float ANIMATION_BLEND_TIME = 0.2f;
	const float SHOOT_BLEND_TIME = 0.05f;
	const float WALK_ANIMATION_THRESHOLD = 10.0f;
	const int MAX_HP = 30;
	const float CAPSULE_RADIUS = 30.0f;
	const float CAPSULE_HEIGHT = 120.0f;

	const float WALK_SPEED = 140.0f;
	const float ALERT_SPEED = 220.0f;
	const float TURN_SPEED = 220.0f;
	const float ARRIVE_DISTANCE = 30.0f;
	const float PATROL_WAIT_TIME = 1.5f;

	const float EYE_HEIGHT = 165.0f;
	const float PLAYER_AIM_HEIGHT = 145.0f;
	const float SIGHT_RANGE = 1600.0f;
	const float SIGHT_HALF_ANGLE = 55.0f;
	const float SIGHT_RAY_MARGIN = 50.0f;
	const float SIGHT_RAY_START_MARGIN = 10.0f;
	const int SIGHT_RAY_RETRY_NUM = 4;
	const float SIGHT_CHARACTER_MARGIN = 5.0f;
	const float SIGHT_CHARACTER_SKIP = 70.0f;
	const float SENSE_RANGE = 450.0f;
	const float SENSE_HALF_ANGLE = 100.0f;
	const float SUSPICION_TIME = 5.0f;
	const float SUSPICION_DECAY = 0.5f;
	const float APPROACH_SPEED = 110.0f;
	const float APPROACH_STOP_DISTANCE = 130.0f;
	const float APPROACH_LOOK_ANGLE = 45.0f;
	const float APPROACH_LOOK_SPEED = 2.2f;
	const float DIVING_AIM_HEIGHT = 20.0f;
	const float REACTION_TIME = 0.45f;
	const float LOST_TIME = 2.0f;
	const float ALERT_LOOK_TIME = 3.0f;
	const float ALERT_LOOK_SPEED = 1.6f;
	const float ALERT_LOOK_ANGLE = 70.0f;

	const float SHOT_INTERVAL = 1.1f;
	const float FIRST_SHOT_DELAY = 0.5f;
	const float SHOT_FACING_ANGLE = 12.0f;
	const float SHOT_SPREAD = 0.035f;
	const int SHOT_DAMAGE = 10;
	const float MUZZLE_HEIGHT = 152.0f;
	const float MUZZLE_FORWARD = 62.0f;
	const float MUZZLE_RIGHT = -6.0f;

	const float MARK_HEIGHT = 215.0f;
	const float MARK_SCALE = 1.3f;
	const float MARK_POP_TIME = 0.25f;
	const float MARK_HALF_WIDTH = 10.0f;
	const Vector4 MARK_ALERT_COLOR = { 1.0f, 0.85f, 0.25f, 1.0f };
	const Vector4 MARK_ATTACK_COLOR = { 1.0f, 0.25f, 0.2f, 1.0f };
	const Vector4 MARK_SHADOW_COLOR = { 0.0f, 0.0f, 0.0f, 0.6f };
	const float MARK_SUSPICION_BLEND = 0.7f;

	float AngleDiff(float from, float to)
	{
		float diff = fmodf(to - from + 540.0f, 360.0f) - 180.0f;
		return diff;
	}

	float RandomRange(float range)
	{
		return (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f) * range;
	}
}

Soldier::Soldier()
{
}

Soldier::~Soldier()
{
}

void Soldier::OnStart()
{
	for (int i = 0; i < enAnimationClip_Num; i++) {
		m_animationClips[i].Load(ANIMATION_PATHS[i]);
		m_animationClips[i].SetLoopFlag(i != enAnimationClip_Shoot && i != enAnimationClip_Death);
	}
	InitBase(MODEL_PATH, MAX_HP, CAPSULE_RADIUS, CAPSULE_HEIGHT, m_animationClips, enAnimationClip_Num);
	m_player = FindGO<Player>(ObjectName::PLAYER);
	m_rotation.SetRotationDegY(m_yaw);
	m_deathAnimationTime = DEATH_ANIMATION_TIME;
	m_prevPosition = m_position;

	m_mark.SetFontType(enFontType::Bold);
	m_mark.SetScale(MARK_SCALE);
	m_mark.SetShadowParam(true, 2.0f, MARK_SHADOW_COLOR);
}

void Soldier::UpdateAlive()
{
	float deltaTime = g_gameTime->GetFrameDeltaTime();
	UpdateStuck(deltaTime);
	m_moveSpeed.x = 0.0f;
	m_moveSpeed.z = 0.0f;

	switch (m_state) {
	case enState_Patrol:
		UpdatePatrol(deltaTime);
		break;
	case enState_Alert:
		UpdateAlert(deltaTime);
		break;
	case enState_Attack:
		UpdateAttack(deltaTime);
		break;
	}

	UpdateAvoid(deltaTime);
	m_lastMoveIntent = sqrtf(m_moveSpeed.x * m_moveSpeed.x + m_moveSpeed.z * m_moveSpeed.z);
	m_rotation.SetRotationDegY(m_yaw);
	UpdateAnimation();
	UpdateMark(deltaTime);
}

void Soldier::UpdateAnimation()
{
	if (m_isShooting) {
		if (m_model.IsPlayingAnimation()) {
			return;
		}
		m_isShooting = false;
	}

	float speed = sqrtf(m_moveSpeed.x * m_moveSpeed.x + m_moveSpeed.z * m_moveSpeed.z);
	EnAnimationClip next = enAnimationClip_Idle;
	if (speed > WALK_ANIMATION_THRESHOLD) {
		next = enAnimationClip_Walk;
	}
	else if (m_state == enState_Alert) {
		next = enAnimationClip_Look;
	}
	else if (m_state == enState_Attack) {
		next = enAnimationClip_Aim;
	}
	if (next != m_currentAnimation) {
		m_model.PlayAnimation(next, ANIMATION_BLEND_TIME);
		m_currentAnimation = next;
	}
	m_model.SetAnimationSpeed(next == enAnimationClip_Walk ? speed / WALK_SPEED : 1.0f);
}

void Soldier::UpdateStuck(float deltaTime)
{
	Vector3 moved = m_position - m_prevPosition;
	moved.y = 0.0f;
	m_prevPosition = m_position;
	if (m_lastMoveIntent > MOVE_INTENT_THRESHOLD && m_avoidTimer <= 0.0f
		&& moved.Length() < m_lastMoveIntent * deltaTime * STUCK_MOVE_RATE) {
		m_stuckTimer += deltaTime;
		m_freeTimer = 0.0f;
		return;
	}
	m_stuckTimer = 0.0f;
	m_freeTimer += deltaTime;
	if (m_freeTimer >= AVOID_COUNT_RESET_TIME) {
		m_avoidCount = 0;
	}
}

void Soldier::UpdateAvoid(float deltaTime)
{
	if (m_avoidTimer > 0.0f) {
		m_avoidTimer -= deltaTime;
		m_moveSpeed.x = m_avoidDirection.x * AVOID_SPEED;
		m_moveSpeed.z = m_avoidDirection.z * AVOID_SPEED;
		TurnToYaw(MathUtil::YawFromDirection(m_avoidDirection), deltaTime);
		return;
	}
	if (m_stuckTimer < STUCK_TIME) {
		return;
	}

	m_stuckTimer = 0.0f;
	m_avoidCount++;
	if (m_avoidCount >= AVOID_GIVE_UP_COUNT) {
		m_avoidCount = 0;
		GiveUpMove();
		return;
	}

	Vector3 forward = GetForward();
	Vector3 right = { forward.z, 0.0f, -forward.x };
	float side = (m_avoidCount % 2 == 1) ? 1.0f : -1.0f;
	m_avoidDirection = right * side - forward * AVOID_BACK_RATE;
	m_avoidDirection.Normalize();
	m_avoidTimer = AVOID_TIME;
}

void Soldier::GiveUpMove()
{
	if (m_state == enState_Alert) {
		m_lastKnownPosition = m_position;
		return;
	}
	if (m_state == enState_Patrol && !m_patrolPoints.empty()) {
		AdvancePatrolPoint();
	}
}

void Soldier::AdvancePatrolPoint()
{
	m_patrolIndex = (m_patrolIndex + 1) % static_cast<int>(m_patrolPoints.size());
	m_waitTimer = PATROL_WAIT_TIME;
}

void Soldier::UpdatePatrol(float deltaTime)
{
	if (CanSeePlayer()) {
		TurnTo(m_player->GetPosition(), deltaTime);
		m_reactionTimer += deltaTime;
		if (m_reactionTimer >= REACTION_TIME) {
			ChangeState(enState_Attack);
		}
		return;
	}
	m_reactionTimer = 0.0f;

	if (CanSensePlayer()) {
		m_lastKnownPosition = m_player->GetPosition();
		ChangeState(enState_Alert);
		return;
	}

	if (m_patrolPoints.empty()) {
		return;
	}
	if (m_waitTimer > 0.0f) {
		m_waitTimer -= deltaTime;
		return;
	}
	if (MoveTo(m_patrolPoints[m_patrolIndex], WALK_SPEED)) {
		AdvancePatrolPoint();
	}
	else {
		TurnTo(m_patrolPoints[m_patrolIndex], deltaTime);
	}
}

void Soldier::UpdateAlert(float deltaTime)
{
	if (CanSeePlayer()) {
		TurnTo(m_player->GetPosition(), deltaTime);
		m_reactionTimer += deltaTime;
		if (m_reactionTimer >= REACTION_TIME * 0.5f) {
			ChangeState(enState_Attack);
		}
		return;
	}
	m_reactionTimer = 0.0f;

	if (CanSensePlayer()) {
		m_lastKnownPosition = m_player->GetPosition();
		m_alertTimer = 0.0f;
		m_suspicion += deltaTime;
		if (m_suspicion >= SUSPICION_TIME) {
			ChangeState(enState_Attack);
			return;
		}
		ApproachLookingAround(m_lastKnownPosition, deltaTime);
		return;
	}
	m_suspicion = MathUtil::Clamp(m_suspicion - SUSPICION_DECAY * deltaTime, 0.0f, SUSPICION_TIME);

	if (!MoveTo(m_lastKnownPosition, ALERT_SPEED)) {
		TurnTo(m_lastKnownPosition, deltaTime);
		return;
	}

	m_alertTimer += deltaTime;
	m_yaw += sinf(m_alertTimer * ALERT_LOOK_SPEED * Math::PI) * ALERT_LOOK_ANGLE * deltaTime;
	if (m_alertTimer >= ALERT_LOOK_TIME) {
		ChangeState(enState_Patrol);
	}
}

void Soldier::UpdateAttack(float deltaTime)
{
	if (m_player == nullptr || m_player->IsDead()) {
		ChangeState(enState_Patrol);
		return;
	}

	if (CanSeePlayer() || CanSensePlayer()) {
		m_lastKnownPosition = m_player->GetPosition();
		m_lostTimer = 0.0f;
		TurnTo(m_lastKnownPosition, deltaTime);

		m_shotTimer -= deltaTime;
		float targetYaw = MathUtil::YawFromDirection(m_lastKnownPosition - m_position);
		if (m_shotTimer <= 0.0f && fabsf(AngleDiff(m_yaw, targetYaw)) <= SHOT_FACING_ANGLE) {
			Shoot();
			m_shotTimer = SHOT_INTERVAL;
		}
		return;
	}

	m_lostTimer += deltaTime;
	TurnTo(m_lastKnownPosition, deltaTime);
	if (m_lostTimer >= LOST_TIME) {
		ChangeState(enState_Alert);
	}
}

void Soldier::ChangeState(EnState state)
{
	if (m_state == state) {
		return;
	}
	m_state = state;
	m_reactionTimer = 0.0f;
	if (state == enState_Patrol) {
		m_suspicion = 0.0f;
	}
	m_alertTimer = 0.0f;
	m_lostTimer = 0.0f;
	m_waitTimer = 0.0f;
	if (state == enState_Attack) {
		m_shotTimer = FIRST_SHOT_DELAY;
	}
	if (state != enState_Patrol) {
		m_markTimer = MARK_POP_TIME;
	}
}

Vector3 Soldier::GetEyePosition() const
{
	Vector3 eye = m_position;
	eye.y += EYE_HEIGHT;
	return eye;
}

Vector3 Soldier::GetPlayerAimPosition() const
{
	Vector3 aim = m_player->GetPosition();
	aim.y += m_player->IsDiving() ? DIVING_AIM_HEIGHT : PLAYER_AIM_HEIGHT;
	return aim;
}

bool Soldier::CanSeePlayer() const
{
	if (m_player == nullptr || m_player->IsDead() || m_player->IsDiving()) {
		return false;
	}

	Vector3 eye = GetEyePosition();
	Vector3 toPlayer = GetPlayerAimPosition() - eye;
	float distance = toPlayer.Length();
	if (distance > SIGHT_RANGE || distance < 0.01f) {
		return false;
	}

	if (m_state != enState_Attack) {
		Vector3 flat = toPlayer;
		flat.y = 0.0f;
		flat.Normalize();
		if (flat.Dot(GetForward()) < cosf(Math::DegToRad(SIGHT_HALF_ANGLE))) {
			return false;
		}
	}

	Vector3 direction = toPlayer;
	direction.Normalize();
	Vector3 rayStart = eye + direction * (CAPSULE_RADIUS + SIGHT_RAY_START_MARGIN);
	Vector3 rayEnd = eye + direction * (distance - SIGHT_RAY_MARGIN);
	return !IsSightBlocked(rayStart, rayEnd);
}

bool Soldier::IsSightBlocked(const Vector3& start, const Vector3& end) const
{
	Vector3 direction = end - start;
	direction.Normalize();
	Vector3 rayStart = start;
	for (int i = 0; i < SIGHT_RAY_RETRY_NUM; i++) {
		Vector3 hitPos;
		if (!PhysicsWorld::GetInstance()->RayTest(rayStart, end, hitPos)) {
			return false;
		}
		bool isCharacter = false;
		QueryGOs<EnemyBase>(ObjectName::ENEMY, [&](EnemyBase* enemy) {
			float rate = 0.0f;
			if (enemy->HitTest(hitPos, hitPos, SIGHT_CHARACTER_MARGIN, rate)) {
				isCharacter = true;
				return false;
			}
			return true;
		});
		if (!isCharacter) {
			return true;
		}
		rayStart = hitPos + direction * SIGHT_CHARACTER_SKIP;
		if ((end - rayStart).Dot(direction) <= 0.0f) {
			return false;
		}
	}
	return true;
}

bool Soldier::CanSensePlayer() const
{
	if (m_player == nullptr || m_player->IsDead() || !m_player->IsDiving()) {
		return false;
	}
	Vector3 toPlayer = m_player->GetPosition() - m_position;
	toPlayer.y = 0.0f;
	float distance = toPlayer.Length();
	if (distance > SENSE_RANGE) {
		return false;
	}
	if (m_state == enState_Attack || distance < 0.01f) {
		return true;
	}
	toPlayer.Normalize();
	return toPlayer.Dot(GetForward()) >= cosf(Math::DegToRad(SENSE_HALF_ANGLE));
}

void Soldier::ApproachLookingAround(const Vector3& target, float deltaTime)
{
	Vector3 toTarget = target - m_position;
	toTarget.y = 0.0f;
	m_lookPhase += APPROACH_LOOK_SPEED * deltaTime;
	TurnToYaw(MathUtil::YawFromDirection(toTarget) + sinf(m_lookPhase) * APPROACH_LOOK_ANGLE, deltaTime);
	if (toTarget.Length() > APPROACH_STOP_DISTANCE) {
		Vector3 forward = GetForward();
		m_moveSpeed.x = forward.x * APPROACH_SPEED;
		m_moveSpeed.z = forward.z * APPROACH_SPEED;
	}
}

bool Soldier::MoveTo(const Vector3& target, float speed)
{
	Vector3 toTarget = target - m_position;
	toTarget.y = 0.0f;
	if (toTarget.Length() <= ARRIVE_DISTANCE) {
		return true;
	}
	toTarget.Normalize();
	Vector3 forward = GetForward();
	float facing = forward.Dot(toTarget);
	if (facing < 0.0f) {
		facing = 0.0f;
	}
	m_moveSpeed.x = forward.x * speed * facing;
	m_moveSpeed.z = forward.z * speed * facing;
	return false;
}

void Soldier::TurnTo(const Vector3& target, float deltaTime)
{
	Vector3 toTarget = target - m_position;
	toTarget.y = 0.0f;
	if (toTarget.LengthSq() < 1.0f) {
		return;
	}
	TurnToYaw(MathUtil::YawFromDirection(toTarget), deltaTime);
}

void Soldier::TurnToYaw(float targetYaw, float deltaTime)
{
	float diff = AngleDiff(m_yaw, targetYaw);
	float step = TURN_SPEED * deltaTime;
	if (fabsf(diff) <= step) {
		m_yaw = targetYaw;
	}
	else {
		m_yaw += diff > 0.0f ? step : -step;
	}
}

void Soldier::Shoot()
{
	float yawRad = Math::DegToRad(m_yaw);
	Vector3 forward = { sinf(yawRad), 0.0f, cosf(yawRad) };
	Vector3 right = { cosf(yawRad), 0.0f, -sinf(yawRad) };
	Vector3 muzzle = m_position + forward * MUZZLE_FORWARD + right * MUZZLE_RIGHT;
	muzzle.y += MUZZLE_HEIGHT;

	Vector3 direction = GetPlayerAimPosition() - muzzle;
	direction.Normalize();
	direction.x += RandomRange(SHOT_SPREAD);
	direction.y += RandomRange(SHOT_SPREAD);
	direction.z += RandomRange(SHOT_SPREAD);
	direction.Normalize();

	m_model.PlayAnimation(enAnimationClip_Shoot, SHOOT_BLEND_TIME);
	m_model.SetAnimationSpeed(1.0f);
	m_currentAnimation = enAnimationClip_Shoot;
	m_isShooting = true;

	EnemyBullet* bullet = NewGO<EnemyBullet>(0, ObjectName::ENEMY_BULLET);
	bullet->Init(muzzle, direction, SHOT_DAMAGE);
}

void Soldier::OnDamage()
{
	if (m_player == nullptr) {
		return;
	}
	m_lastKnownPosition = m_player->GetPosition();
	if (m_state == enState_Patrol) {
		ChangeState(enState_Alert);
	}
}

void Soldier::OnDead()
{
	m_isMarkVisible = false;
	m_isShooting = false;
	m_model.SetAnimationSpeed(1.0f);
	m_model.PlayAnimation(enAnimationClip_Death, DEATH_BLEND_TIME);
	m_currentAnimation = enAnimationClip_Death;
}

void Soldier::UpdateMark(float deltaTime)
{
	m_isMarkVisible = false;
	if (m_state == enState_Patrol) {
		return;
	}

	Vector3 worldPos = m_position;
	worldPos.y += MARK_HEIGHT;
	Vector3 toMark = worldPos - g_camera3D->GetPosition();
	if (toMark.Dot(g_camera3D->GetForward()) <= 0.0f) {
		return;
	}

	if (m_markTimer > 0.0f) {
		m_markTimer -= deltaTime;
	}
	float pop = m_markTimer > 0.0f ? m_markTimer / MARK_POP_TIME : 0.0f;
	float scale = MARK_SCALE * (1.0f + pop * 0.6f);

	Vector2 screen;
	g_camera3D->CalcScreenPositionFromWorldPosition(screen, worldPos);
	m_mark.SetText(m_state == enState_Attack ? L"!" : L"?");
	float blend = m_suspicion / SUSPICION_TIME * MARK_SUSPICION_BLEND;
	Vector4 alertColor = {
		Math::Lerp(blend, MARK_ALERT_COLOR.x, MARK_ATTACK_COLOR.x),
		Math::Lerp(blend, MARK_ALERT_COLOR.y, MARK_ATTACK_COLOR.y),
		Math::Lerp(blend, MARK_ALERT_COLOR.z, MARK_ATTACK_COLOR.z),
		1.0f,
	};
	m_mark.SetColor(m_state == enState_Attack ? MARK_ATTACK_COLOR : alertColor);
	m_mark.SetScale(scale);
	m_mark.SetPosition({ screen.x - MARK_HALF_WIDTH * scale, screen.y + 20.0f * scale, 0.0f });
	m_isMarkVisible = true;
}

void Soldier::Render(RenderContext& rc)
{
	EnemyBase::Render(rc);
	if (m_isMarkVisible && !IsDead()) {
		m_mark.Draw(rc);
	}
}

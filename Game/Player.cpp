#include "stdafx.h"
#include "Player.h"

namespace
{
	const float CAPSULE_RADIUS = 30.0f;
	const float CAPSULE_HEIGHT = 110.0f;
	const float EYE_HEIGHT = 160.0f;
	const float WALK_SPEED = 400.0f;
	const float GRAVITY = 980.0f;
	const float YAW_SPEED = 150.0f;
	const float PITCH_SPEED = 100.0f;
	const float PITCH_LIMIT = 80.0f;
	const float STICK_DEADZONE = 0.15f;
}

Player::Player()
{
}

Player::~Player()
{
}

bool Player::Start()
{
	m_charaCon.Init(CAPSULE_RADIUS, CAPSULE_HEIGHT, m_position);
	g_camera3D->SetNear(0.5f);
	g_camera3D->SetFar(20000.0f);
	UpdateCamera();
	return true;
}

void Player::Update()
{
	Rotate();
	Move();
	UpdateCamera();
}

void Player::Rotate()
{
	float deltaTime = g_gameTime->GetFrameDeltaTime();
	float stickX = g_pad[0]->GetRStickXF();
	float stickY = g_pad[0]->GetRStickYF();
	if (fabsf(stickX) < STICK_DEADZONE) {
		stickX = 0.0f;
	}
	if (fabsf(stickY) < STICK_DEADZONE) {
		stickY = 0.0f;
	}

	m_yaw += stickX * YAW_SPEED * deltaTime;
	m_pitch += stickY * PITCH_SPEED * deltaTime;
	if (m_pitch > PITCH_LIMIT) {
		m_pitch = PITCH_LIMIT;
	}
	if (m_pitch < -PITCH_LIMIT) {
		m_pitch = -PITCH_LIMIT;
	}

	float yawRad = Math::DegToRad(m_yaw);
	m_forward.Set(sinf(yawRad), 0.0f, cosf(yawRad));
	m_right.Set(cosf(yawRad), 0.0f, -sinf(yawRad));
}

void Player::Move()
{
	float deltaTime = g_gameTime->GetFrameDeltaTime();
	float stickX = g_pad[0]->GetLStickXF();
	float stickY = g_pad[0]->GetLStickYF();

	Vector3 inputDir = m_forward * stickY + m_right * stickX;
	if (inputDir.LengthSq() > 1.0f) {
		inputDir.Normalize();
	}

	m_moveSpeed.x = inputDir.x * WALK_SPEED;
	m_moveSpeed.z = inputDir.z * WALK_SPEED;

	if (m_charaCon.IsOnGround()) {
		m_moveSpeed.y = 0.0f;
	}
	m_moveSpeed.y -= GRAVITY * deltaTime;

	m_position = m_charaCon.Execute(m_moveSpeed, deltaTime);
}

void Player::UpdateCamera()
{
	float pitchRad = Math::DegToRad(m_pitch);
	Vector3 lookDir = m_forward * cosf(pitchRad) + Vector3::Up * sinf(pitchRad);

	Vector3 eyePos = m_position;
	eyePos.y += EYE_HEIGHT;

	g_camera3D->SetPosition(eyePos);
	g_camera3D->SetTarget(eyePos + lookDir * 100.0f);
	g_camera3D->SetUp(Vector3::Up);
}

void Player::Render(RenderContext& rc)
{
}

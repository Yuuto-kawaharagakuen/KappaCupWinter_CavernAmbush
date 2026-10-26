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

private:
	void UpdateState();
	void Rotate();
	void Move();
	void UpdateCamera();

	CharacterController m_charaCon;
	EnState m_state = enState_Surface;
	Vector3 m_position = Vector3::Zero;
	Vector3 m_moveSpeed = Vector3::Zero;
	Vector3 m_forward = Vector3::AxisZ;
	Vector3 m_right = Vector3::AxisX;
	float m_yaw = 0.0f;
	float m_pitch = 0.0f;
	float m_cameraBlend = 0.0f;
};

#pragma once

class Player;

class Grenade : public IGameObject
{
public:
	Grenade();
	~Grenade();
	bool Start() override;
	void Update() override;
	void Render(RenderContext& rc) override;

	void Init(const Vector3& position, const Vector3& velocity, int damage);

private:
	void Move(float deltaTime);
	void Explode();

	ModelRender m_model;
	Player* m_player = nullptr;
	Vector3 m_position = Vector3::Zero;
	Vector3 m_velocity = Vector3::Zero;
	Quaternion m_rotation = Quaternion::Identity;
	Vector3 m_spinAxis = Vector3::AxisX;
	float m_timer = 0.0f;
	int m_damage = 0;
	bool m_isLanded = false;
};

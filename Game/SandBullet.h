#pragma once

class SandBullet : public IGameObject
{
public:
	SandBullet();
	~SandBullet();
	bool Start() override;
	void Update() override;
	void Render(RenderContext& rc) override;

	void Init(const Vector3& position, const Vector3& direction);
	void OnHit();

	const Vector3& GetPosition() const
	{
		return m_position;
	}

private:
	ModelRender m_model;
	CollisionObject* m_collision = nullptr;
	Vector3 m_position = Vector3::Zero;
	Vector3 m_direction = Vector3::AxisZ;
	float m_traveled = 0.0f;
	bool m_isDead = false;
};

#pragma once

class Player;

class EnemyBullet : public IGameObject
{
public:
	EnemyBullet();
	~EnemyBullet();
	bool Start() override;
	void Update() override;
	void Render(RenderContext& rc) override;

	void Init(const Vector3& position, const Vector3& direction, int damage);

private:
	ModelRender m_model;
	Player* m_player = nullptr;
	Vector3 m_position = Vector3::Zero;
	Vector3 m_direction = Vector3::AxisZ;
	float m_traveled = 0.0f;
	int m_damage = 0;
	bool m_isDead = false;
};

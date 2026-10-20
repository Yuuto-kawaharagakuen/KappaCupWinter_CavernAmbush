#pragma once

class BackGround : public IGameObject
{
public:
	BackGround();
	~BackGround();
	bool Start() override;

private:
	struct StaticBox
	{
		BoxCollider collider;
		RigidBody rigidBody;
	};

	void AddBox(const Vector3& position, const Vector3& size);

	std::vector<std::unique_ptr<StaticBox>> m_boxes;
};

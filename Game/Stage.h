#pragma once

class Stage : public IGameObject
{
public:
	Stage();
	~Stage();
	void Load(const char* levelFilePath);
	void Update() override;
	void Render(RenderContext& rc) override;

	const Vector3& GetPlayerStartPosition() const
	{
		return m_playerStartPosition;
	}
	const Vector3& GetExitPosition() const
	{
		return m_exitPosition;
	}

private:
	struct Lantern
	{
		ModelRender frame;
		ModelRender glass;
		ModelRender rope;
	};

	struct Sandbag
	{
		ModelRender model;
		std::vector<std::unique_ptr<BoxCollider>> colliders;
		std::vector<std::unique_ptr<RigidBody>> rigidBodies;
	};

	void AddLantern(const Vector3& position);
	void AddSandbag(const LevelObjectData& objData);
	void InitBarrier(const Vector3& position);
	void InitSky();
	void InitOutdoorModel(const LevelObjectData& objData);
	void AddPointLight(const LevelObjectData& objData);
	void InitLighting();
	void ResetLighting();

	LevelRender m_levelRender;
	ModelRender m_skyModel;
	ModelRender m_outdoorModel;
	Texture m_skyTexture;
	float m_skyLuminance = 1.0f;
	bool m_hasOutdoorModel = false;
	float m_outdoorStartX = 0.0f;
	std::vector<std::unique_ptr<PointLight>> m_pointLights;
	std::vector<std::unique_ptr<Lantern>> m_lanterns;
	std::vector<std::unique_ptr<Sandbag>> m_sandbags;
	std::vector<Vector3> m_lampPositions;
	float m_lanternEmission = 1.0f;
	BoxCollider m_barrierCollider;
	RigidBody m_barrierRigidBody;
	Vector3 m_playerStartPosition = Vector3::Zero;
	Vector3 m_exitPosition = Vector3::Zero;
};

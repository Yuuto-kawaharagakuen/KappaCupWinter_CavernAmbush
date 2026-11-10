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
	const Vector3& GetHolePosition() const
	{
		return m_holePosition;
	}

private:
	struct Lantern
	{
		ModelRender frame;
		ModelRender glass;
		ModelRender rope;
	};

	void AddLantern(const Vector3& position);
	void InitBarrier(const Vector3& position);
	void InitSky();
	void InitOutdoorModel(const LevelObjectData& objData);
	void AddPointLight(const LevelObjectData& objData);
	void InitLighting();
	void ResetLighting();

	LevelRender m_levelRender;
	ModelRender m_holeModel;
	ModelRender m_skyModel;
	ModelRender m_outdoorModel;
	Texture m_skyTexture;
	float m_skyLuminance = 1.0f;
	bool m_hasOutdoorModel = false;
	float m_outdoorStartX = 0.0f;
	std::vector<std::unique_ptr<PointLight>> m_pointLights;
	std::vector<std::unique_ptr<Lantern>> m_lanterns;
	std::vector<Vector3> m_lampPositions;
	float m_lanternEmission = 1.0f;
	BoxCollider m_barrierCollider;
	RigidBody m_barrierRigidBody;
	Vector3 m_playerStartPosition = Vector3::Zero;
	Vector3 m_holePosition = Vector3::Zero;
};

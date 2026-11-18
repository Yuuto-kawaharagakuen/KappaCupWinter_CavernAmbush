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
	bool IsPlayerStartDiving() const
	{
		return m_isPlayerStartDiving;
	}
	const Vector3& GetExitPosition() const
	{
		return m_exitPosition;
	}
	bool HasExit() const
	{
		return m_hasExit;
	}
	const Vector3& GetBossDoorPosition() const
	{
		return m_bossDoorPosition;
	}
	bool HasBossDoor() const
	{
		return m_hasBossDoor;
	}
	const Vector3& GetBossStartPosition() const
	{
		return m_bossStartPosition;
	}
	bool HasBossStart() const
	{
		return m_hasBossStart;
	}
	const Vector3& GetBackPosition() const
	{
		return m_backPosition;
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
	bool m_isPlayerStartDiving = false;
	Vector3 m_exitPosition = Vector3::Zero;
	bool m_hasExit = false;
	Vector3 m_bossDoorPosition = Vector3::Zero;
	bool m_hasBossDoor = false;
	Vector3 m_bossStartPosition = Vector3::Zero;
	bool m_hasBossStart = false;
	Vector3 m_backPosition = Vector3::Zero;
};

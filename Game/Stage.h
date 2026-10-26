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
	LevelRender m_levelRender;
	Vector3 m_playerStartPosition = Vector3::Zero;
	Vector3 m_holePosition = Vector3::Zero;
};

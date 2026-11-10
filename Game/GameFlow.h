#pragma once

class GameFlow : public IGameObject
{
public:
	static GameFlow* Get();

	void StartNewGame();
	void Retry();
	bool ClearCurrentStage(float stageTime);

	const char* GetStageLevelPath() const;
	int GetCurrentStage() const
	{
		return m_currentStage;
	}
	int GetRetryStage() const
	{
		return m_retryStage;
	}
	bool HasKey() const
	{
		return m_hasKey;
	}
	void SetHasKey(bool hasKey)
	{
		m_hasKey = hasKey;
	}
	float GetTotalTime() const
	{
		return m_totalTime;
	}
	int GetRetryCount() const
	{
		return m_retryCount;
	}

private:
	int m_currentStage = 0;
	int m_retryStage = 0;
	bool m_hasKey = false;
	float m_totalTime = 0.0f;
	int m_retryCount = 0;
};

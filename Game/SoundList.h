#pragma once

enum class SoundId
{
	PlayerShot,
	PlayerReload,
	PlayerGunRaise,
	PlayerEmpty,
	PlayerDive,
	PlayerSurface,
	PlayerDiveMove,
	PlayerDamage,
	PlayerReloadFull,
	PlayerScope,
	EnemyShot,
	EnemyHit,
	EnemyDown,
	BulletRicochet,
	EnemyAlert,
	MenuCursor,
	MenuDecide,
	SoldierStep,
	Num,
};

namespace SoundList
{
	constexpr float MENU_VOLUME = 0.8f;

	inline void Init()
	{
		static bool isInitialized = false;
		if (isInitialized) {
			return;
		}
		isInitialized = true;
		const char* const names[] = {
			"PlayerShot",
			"PlayerReload",
			"PlayerGunRaise",
			"PlayerEmpty",
			"PlayerDive",
			"PlayerSurface",
			"PlayerDiveMove",
			"PlayerDamage",
			"PlayerReloadFull",
			"PlayerScope",
			"EnemyShot",
			"EnemyHit",
			"EnemyDown",
			"BulletRicochet",
			"EnemyAlert",
			"MenuCursor",
			"MenuDecide",
			"SoldierStep",
		};
		static_assert(sizeof(names) / sizeof(names[0]) == static_cast<int>(SoundId::Num), "SoundId and names mismatch");
		for (int i = 0; i < static_cast<int>(SoundId::Num); i++) {
			char path[128];
			sprintf_s(path, "Assets/sound/%s.wav", names[i]);
			g_soundEngine->ResistWaveFileBank(i, path);
		}
	}

	inline SoundSource* Create(SoundId id, float volume, float speed)
	{
		Init();
		if (!g_soundEngine->GetWaveFileBank().FindWaveFile(static_cast<int>(id))) {
			return nullptr;
		}
		SoundSource* sound = NewGO<SoundSource>(0);
		sound->Init(static_cast<int>(id));
		sound->SetVolume(volume);
		sound->SetFrequencyRatio(speed);
		return sound;
	}

	inline void Play(SoundId id, float volume, float speed = 1.0f)
	{
		SoundSource* sound = Create(id, volume, speed);
		if (sound != nullptr) {
			sound->Play(false);
		}
	}

	inline void PlayAt(SoundId id, float volume, const Vector3& position, float range)
	{
		float distance = (position - g_camera3D->GetPosition()).Length();
		float rate = 1.0f - distance / range;
		if (rate <= 0.0f) {
			return;
		}
		Play(id, volume * rate * rate);
	}

	inline SoundSource* PlayLoop(SoundId id, float volume)
	{
		SoundSource* sound = Create(id, volume, 1.0f);
		if (sound != nullptr) {
			sound->Play(true);
		}
		return sound;
	}

	inline void StopLoop(SoundSource*& sound)
	{
		if (sound == nullptr) {
			return;
		}
		sound->Stop();
		DeleteGO(sound);
		sound = nullptr;
	}
}

#pragma once
using namespace FMOD;

enum class EAudioBus
{
	MASTER = 0,
	BGM,
	SFX,
	AMBIENT,

	COUNT,
};
constexpr size_t AUDIO_BUS_COUNT = static_cast<size_t>(EAudioBus::COUNT);

using SoundID = uint64_t;

struct AudioListenerState
{
	XMFLOAT3 pos{};
	XMFLOAT3 vel{};
	XMFLOAT3 forward{ 0.f, 0.f, 1.f };
	XMFLOAT3 up{ 0.f, 1.f, 0.f };
};

struct AudioPlayDesc
{
	bool bLoop = false;
	bool b3D = false;
	float volumn = 1.f;
	float minDistance = 1.f;
	float maxDistance = 32.f;
	EAudioBus bus = EAudioBus::SFX;
};

struct AudioLoadDesc
{
	bool bLoop = false;
	bool b3D = false;
	bool bStream = false;
};

struct AudioRequest
{
	bool b3D = false;
	SoundID id = 0;
	XMFLOAT3 pos{};
	float volume = 1.f;
	float minDistance = 1.f;
	float maxDistance = 24.f;
	EAudioBus bus = EAudioBus::SFX;
};

class CAudioSystem
{
public:
	CAudioSystem();
	~CAudioSystem();

	bool Initialize();
	void Shutdown();
	void Tick();

	bool LoadSound(SoundID id, const char* path, const AudioLoadDesc& desc);
	void UnloadAllSounds();

	FMOD::Channel* Play2D(SoundID id, float volume = 1.f, EAudioBus bus = EAudioBus::SFX);
	FMOD::Channel* Play3D(SoundID id, const XMFLOAT3& worldPos, float volume = 1.f
		, float minDistance = 1.f, float maxDistance = 32.f, EAudioBus bus = EAudioBus::SFX);

	void SetListener(const AudioListenerState& state);

	void SetBusVolume(EAudioBus bus, float volume);
	float GetBusVolume(EAudioBus bus) const;

public:
	inline void SetMasterVolume(float volume) { m_fMasterVolume = volume; }
	inline float GetMasterVolume() const { return m_fMasterVolume; }
	

private:
	FMOD::ChannelGroup* _GetBusGroup(EAudioBus bus);
	bool _CreateChannelGroups();
	FMOD_MODE _MakeModelFlags(const AudioLoadDesc& desc) const;
	FMOD_VECTOR _ToFMOD(const XMFLOAT3& v);

private:
	FMOD::System* m_pSystem = nullptr;
	
	unordered_map<SoundID, FMOD::Sound*> m_mapSounds;

	array<FMOD::ChannelGroup*, AUDIO_BUS_COUNT> m_busGroups{};
	array<float, AUDIO_BUS_COUNT> m_busVolumes{};

	float m_fMasterVolume = 1.0f;

	queue<AudioRequest> m_pendingRequests;
};
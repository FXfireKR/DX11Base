#pragma once

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

public:
	void Submit2D(SoundID soundID, float volume = 1.f, EAudioBus bus = EAudioBus::SFX);
	void Submit3D(SoundID soundID, const XMFLOAT3& pos, float volume = 1.f
		, float minDistance = 1.f, float maxDistance = 24.f, EAudioBus bus = EAudioBus::SFX);

public:
	bool LoadSound(SoundID id, const char* path, bool b3D, bool bLoop = false);
	void SetListener(const AudioListenerState& state);

	void SetVolume(EAudioBus bus, float volume);
	float GetVolume(EAudioBus bus) const;

public:
	inline void SetDispatchPerFrame(size_t uPerFrame) { m_uDispatchPerFrame = uPerFrame; }
	inline size_t GetDispatchPerFrame() const { return m_uDispatchPerFrame; }
	
private:
	void _Dispatch();

	FMOD::Channel* _Play(const AudioRequest& request);

	FMOD::ChannelGroup* _GetBusGroup(EAudioBus bus);
	bool _CreateChannelGroups();
	FMOD_MODE _MakeModeFlags(const AudioLoadDesc& desc) const;
	FMOD_VECTOR _ToFMOD(const XMFLOAT3& v);

private:
	FMOD::System* m_pSystem = nullptr;
	
	unordered_map<SoundID, FMOD::Sound*> m_mapSounds;

	array<FMOD::ChannelGroup*, AUDIO_BUS_COUNT> m_busGroups{};
	array<float, AUDIO_BUS_COUNT> m_busVolumes{};

	queue<AudioRequest> m_pendingRequests;
	size_t m_uDispatchPerFrame = 16;
};
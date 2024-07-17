#pragma once
#include <map>
#include <vector>

#include <glm/glm.hpp>
#include <fmod/fmod.hpp>

void CheckError(FMOD_RESULT result, const char* file, int line);
#define FMODCheckError(result) CheckError(result, __FILE__, __LINE__)

struct sAudio
{
	const char* name;
	FMOD::Sound* Audio;
};

struct sChannel
{
	const char* name;
	FMOD::Channel* fmodChannel;
	float volume = 1.0f;
	float pitch = 0.0f;
	float pan = 0.0f;
	bool isPaused = true;

	int DSPindex = 0;
	int ReverbDSPindex = 0;
	int HighPassDSPindex = 0;
	int LowPassDSPindex = 0;
	int DistortionDSPindex = 0;
	int ChorusPassDSPindex = 0;
};

struct sDriverInfo
{
	int id;
	char name[256];
	FMOD_GUID guid;
	int systemrate;
	FMOD_SPEAKERMODE speakermode;
	int speakermodechannels;
};

#define CHANNELGROUP_MASTER_INDEX	0
#define CHANNELGROUP_MUSIC_INDEX	1
#define CHANNELGROUP_SFX_INDEX		2

class cAudioManager
{
public:
	cAudioManager();
	~cAudioManager();

	void Initialize();
	void Update();
	void Destroy();

	std::vector<sDriverInfo> GetDeviceDrivers();
	void SetDeviceDriver(int id);

	void LoadAudioStream(const char* source);
	void LoadAudio(const char* file);
	int PlayAudio(const char* AudioName, int ChannelGroupIndex, glm::vec3 soundPos);

	// Channel Controls
	void SetChannelVolume(int channelId, float value);
	void SetChannelPitch(int channelId, float value);
	void SetChannelPan(int channelId, float value);

	void SetPaused(int channelId, bool pause);
	bool GetPaused(int channelID);

	void SetChannelGroupVolume(int channelId, float value);
	void SetChannelGroupPitch(int channelId, float value);
	void SetChannelGroupPan(int channelId, float value);

	bool GetChannelPlaying(int channelId);
	void GetPlaybackPosition(int channelId, unsigned int& value);

	// DSP Effects
	void AddReverbFilterOnChannel(int channelId);
	void AddLowPassFilterOnChannel(int channelId);
	void AddHighPassFilterOnChannel(int channelId);
	void AddDistortionFilterOnChannel(int channelId);
	void AddChorusPassOnChannel(int channelId);

	/** MODIFYING REVERB DSP EFFECT *
	Decay [100, 20000] // Density [10, 100] // Diffusion [10, 100]*/
	void SetReverbValuesOnChannel(int channelId, float decay = 1500.0f, float density = 50.0f, float diffusion = 50.0f);
	/** MODIFYING LOW PASS FILTER DSP EFFECT *
	Cutoff [1, 22000]*/
	void SetLowPassFilterValuesOnChannel(int channelId, float cutoff = 5000.0f);
	/** MODIFYING HIGH PASS FILTER DSP EFFECT *
	Cutoff [1, 22000]*/
	void SetHighPassFilterValuesOnChannel(int channelId, float cutoff = 5000.0f);
	/** MODIFYING DISTORTION PASS FILTER DSP EFFECT * 
	Level [0, 1]*/
	void SetDistortionLevelFilterValuesOnChannel(int channelId, float level = 0.5f);
	/** MODIFYING CHROUS PASS FILTER DSP EFFECT *
	Mix [0, 100] // Rate [0, 20] // Depth [0, 100]*/
	void SetChorusPassValuesOnChannel(int channelId, float mix = 50.0f, float rate = 0.8f, float depth = 3.0f);

	// 3D Stuff
	void Load3DAudio(const char* file);
	void Get3DAttributes(int channelId, glm::vec3& audioPos, glm::vec3& audioVel);
	void Set3DAttributes(int channelId, glm::vec3& audioPos, glm::vec3& audioVel);
	void SetListenerAttributes(const glm::vec3& position, const glm::vec3& velocity,
		const glm::vec3& forward, const glm::vec3& up);

	void CreateNewGeometry(int maxPoly, int maxVert);
	int AddPolygon(float direct, float reverb, bool doublesided,
		const std::vector<glm::vec3>& vertices);

	void SetGeometryScale(const glm::vec3& scale);
	void SetGeometryPosition(const glm::vec3& position);
	void SetGeometryRotation(const glm::vec3& forward, const glm::vec3& up);

	void GetListenerAttributes(glm::vec3& position, glm::vec3& velocity,
		glm::vec3& forward, glm::vec3& up);

	unsigned int GetData(const char* sound, void* data);

private:

	inline FMOD::ChannelGroup* GetMasterChannelGroup()
	{
		return m_ChannelGroupList[CHANNELGROUP_MASTER_INDEX];
	}
	inline FMOD::ChannelGroup* GetSFXChannelGroup()
	{
		return m_ChannelGroupList[CHANNELGROUP_SFX_INDEX];
	}
	inline FMOD::ChannelGroup* GetMusicChannelGroup()
	{
		return m_ChannelGroupList[CHANNELGROUP_MUSIC_INDEX];
	}

	bool m_Initialized = false;

	std::map<const char*, sAudio*> m_AudioMap;
	std::vector<sChannel*> m_ChannelList;
	sAudio* m_CurrentAudio;

	int m_NextChannelId = 0;
	int m_PolygonIndex = 0;

	FMOD::System* m_System = nullptr;

	// 3D
	std::vector<FMOD::Geometry*> vpGeometry;

	// DSP
	FMOD::DSP* m_ReverbDSP;
	FMOD::DSP* m_HighPassDSP;
	FMOD::DSP* m_LowPassDSP;
	FMOD::DSP* m_DistortionDSP;
	FMOD::DSP* m_ChorusPassDSP;

	// Channel Group
	std::vector<FMOD::ChannelGroup*> m_ChannelGroupList;
};


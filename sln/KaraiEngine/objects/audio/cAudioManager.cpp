#include "cAudioManager.h"
#include "../../global/global.h"

void GLMToFMOD(const glm::vec3& in, FMOD_VECTOR& out)
{
	out.x = in.x;
	out.y = in.y;
	out.z = in.z;
}

void FMODToGLM(const FMOD_VECTOR& in, glm::vec3& out)
{
	out.x = in.x;
	out.y = in.y;
	out.z = in.z;
}

const int MAX_CHANNELS = 20;

void CheckError(FMOD_RESULT result, const char* file, int line)
{
	if (result != FMOD_OK)
	{
		g_pConsole->cError("FMOD Error " + std::to_string(result) + ": " + file + " at " + std::to_string(line));
	}
}

cAudioManager::cAudioManager(){}

cAudioManager::~cAudioManager(){}

void cAudioManager::Initialize()
{
	if (m_Initialized)
		return;

	FMOD_RESULT result;
	result = FMOD::System_Create(&m_System);
	if (result != FMOD_OK)
	{
		g_pConsole->cError("Failed to create the FMOD System!");
		return;
	}

	result = m_System->init(MAX_CHANNELS, FMOD_INIT_NORMAL | FMOD_INIT_PROFILE_ENABLE, nullptr);
	if (result != FMOD_OK)
	{
		g_pConsole->cError("Failed to initialize the system!");
		// Cleanup
		result = m_System->close();
		if (result != FMOD_OK)
		{
			g_pConsole->cError("Failed to close system!");
		}
		return;
	}

	g_pConsole->cSuccess("AudioManager::Initialize(): Successful!");

	for (int i = 0; i < MAX_CHANNELS; i++)
	{
		m_ChannelList.push_back(new sChannel);
	}

	result = m_System->set3DSettings(0.5f, 0.5f, 0.1f);
	result = m_System->setGeometrySettings(1000.0f);
	FMODCheckError(result);

	m_Initialized = true;
}

void cAudioManager::Destroy()
{
	if (!m_Initialized)
		return;

	FMOD_RESULT result;

	// Move this to a function, clear and release Audios from Audio map
	for (std::pair<const char*, sAudio*> pair : m_AudioMap)
	{
		result = pair.second->Audio->release();
		FMODCheckError(result);
	}

	m_AudioMap.clear();

	result = m_System->close();
	FMODCheckError(result);

	result = m_System->release();
	FMODCheckError(result);

	m_Initialized = false;
}

std::vector<sDriverInfo> cAudioManager::GetDeviceDrivers()
{
	int numDrivers;

	// Retrieve the number of audio drivers available on your PC
	m_System->getNumDrivers(&numDrivers);

	std::vector<sDriverInfo> drivers;
	for (int i = 0; i < numDrivers; i++) {
		sDriverInfo driverInfo;
		driverInfo.id = i;

		FMOD_RESULT result = m_System->getDriverInfo(
			driverInfo.id,
			driverInfo.name,
			256,
			&driverInfo.guid,
			&driverInfo.systemrate,
			&driverInfo.speakermode,
			&driverInfo.speakermodechannels);

		// Print to console
		printf("Device %d: \n name: %s\n guid: %d %d %d\n systemrate: \n speakermode: %d\n channels: %d\n",
			driverInfo.id,
			driverInfo.name,
			driverInfo.guid.Data1,
			driverInfo.guid.Data2,
			driverInfo.guid.Data3,
			driverInfo.systemrate,
			(int)driverInfo.speakermode,
			driverInfo.speakermodechannels);
		
		drivers.push_back(driverInfo);
	}
	return drivers;
}

void cAudioManager::SetDeviceDriver(int id)
{
	m_System->setDriver(id);
}

void cAudioManager::LoadAudioStream(const char* source)
{
	if (!m_Initialized)
		return;

	if (m_AudioMap.find(source) != m_AudioMap.end())
	{
		g_pConsole->cError("Audio already loaded!");
		return;
	}

	m_AudioMap.insert(std::pair<const char*, sAudio*>(source, new sAudio()));

	// Create a stream for the audio.
	FMOD_RESULT result = m_System->createStream(source, FMOD_DEFAULT, 0, &m_AudioMap[source]->Audio);

	std::string string = source;
	if (result != FMOD_OK)
	{
		g_pConsole->cError("Failed to load the Audio stream: " + string);
		return;
	}

	g_pConsole->cSuccess(string + " loaded successful!");
}

// This function loads a media file directly into memory
void cAudioManager::Load3DAudio(const char* file)
{
	if (!m_Initialized)
		return;

	if (m_AudioMap.find(file) != m_AudioMap.end())
	{
		g_pConsole->cMessage("Audio already loaded!");
		return;
	}

	m_AudioMap.insert(std::pair<const char*, sAudio*>(file, new sAudio()));

	FMOD_RESULT result;

	// This call loads our audio file entirely into memory
	result = m_System->createSound(file, FMOD_3D | FMOD_LOOP_NORMAL, nullptr, &m_AudioMap[file]->Audio);
	FMODCheckError(result);

	std::string strFile = file;
	if (result != FMOD_OK)
	{

		g_pConsole->cError("Failed to load the Audio file: " + strFile);
		return;
	}

	m_AudioMap[file]->Audio->set3DMinMaxDistance(0.5f, 3000.f);

	g_pConsole->cSuccess("Audio: " + strFile + " loaded successful!");
}

void cAudioManager::LoadAudio(const char* file)
{
	if (!m_Initialized)
		return;

	if (m_AudioMap.find(file) != m_AudioMap.end())
	{
		g_pConsole->cMessage("Audio already loaded!");
		return;
	}

	m_AudioMap.insert(std::pair<const char*, sAudio*>(file, new sAudio()));

	FMOD_RESULT result;

	result = m_System->createSound(file, FMOD_LOOP_NORMAL, 0, &m_AudioMap[file]->Audio);

	std::string strFile = file;
	if (result != FMOD_OK)
	{
		g_pConsole->cError("Failed to load the Audio file: " + strFile);
		return;
	}

	g_pConsole->cSuccess("Audio: " + strFile + " loaded successful!");
}

int cAudioManager::PlayAudio(const char* AudioName, int ChannelGroupIndex, glm::vec3 audioPos)
{
	if (!m_Initialized)
	{
		printf("Not initialized!\n");
		return m_NextChannelId;
	}

	std::map<const char*, sAudio*>::iterator it = m_AudioMap.find(AudioName);

	if (it == m_AudioMap.end())
	{
		printf("Audio not found!\n");
		return m_NextChannelId;
	}
	 
	int channelId = m_NextChannelId;
	m_NextChannelId = (m_NextChannelId + 1) % MAX_CHANNELS;
	sChannel* channel = m_ChannelList[channelId];

	FMOD_RESULT result = m_System->playSound(
		it->second->Audio,						// the sound to play
		nullptr,								// channelGroup
		true,									// paused
		&channel->fmodChannel);					// Channel returned

	FMODCheckError(result);

	m_CurrentAudio = it->second;

	FMOD_VECTOR soundPos;
	GLMToFMOD(audioPos, soundPos);

	// For 3D Audio
	//FMOD_VECTOR soundVel = { 0.f, 0.f, 0.f };
	//result = channel->fmodChannel->set3DAttributes(&soundPos, &soundVel);
	//FMODCheckError(result);

	result = channel->fmodChannel->setPaused(true);
	FMODCheckError(result);

	return channelId;
}

void cAudioManager::Update()
{
	if (!m_Initialized)
		return;

	FMOD_RESULT result;
	result = m_System->update();
	if (result != FMOD_OK)
	{
		FMODCheckError(result);
		Destroy();
	}
}

/**										**/
/**		ADD DSP EFFECTS TO CHANNEL		**/
/**										**/

/** REVERB DSP EFFECT **/
void cAudioManager::AddReverbFilterOnChannel(int channelId)
{
	FMOD_RESULT result = m_System->createDSPByType(FMOD_DSP_TYPE_SFXREVERB, &m_ReverbDSP);
	FMODCheckError(result);

	result = m_ReverbDSP->setParameterFloat(FMOD_DSP_SFXREVERB_DECAYTIME, 1500);
	result = m_ReverbDSP->setParameterFloat(FMOD_DSP_SFXREVERB_DENSITY, 50);
	result = m_ReverbDSP->setParameterFloat(FMOD_DSP_SFXREVERB_DIFFUSION, 50);
	FMODCheckError(result);

	sChannel* channel = m_ChannelList[channelId];

	result = channel->fmodChannel->addDSP(channel->DSPindex, m_ReverbDSP);
	FMODCheckError(result);

	channel->ReverbDSPindex = channel->DSPindex;
	channel->DSPindex++;
	
	return;
}

/** LOW PASS FILTER DSP EFFECT **/
void cAudioManager::AddLowPassFilterOnChannel(int channelId)
{
	FMOD_RESULT result = m_System->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &m_LowPassDSP);
	FMODCheckError(result);

	result = m_LowPassDSP->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, 5000);
	FMODCheckError(result);

	sChannel* channel = m_ChannelList[channelId];

	result = channel->fmodChannel->addDSP(channel->DSPindex, m_LowPassDSP);
	FMODCheckError(result);

	channel->LowPassDSPindex = channel->DSPindex;
	channel->DSPindex++;

	return;
}

/** HIGH PASS FILTER DSP EFFECT **/
void cAudioManager::AddHighPassFilterOnChannel(int channelId)
{
	FMOD_RESULT result = m_System->createDSPByType(FMOD_DSP_TYPE_HIGHPASS, &m_HighPassDSP);
	FMODCheckError(result);

	result = m_HighPassDSP->setParameterFloat(FMOD_DSP_HIGHPASS_CUTOFF, 500);
	FMODCheckError(result);

	sChannel* channel = m_ChannelList[channelId];

	result = channel->fmodChannel->addDSP(channel->DSPindex, m_HighPassDSP);
	FMODCheckError(result);

	channel->HighPassDSPindex = channel->DSPindex;
	channel->DSPindex++;

	return;
}

/** DISTORTION PASS FILTER DSP EFFECT **/
void cAudioManager::AddDistortionFilterOnChannel(int channelId)
{
	FMOD_RESULT result = m_System->createDSPByType(FMOD_DSP_TYPE_DISTORTION, &m_DistortionDSP);
	FMODCheckError(result);

	result = m_DistortionDSP->setParameterFloat(FMOD_DSP_DISTORTION_LEVEL, 0.7f);
	FMODCheckError(result);

	sChannel* channel = m_ChannelList[channelId];

	result = channel->fmodChannel->addDSP(channel->DSPindex, m_DistortionDSP);
	FMODCheckError(result);

	channel->DistortionDSPindex = channel->DSPindex;
	channel->DSPindex++;

	return;
}

/** CHROSU PASS FILTER DSP EFFECT **/
void cAudioManager::AddChorusPassOnChannel(int channelId)
{
	FMOD_RESULT result = m_System->createDSPByType(FMOD_DSP_TYPE_CHORUS, &m_ChorusPassDSP);
	FMODCheckError(result);

	result = m_ChorusPassDSP->setParameterFloat(FMOD_DSP_CHORUS_MIX, 50.f);
	result = m_ChorusPassDSP->setParameterFloat(FMOD_DSP_CHORUS_RATE, 0.8f);
	result = m_ChorusPassDSP->setParameterFloat(FMOD_DSP_CHORUS_DEPTH, 3.f);
	FMODCheckError(result);

	sChannel* channel = m_ChannelList[channelId];

	result = channel->fmodChannel->addDSP(channel->DSPindex, m_ChorusPassDSP);
	FMODCheckError(result);

	channel->ChorusPassDSPindex = channel->DSPindex;
	channel->DSPindex++;

	return;
}

/**										**/
/**		MODIFY DSP EFFECTS ON CHANNEL	**/
/**										**/

/** MODIFYING REVERB DSP EFFECT **/
void cAudioManager::SetReverbValuesOnChannel(int channelId, float decay, float density, float diffusion)
{
	sChannel* channel = m_ChannelList[channelId];

	FMOD_RESULT result = channel->fmodChannel->getDSP(channel->ReverbDSPindex, &m_ReverbDSP);
	FMODCheckError(result);

	result = m_ReverbDSP->setParameterFloat(FMOD_DSP_SFXREVERB_DECAYTIME, decay);
	result = m_ReverbDSP->setParameterFloat(FMOD_DSP_SFXREVERB_DENSITY, density);
	result = m_ReverbDSP->setParameterFloat(FMOD_DSP_SFXREVERB_DIFFUSION, diffusion);
	FMODCheckError(result);

	return;
}

/** MODIFYING LOW PASS FILTER DSP EFFECT **/
void cAudioManager::SetLowPassFilterValuesOnChannel(int channelId, float cutoff)
{
	sChannel* channel = m_ChannelList[channelId];

	FMOD_RESULT result = channel->fmodChannel->getDSP(channel->LowPassDSPindex, &m_LowPassDSP);
	FMODCheckError(result);

	result = m_LowPassDSP->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, cutoff);
	FMODCheckError(result);

	return;
}

/** MODIFYING HIGH PASS FILTER DSP EFFECT **/
void cAudioManager::SetHighPassFilterValuesOnChannel(int channelId, float cutoff)
{
	sChannel* channel = m_ChannelList[channelId];

	FMOD_RESULT result = channel->fmodChannel->getDSP(channel->HighPassDSPindex, &m_HighPassDSP);
	FMODCheckError(result);

	result = m_HighPassDSP->setParameterFloat(FMOD_DSP_HIGHPASS_CUTOFF, cutoff);
	FMODCheckError(result);

	return;
}

/** MODIFYING DISTORTION PASS FILTER DSP EFFECT **/
void cAudioManager::SetDistortionLevelFilterValuesOnChannel(int channelId, float level)
{
	sChannel* channel = m_ChannelList[channelId];

	FMOD_RESULT result = channel->fmodChannel->getDSP(channel->DistortionDSPindex, &m_DistortionDSP);
	FMODCheckError(result);

	result = m_DistortionDSP->setParameterFloat(FMOD_DSP_DISTORTION_LEVEL, level);
	FMODCheckError(result);

	return;
}

/** MODIFYING CHROUS PASS FILTER DSP EFFECT **/
void cAudioManager::SetChorusPassValuesOnChannel(int channelId, float mix, float rate, float depth)
{
	sChannel* channel = m_ChannelList[channelId];

	FMOD_RESULT result = channel->fmodChannel->getDSP(channel->ChorusPassDSPindex, &m_ChorusPassDSP);
	FMODCheckError(result);

	result = m_ChorusPassDSP->setParameterFloat(FMOD_DSP_CHORUS_MIX, mix);
	result = m_ChorusPassDSP->setParameterFloat(FMOD_DSP_CHORUS_RATE, rate);
	result = m_ChorusPassDSP->setParameterFloat(FMOD_DSP_CHORUS_DEPTH, depth);
	FMODCheckError(result);

	return;
}

//	SET CHANNEL API CALLS
void cAudioManager::SetChannelVolume(int channelId, float value)
{
	m_ChannelList[channelId]->volume = value;
	FMOD_RESULT result = m_ChannelList[channelId]->fmodChannel->setVolume(value);
	FMODCheckError(result);	
	return;
}

void cAudioManager::SetChannelPitch(int channelId, float value)
{
	m_ChannelList[channelId]->pitch = value;
	FMOD_RESULT result = m_ChannelList[channelId]->fmodChannel->setPitch(value);
	FMODCheckError(result);
	return;
}

void cAudioManager::SetChannelPan(int channelId, float value)
{
	m_ChannelList[channelId]->pan = value;
	FMOD_RESULT result = m_ChannelList[channelId]->fmodChannel->setPan(value);
	FMODCheckError(result);
	return;
}

void cAudioManager::SetPaused(int channelId, bool pause)
{
	if (m_ChannelList[channelId]->isPaused == pause)
	{
		return;
	}

	m_ChannelList[channelId]->isPaused = pause;
	FMOD_RESULT result = m_ChannelList[channelId]->fmodChannel->setPaused(pause);
	FMODCheckError(result);
	return;
}

bool cAudioManager::GetPaused(int channelID)
{
	return m_ChannelList[channelID]->isPaused;
}

// SET CHANNEL GROUP API CALLS
void cAudioManager::SetChannelGroupVolume(int channelId, float value)
{
	printf("Set ChannelGroup Volume %d to %.2f\n", channelId, value);
	FMOD_RESULT result = m_ChannelGroupList[channelId]->setVolume(value);
	FMODCheckError(result);
	return;
}

void cAudioManager::SetChannelGroupPitch(int channelId, float value)
{
	FMOD_RESULT result = m_ChannelGroupList[channelId]->setPitch(value);
	FMODCheckError(result);
	return;
}

void cAudioManager::SetChannelGroupPan(int channelId, float value)
{
	FMOD_RESULT result = m_ChannelGroupList[channelId]->setPan(value);
	FMODCheckError(result);
	return;
}


bool cAudioManager::GetChannelPlaying(int channelId)
{
	bool isPlaying;
	FMOD_RESULT result = m_ChannelList[channelId]->fmodChannel->isPlaying(&isPlaying);
	FMODCheckError(result);
	return isPlaying;
}

void cAudioManager::GetPlaybackPosition(int channelId, unsigned int& value)
{
	FMOD_RESULT result = m_ChannelList[channelId]->fmodChannel->getPosition(&value, FMOD_TIMEUNIT_MS);
	FMODCheckError(result);
	return;
}

/**										**/
/**				3D Attributes			**/
/**										**/

void cAudioManager::Get3DAttributes(int channelId, glm::vec3 &audioPos, glm::vec3& audioVel)
{
	FMOD_RESULT result;

	FMOD_VECTOR soundPos;
	FMOD_VECTOR soundVel;

	sChannel* channel = m_ChannelList[channelId];
	result = channel->fmodChannel->get3DAttributes(&soundPos, &soundVel);
	FMODCheckError(result);

	FMODToGLM(soundPos, audioPos);
	FMODToGLM(soundVel, audioVel);

	return;
}

void cAudioManager::Set3DAttributes(int channelId, glm::vec3& audioPos, glm::vec3& audioVel)
{
	FMOD_RESULT result;

	FMOD_VECTOR soundPos;
	FMOD_VECTOR soundVel;

	GLMToFMOD(audioPos, soundPos);
	GLMToFMOD(audioVel, soundVel);

	sChannel* channel = m_ChannelList[channelId];
	result = channel->fmodChannel->set3DAttributes(&soundPos, &soundVel);
	FMODCheckError(result);

	return;
}

void cAudioManager::SetListenerAttributes(const glm::vec3& position,
	const glm::vec3& velocity, const glm::vec3& forward, const glm::vec3& up)
{
	FMOD_VECTOR fmodPosition;
	FMOD_VECTOR fmodVelocity;
	FMOD_VECTOR fmodForward;
	FMOD_VECTOR fmodUp;

	GLMToFMOD(position, fmodPosition);
	GLMToFMOD(velocity, fmodVelocity);
	GLMToFMOD(forward, fmodForward);
	GLMToFMOD(up, fmodUp);

	FMOD_RESULT result = m_System->set3DListenerAttributes(
		0, &fmodPosition, &fmodVelocity, &fmodForward, &fmodUp);

	float direct, reverb;
	FMOD_VECTOR origin;
	GLMToFMOD(glm::vec3(0.f), origin);
	m_System->getGeometryOcclusion(&origin, &fmodPosition, &direct, &reverb);
	/*printf("ListenerPosition: %.2f, %.2f, %.2f | direct: %.4f, reverb: %.4f\n", position.x, position.y, position.z, direct, reverb);*/
	FMODCheckError(result);
}

void cAudioManager::GetListenerAttributes(glm::vec3& position,
	glm::vec3& velocity, glm::vec3& forward, glm::vec3& up)
{
	FMOD_VECTOR fmodPosition;
	FMOD_VECTOR fmodVelocity;
	FMOD_VECTOR fmodForward;
	FMOD_VECTOR fmodUp;

	FMOD_RESULT result = m_System->get3DListenerAttributes(
		0, &fmodPosition, &fmodVelocity, &fmodForward, &fmodUp);
	FMODCheckError(result);

	FMODToGLM(fmodPosition, position);
	FMODToGLM(fmodVelocity, velocity);
	FMODToGLM(fmodForward, forward);
	FMODToGLM(fmodUp, up);
}

void cAudioManager::CreateNewGeometry(int maxPoly, int maxVert)
{
	FMOD_RESULT result;
	FMOD::Geometry* m_Geometry;
	result = m_System->createGeometry(maxPoly, maxVert, &m_Geometry);
	vpGeometry.push_back(m_Geometry);
	FMODCheckError(result);

	return;
}

int cAudioManager::AddPolygon(float direct, float reverb, bool doublesided,
	const std::vector<glm::vec3>& vertices)
{
	FMOD_RESULT result;
	int polyIndex = m_PolygonIndex;
	m_PolygonIndex++;

	// Add the polygon
	int numVertices = vertices.size();
	FMOD_VECTOR* fmodVertices = (FMOD_VECTOR*)malloc(sizeof(FMOD_VECTOR) * numVertices);
	for (int i = 0; i < numVertices; i++) {
		GLMToFMOD(vertices[i], fmodVertices[i]);
		//printf("%.2f, %.2f, %.2f\n", fmodVertices[i].x, fmodVertices[i].y, fmodVertices[i].z);
	}

	int vecIndex = vpGeometry.size() - 1;
	result = vpGeometry[vecIndex]->addPolygon(direct, reverb, doublesided, numVertices, fmodVertices, &polyIndex);
	FMODCheckError(result);

	FMOD_VECTOR v;
	glm::vec3 g;
	for (int i = 0; i < numVertices; i++)
	{
		vpGeometry[vecIndex]->getPolygonVertex(polyIndex, i, &v);
		FMODToGLM(v, g);
		//printf("%.2f, %.2f, %.2f = %.2f, %.2f, %.2f\n", g.x, g.y, g.z,
		//	vertices[i].x, vertices[i].y, vertices[i].z);
	}

	vpGeometry[vecIndex]->setActive(true);

	return polyIndex;
}

void cAudioManager::SetGeometryScale(const glm::vec3& scale)
{
	// Set scale
	int index = vpGeometry.size() - 1;

	FMOD_RESULT result;
	FMOD_VECTOR fmodScale;

	GLMToFMOD(scale, fmodScale);
	result = vpGeometry[index]->setScale(&fmodScale);
	FMODCheckError(result);

	return;
}

void cAudioManager::SetGeometryPosition(const glm::vec3& position)
{
	// Set position
	int index = vpGeometry.size() - 1;

	FMOD_RESULT result;
	FMOD_VECTOR fmodPosition;

	GLMToFMOD(position, fmodPosition);
	result = vpGeometry[index]->setPosition(&fmodPosition);
	FMODCheckError(result);

	return;
}

void cAudioManager::SetGeometryRotation(const glm::vec3& forward, const glm::vec3& up)
{
	// Set rotation
	int index = vpGeometry.size() - 1;

	FMOD_RESULT result;
	FMOD_VECTOR fmodForward;
	FMOD_VECTOR fmodUp;

	GLMToFMOD(forward, fmodForward);
	GLMToFMOD(up, fmodUp);
	result = vpGeometry[index]->setRotation(&fmodForward, &fmodUp);
	FMODCheckError(result);
}

unsigned int cAudioManager::GetData(const char* sound, void* data)
{
	sAudio* audio = m_AudioMap[sound];
	unsigned int length = 0;
	audio->Audio->getLength(&length, FMOD_TIMEUNIT_RAWBYTES);
	unsigned int bytesRead;
	FMOD_RESULT result = audio->Audio->readData(data, length, &bytesRead);
	FMODCheckError(result);
	return bytesRead;
}
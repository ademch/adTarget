
#include "stdafx.h"
#include "SoundAL.h"

namespace SoundAL
{
	bool _eax_initialized = false;
	bool _initialized     = false;
	bool _loaded          = false;

	ALCdevice*  pDevice  = nullptr;
	ALCcontext* pContext = nullptr;
}


// EAX Function pointers
//EAXSet _eax_set;
//EAXGet _eax_get;

void _init_eax4();
void _shutdown_eax4();



bool SoundAL::Initialize()
{
	printf("Initializing OpenAL...\n");

	// Position of the Listener.
	ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };

	// Velocity of the Listener.
	ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };

	// Orientation of the Listener. (first 3 elements are "at", second 3 are "up")
	// Also note that these should be units of '1'.
	ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };

	// Open default sound device
	pDevice = alcOpenDevice(NULL);
	if (!pDevice)
	{
		printf("Default sound device not present\n");
		return false;
	}

	// Creating rendering context
	pContext = alcCreateContext(pDevice, NULL);
	if (!CheckALCError())
	{   
		printf("Error creating OpenAL context\n");
		return false;
	}

	alcMakeContextCurrent(pContext);

	// Set listener properties
	alListenerfv(AL_POSITION,    ListenerPos);
	alListenerfv(AL_VELOCITY,    ListenerVel);
	alListenerfv(AL_ORIENTATION, ListenerOri);

	//	_init_eax4();

	_initialized = true;

	return true;
}


void SoundAL::DestroyOpenAL()
{
	if (!_initialized) return;

	//	_shutdown_eax4();

	// Clear all buffers and sources
	//???

	// Shut down context
	alcMakeContextCurrent(NULL);

	alcDestroyContext(pContext);

	alcCloseDevice(pDevice);
}


ALboolean SoundAL::CheckALCError()
{
	ALenum ErrCode;
	std::string Err = "ALC error: ";

	if ((ErrCode = alcGetError(pDevice)) != ALC_NO_ERROR)
	{
		Err += alcGetString(pDevice, ErrCode);

		printf("OpenALsnd error: %s", Err.data());
		return AL_FALSE;
	}
	return AL_TRUE;
}

ALboolean SoundAL::CheckALError()
{
	ALenum ErrCode;
	std::string Err = "OpenAL error: ";

	if ((ErrCode = alGetError()) != AL_NO_ERROR)
	{
		Err += alGetString(ErrCode);

		printf("OpenALsnd error: %s", Err.data());
		return AL_FALSE;
	}
	return AL_TRUE;
}


void SoundAL::SetGain(ALuint mSourceID, float gain)
{
	if (!_initialized) return;
	if (!_loaded) return;
	
	alSourcef(mSourceID, AL_GAIN, gain);
}

void SoundAL::SetSourceDefaultParams(ALuint mSourceID)
{
	ALfloat fSourceGain        = 1.0f;
	ALfloat vSourceVelocity[3] = {0, 0, 0};
	ALfloat vSourcePos[3]      = {0, 0, 0};
	ALfloat fSourcePitch       = 1.0f;

	alSourcef (mSourceID, AL_PITCH,		fSourcePitch);
	alSourcef (mSourceID, AL_GAIN,		fSourceGain);
	alSourcefv(mSourceID, AL_POSITION,	vSourcePos);
	alSourcefv(mSourceID, AL_VELOCITY,	vSourceVelocity);
	alSourcei (mSourceID, AL_LOOPING, AL_FALSE);
}

void SoundAL::Play(ALuint mSourceID)
{
	if (!_initialized) return;

	if (!_loaded) return;

	if (_IsPlaying(mSourceID))
		return;

	alSourcePlay(mSourceID);
}

void SoundAL::Pause(ALuint mSourceID)
{
	if (!_initialized) return;
	if (!_loaded) return;

	if (!_IsPlaying(mSourceID))
		return;

	alSourcePause(mSourceID);
}

bool SoundAL::_IsPlaying(ALuint mSourceID)
{
	ALenum state = 0;

	if (!_initialized) return false;

	alGetSourcei(mSourceID, AL_SOURCE_STATE, &state);

	return (state == AL_PLAYING);
}


void SoundAL::PrintSndInfo()
{
	if (!_initialized) return;

	printf("    ALC info: \n");
	printf("      DEFAULT_DEVICE_SPECIFIER: %s\n", alcGetString(pDevice, ALC_DEFAULT_DEVICE_SPECIFIER));
	printf("      Device_specifier: %s\n",         alcGetString(pDevice, ALC_DEVICE_SPECIFIER));
	printf("      alc extensions: %s\n",           alcGetString(pDevice, ALC_EXTENSIONS));
	printf("    AL info:\n");
	printf("      Version: %s\n",                  alGetString(AL_VERSION));
	printf("      Renderer: %s\n",                 alGetString(AL_RENDERER));
	printf("      Vendor: %s\n",                   alGetString(AL_VENDOR));
	printf("      al extensions: %s\n",            alGetString(AL_EXTENSIONS));
}

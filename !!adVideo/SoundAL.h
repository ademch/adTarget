#ifndef SOUND_AL_H
#define SOUND_AL_H

#include "OpenAL/include/al.h"
#include "OpenAL/include/alc.h"


// Typedefs
typedef struct
{
	unsigned int	ID;
	unsigned int	Rate;
	unsigned int	Format;
} SndInfo;


namespace SoundAL
{
	bool Initialize();
	void DestroyOpenAL();

	void Play(ALuint mSourceID);
	void Pause(ALuint mSourceID);
	void SetGain(ALuint mSourceID, float gain);
	void SetSourceDefaultParams(ALuint mSourceID);

	void PrintSndInfo();

	ALboolean CheckALCError();
	ALboolean CheckALError();

	bool _IsPlaying(ALuint mSourceID);

	extern bool _eax_initialized;
	extern bool _initialized;
	extern bool _loaded;

	extern ALCdevice*  pDevice;
	extern ALCcontext* pContext;

}
#endif
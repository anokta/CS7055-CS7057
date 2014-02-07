#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include "fmod\inc\fmod.hpp"
#include <map>
#include <string>

#include "AudioEventManager.h"

/**
* FMOD audio library is used to handle audio events. For more information: http://www.fmod.org/
*/
class AudioManager {

public:
	// Maintain the manager
	inline static AudioManager * GetInstance() { return _instance; }
	static void Create(); 
	static void Destroy();

	bool Initialize();
	void Shutdown();

	// Maintain the resources
	void LoadResources();

	void AddSample(std::string, const char*, bool = false);
	void AddMusic(std::string, const char*, bool = false);

	// Audio properties
	inline int GetBarCount() { return barCount; }
	inline int GetBeatCount() { return beatCount; }
	inline float GetFrequencyBand(int index) { return bands[index]; }
	inline float * GetFrequencyBands() { return bands; }

	// Manipulate the sound objects
	void Play(std::string, float = 1.0f, bool = false);
	void Pause(std::string);
	void Resume(std::string);
	void Stop(std::string);

	void SetVolume(std::string, float);

	bool IsChannelOn(int i);
	void StartMusic();
	void ToggleAddon(int i);
	void ChangeSong(int index);

	// Update the system
	void Update();

protected:
	static AudioManager * _instance;	///< The Unique Instance.

	FMOD::System *system;	///< FMOD Audio System.		
	std::map<std::string, FMOD::Sound*> resources;	///< Audio Resources.
	std::map<std::string, FMOD::Channel*> channels; ///< Looping Audio Channels.

	// Audio properties
    static const int BPM = 140;
    static const int BARS = 8;
    static const int BEATS = 8;

    int barCount;
    int beatCount;

    float timeInterval, timeNextBar;

	std::string labels[9];
	bool labelChannels[9];
	int labelSize;

	// Audio spectrum analysis
	static const int SAMPLE_SIZE = 512;
	float samples[SAMPLE_SIZE];

	static const int BAND_SIZE = 8;
	float bands[BAND_SIZE];

	bool kickPerceived, snarePerceived;

	void analyzeSpectrum();
};

#endif
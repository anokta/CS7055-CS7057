#ifndef AUDIO_EVENT_MANAGER_H
#define AUDIO_EVENT_MANAGER_H

#include "AudioEvent.h"
#include "AudioListener.h"

#include <vector>

class AudioEventManager
{
public:
	// Singleton pattern
	inline static AudioEventManager * GetInstance() { return _instance; };
	static void Create(); 
	static void Destroy();
	
	void AddListener(AudioListener * listener);

	// Audio event triggers
	void TriggerOnNextBar();
	void TriggerOnNextBeat();
	void TriggerOnKick();
	void TriggerOnSnare();

	// Audio events
	AudioEvent OnNextBar, OnNextBeat, OnKick, OnSnare;

private:
	static AudioEventManager * _instance;

	std::vector<AudioListener*> audioListeners;
};

#endif
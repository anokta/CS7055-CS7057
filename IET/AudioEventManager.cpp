#include "AudioEventManager.h"

AudioEventManager * AudioEventManager::_instance = NULL;

void AudioEventManager::Create() { 
	if(_instance == NULL){
		_instance = new AudioEventManager();
	}
}

void AudioEventManager::Destroy() {
	delete _instance;
	_instance = NULL;
}

void AudioEventManager::AddListener(AudioListener * listener)
{
	audioListeners.push_back(listener);
}

void AudioEventManager::TriggerOnNextBar()
{
	OnNextBar();

	for(unsigned int i=0; i<audioListeners.size(); ++i)
		audioListeners[i]->OnNextBar();
}

void AudioEventManager::TriggerOnNextBeat()
{
	OnNextBeat();

	for(unsigned int i=0; i<audioListeners.size(); ++i)
		audioListeners[i]->OnNextBeat();
}

void AudioEventManager::TriggerOnKick()
{
	OnKick();

	for(unsigned int i=0; i<audioListeners.size(); ++i)
		audioListeners[i]->OnKick();
}

void AudioEventManager::TriggerOnSnare()
{
	OnSnare();

	for(unsigned int i=0; i<audioListeners.size(); ++i)
		audioListeners[i]->OnSnare();
}
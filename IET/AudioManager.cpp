#include "AudioManager.h"

#include "fmod\inc\fmod_errors.h"
#include <algorithm>
#include <iostream>

AudioManager * AudioManager::_instance = NULL;

/**
* Create the unique instance.
*/
void AudioManager::Create() { 
	if(_instance == NULL){
		_instance = new AudioManager();
	}
}

/**
* Destroy the unique instance.
*/
void AudioManager::Destroy() {
	delete _instance;
	_instance = NULL;
}

/**
* Initialize the system. 
*/
bool AudioManager::Initialize(){
	FMOD_RESULT result;

	result = FMOD::System_Create(&system);		// Create the main system object.
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		return false;
	}

	result = system->init(100, FMOD_INIT_NORMAL, 0);	// Initialize FMOD.
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		return false;
	}

	return true;
}

/**
* Shutdown and cleanup the system.
*/
void AudioManager::Shutdown() {
	channels.clear();
	resources.clear();
	system->release();
}

/**
* Load all the external audio resources into the memory.
*/
void AudioManager::LoadResources() {

	labelSize = 8;

	for(int i=0; i<labelSize; ++i)
		labels[i] = "Song " + std::to_string(i+1);
	
	AddMusic("Song 1", "..\\IET\\res\\audio\\over.mp3", true);
	AddMusic("Song 2", "..\\IET\\res\\audio\\insistent.mp3", true);
	AddMusic("Song 3", "..\\IET\\res\\audio\\Dare.mp3", true);
	AddMusic("Song 4", "..\\IET\\res\\audio\\rockstar.mp3", true);
	AddMusic("Song 5", "..\\IET\\res\\audio\\cider.mp3", true);
	AddMusic("Song 6", "..\\IET\\res\\audio\\sharamjey.mp3", true);
	AddMusic("Song 7", "..\\IET\\res\\audio\\sunday.mp3", true);
	AddMusic("Song 8", "..\\IET\\res\\audio\\lorem ipsum.mp3", true);
	

	timeInterval = 60.0f / (BEATS / 4.0f) / BPM;
}

/**
* Add the sample (short-play) resource into the container to be used later.
* \param tag Name for the sample
* \param fileName Filepath of the sample resource
*/
void AudioManager::AddSample(std::string tag, const char* fileName, bool loop) {
	FMOD::Sound *sound;
	system->createSound(fileName, loop ? FMOD_LOOP_NORMAL : FMOD_DEFAULT, 0, &sound);
	resources[tag] = sound;
}

/**
* Add the stream (long-play) resource into the container to be used later.
* \param tag Name for the sample
* \param fileName Filepath of the sample resource
*/
void AudioManager::AddMusic(std::string tag, const char* fileName, bool loop) {
	FMOD::Sound *sound;
	system->createStream(fileName, loop ? FMOD_LOOP_NORMAL : FMOD_DEFAULT, 0, &sound);
	resources[tag] = sound;
}

/**
* Obtain a channel, register and start playing the given sound.
* \param tag Name of the sound to be played.
* \param volume Play volume.
* \param paused In case of pushing a "paused" sound into the channel (should not be used in general case).
*/
void AudioManager::Play(std::string tag, float volume, bool paused) {
	FMOD::Channel *channel;
	system->playSound(FMOD_CHANNEL_FREE, resources[tag], paused, &channel);
	channel->setVolume(volume);

	FMOD_MODE mode;
	resources[tag]->getMode(&mode);
	if(mode == 0x000200ca || mode == 0x0000004a)	channels[tag] = channel; // FMOD_LOOP_NORMAL
}

/**
* Pause the playing sound.
* \param tag Name of the sound to be paused.
*/
void AudioManager::Pause(std::string tag) {
	channels[tag]->setPaused(true);
}

/**
* Resume playing the paused sound.
* \param tag Name of the sound to be resumed.
*/
void AudioManager::Resume(std::string tag){
	channels[tag]->setPaused(false);
}

/**
* Change volume of the playing sound.
* \param tag Name of the sound.
*/
void AudioManager::SetVolume(std::string tag, float volume) {
	channels[tag]->setVolume(volume);
}

/**
* Stop and unregister the sound with the given name.
* \param tag Name of the sound to be stopped.
*/
void AudioManager::Stop(std::string tag) {
	channels[tag]->stop();
	channels[tag] = NULL;
}

bool AudioManager::IsChannelOn(int i)
{
	float volume; channels[labels[i]]->getVolume(&volume);
	return volume > 0.0f;
}

void AudioManager::StartMusic()
{
	barCount = 0;
	beatCount = 0;

	timeNextBar = 0;

	//for(int i=0; i<7; ++i)
	//{
	//	Play(labels[i], i==0 ? 1.0f : 0.0f);
	//}

	//Play("Music");
}

void AudioManager::ToggleAddon(int i)
{
	float volume; channels[labels[i]]->getVolume(&volume);
	SetVolume(labels[i], 1.0f - volume);
	labelChannels[i] = !labelChannels[i];
}

void AudioManager::ChangeSong(int index)
{
	for(int i=0; i<labelSize; ++i)
	{
		Stop(labels[i]);
	}

	Play(labels[index]);
}

/**
* Update the system (just necessary for certain audio events).
*/
void AudioManager::Update() {
	system->update();

	analyzeSpectrum();
	
	//// get the current track position
	//unsigned int currentTimeMiliseconds;
	//channels[labels[0]]->getPosition(&currentTimeMiliseconds, FMOD_TIMEUNIT_MS);
	
	//float currentTime = currentTimeMiliseconds / 1000.0f;

	//// check if looped back
	//if (currentTime + timeInterval < timeNextBar)
	//{
	//	timeNextBar = 0;
	//}

	//// check if the next beat has come
	//if (currentTime >= timeNextBar)
	//{
	//	kickPerceived = false;
	//	snarePerceived = false;

	//	timeNextBar += timeInterval;

	//	beatCount = beatCount % BEATS + 1;

	//	if (beatCount == 1)
	//	{
	//		barCount = barCount % BARS + 1;
	//		AudioEventManager::GetInstance()->TriggerOnNextBar();
	//	}

	//	AudioEventManager::GetInstance()->TriggerOnNextBeat();
	//}

	//// detect audio events
	//if(!kickPerceived && bands[0] > 3.48f)  
	//{
	//	kickPerceived = true;
	//	AudioEventManager::GetInstance()->TriggerOnKick();
	//} 

	//if(!snarePerceived && bands[6] > 10.0f)  
	//{
	//	snarePerceived = true;
	//	AudioEventManager::GetInstance()->TriggerOnSnare();
	//}
}

void AudioManager::analyzeSpectrum()
{
	// Get spectrum for left and right stereo channels
	float *samplesLeft, *samplesRight;

	samplesLeft = new float[SAMPLE_SIZE];
	samplesRight = new float[SAMPLE_SIZE];

	system->getSpectrum(samplesLeft, SAMPLE_SIZE, 0, FMOD_DSP_FFT_WINDOW_BLACKMANHARRIS);
	system->getSpectrum(samplesRight, SAMPLE_SIZE, 1, FMOD_DSP_FFT_WINDOW_BLACKMANHARRIS);

	// take the average
	for (int i = 0; i < SAMPLE_SIZE; i++)
	{
		samples[i] = (samplesLeft[i] + samplesLeft[i]) / 2;
	}

	// compute the frequency bands
	int count = 0;

	for (int i = 0; i < BAND_SIZE; ++i)
	{
		int sampleCount = (int)pow(2, i) * 2;

		float average = 0;
		for (int j = 0; j < sampleCount; ++j)
			average += samples[count + j] * (count + j + 1);
		average /= sampleCount;

		bands[i] = 10 * average;

		count += sampleCount;
	}

	//float total = 0;
	//for(int i=0; i< BAND_SIZE; ++i)
	//{
	//	total += bands[i];
	//}
	//if(total > 0)
	//for(int i=0; i< BAND_SIZE; ++i)
	//{
	//	bands[i] /= total;
	//}

	delete [] samplesLeft;
	delete [] samplesRight;
}
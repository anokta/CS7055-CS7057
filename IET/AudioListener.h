#ifndef AUDIO_LISTENER_H
#define AUDIO_LISTENER_H

class AudioListener
{
public:
	virtual void OnNextBar() = 0;
	virtual void OnNextBeat() = 0;
	virtual void OnKick() = 0;
	virtual void OnSnare() = 0;
};

#endif
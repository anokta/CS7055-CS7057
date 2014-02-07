#ifndef AUDIO_EVENT_H
#define AUDIO_EVENT_H

#include <deque>

class AudioEvent
{
	typedef void(*Function)();

	std::deque<Function> deque;

public:
	AudioEvent& operator +=(Function f)
	{
		deque.push_back(f);
		return *this;
	}

	void operator()() 
	{ 
		for (std::deque<Function>::iterator it(deque.begin()); it != deque.end(); ++it)
			(*it)();
	}
};

#endif
#pragma once


class Timer {

	float length;
	float time;
	bool bTimeOut = false;
public:	
	Timer(float ln) : length(ln), time(0.f){}

	void step(float deltaTime) {
		time += deltaTime;

		if (time >= length) {
			time -= length;
			bTimeOut = true;
		}
	}

	bool isTimeOut() const { return bTimeOut; }
	float getTime() const { return time; }
	float getLenght() const { return length; }
	void reset() { time = 0.f; }
};
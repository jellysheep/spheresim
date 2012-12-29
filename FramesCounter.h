
#ifndef _FRAMESCOUNTER_H_
#define _FRAMESCOUNTER_H_

class FramesCounter {

protected:
	long frameCounter;
public:
	FramesCounter(){
		frameCounter = 0;
	}
	
	long popFramesCounter(){
		long tmp = frameCounter;
		frameCounter -= tmp;
		return tmp;
	}
	
};

#endif

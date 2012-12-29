#ifndef _CLTIMER_H_
#define _CLTIMER_H_

#include <QThread>
#include "CirclesHolder.h"
#include "FramesCounter.h"

class ClTimer: public QThread, public FramesCounter {

	Q_OBJECT // must include this if you use Qt signals/slots

protected:
	void run();
	CirclesHolder* circlesHolder;

public:
	ClTimer(CirclesHolder* ch);
};

#endif  /* _CLTIMER_H_ */



#ifndef _NANOSECONDTIMER_H_
#define _NANOSECONDTIMER_H_

class NanosecondTimer {

protected:
	long long lastNS;
	
	timespec now;
	long long getNS(){
		#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
			clock_serv_t cclock;
			mach_timespec_t mts;
			host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
			clock_get_time(cclock, &mts);
			mach_port_deallocate(mach_task_self(), cclock);
			now.tv_sec = mts.tv_sec;
			now.tv_nsec = mts.tv_nsec;
		#else
			clock_gettime(CLOCK_REALTIME, &now);
		#endif

		return ((1000000000ll*now.tv_sec)+now.tv_nsec);
	}
public:
	NanosecondTimer(){
		lastNS = getNS();
	}
	
	long getElapsedNS(){
		long long tmp = lastNS;
		lastNS = getNS();
		return lastNS-tmp;
	}
	
};

#endif

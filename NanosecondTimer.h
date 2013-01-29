
#ifndef _NANOSECONDTIMER_H_
#define _NANOSECONDTIMER_H_

class NanosecondTimer {

protected:
	long long lastNS;
	
public:
	NanosecondTimer(){
		lastNS = getNS();
	}
	
	static long long getNS(){
#ifdef __WIN32__
		return GetTickCount(void)*1000ll*1000ll;
#else
		static timespec now;
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
#endif
	}
	
	long getElapsedNS(){
		long long tmp = lastNS;
		lastNS = getNS();
		return lastNS-tmp;
	}
	
};

#endif

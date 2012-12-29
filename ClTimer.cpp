
#include "ClTimer.h"
#include <cstdio>

ClTimer::ClTimer(CirclesHolder* ch){
	circlesHolder = ch;
}

void ClTimer::run(){
	printf("clTimer!\n");
	while(true){
		frameCounter++;
	}
}


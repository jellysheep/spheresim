
#include "ClTimer.h"
#include <stdio.h>

ClTimer::ClTimer(CirclesHolder* ch){
	circlesHolder = ch;
}

void ClTimer::run(){
	printf("clTimer!\n");
}

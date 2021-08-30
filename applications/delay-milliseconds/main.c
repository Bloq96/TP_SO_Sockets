#include <stdio.h>
#include "delay_milliseconds.h"

int main(int argc, char *argv[]) {
    if(argc>=2) {
	int it = 0, timeMilliseconds = 0;
        while(1) {
	    if(argv[1][it]=='\0') {
	        break;
	    } else if(((int)argv[1][it])-48<0||
	    ((int)argv[1][it])-48>9) {
	        return 1;
	    } else {
	        timeMilliseconds = timeMilliseconds*10 - 48 +
		((int)argv[1][it]);
	    }
	    ++it;
	}
	delayMilliseconds(timeMilliseconds);
	printf("Waited %d ms...\n",timeMilliseconds);
	return 0;
    }
    return 1;
}

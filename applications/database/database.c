#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "delay_milliseconds.h"
#include "string_compare.h"

int main(void) {
    srand(time(NULL));
    FILE *annualAverages;
    annualAverages = fopen("annual-averages.csv","w");
    if(annualAverages!=NULL) {	
	char buffer[257];
	for(int it=1;it<=10;++it) {
            syscall(548,'R',NULL,buffer);
            while(stringCompare((char *)buffer,"Empty buffer!")==0) {
	        delayMilliseconds(1000+(1000*rand())/RAND_MAX);
                syscall(548,'R',NULL,buffer);
            }
	    fprintf(annualAverages,"%dº, %s\n", it, buffer);
	    delayMilliseconds(1000+(1000*rand())/RAND_MAX);
	}
	fclose(annualAverages);
    }
    return 0;
}

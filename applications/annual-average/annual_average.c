#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "delay_milliseconds.h"
#include "string_compare.h"

typedef struct {
    float value;
    int validity;
} Number;

Number readNumber(FILE *filePointer) {
    Number result;
    result.validity = 0;
    result.value = 0;
    if(filePointer==NULL) {
        return result;
    }
    char aux = ' ';
    while(1) {
        if(fscanf(filePointer,"%c",&aux)==EOF) {
	    return result;
	} else if (aux=='\n') {
	    return result;
	} else if(result.validity>0) {
	    continue;
	} else if (((int)aux>=48&&(int)aux<=57)||aux=='-') {
            int decimals = 0;
            float value = 0;
	    int signal = 1;
	    if(aux=='-') {
	        signal = -1;
                if(fscanf(filePointer,"%c",&aux)==EOF) {
		    return result;
		}
	    }
	    while(((int)aux>=48&&(int)aux<=57)||aux=='.') {
	        if(aux=='.') {
		    ++decimals;
		} else {
		    if(decimals>0) {
		        decimals *= 10;
		    }
		    value = value*10 - 48 + ((int)aux); 
	        }
                if(fscanf(filePointer,"%c",&aux)==EOF) {
	            if(decimals>0) {
	                value /= decimals;
	            }
	            result.value = value*signal;
	            result.validity = 1;
		    return result;
		}
	    }
	    if(decimals>0) {
	        value /= decimals;
	    }
	    result.value = value*signal;
	    result.validity = 1;
	    if(aux=='\n') {
	        return result;
	    }
	}
    }
}

void removeLine(FILE *filePointer) {
    if(filePointer==NULL) {
        return;
    }
    char aux = ' ';
    while(1) {
	if(fscanf(filePointer,"%c",&aux)==EOF) {
	    return;
	} else if (aux=='\n'){
	    return;
	}
    }
}

int main(void) {
    srand(time(NULL));
    FILE *monthlyAverages;
    monthlyAverages = fopen("monthly-averages.csv","r");
    if(monthlyAverages!=NULL) {	
        int numberOfDays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30,
	31, 30, 31};
        removeLine(monthlyAverages);
        Number currentValue = readNumber(monthlyAverages);
	int it = 0, count = 0;
	float average = 0;
	char buffer[257], number[10];
	while(currentValue.validity>0) {
	    average += currentValue.value*numberOfDays[it];
            ++it;
	    if(it==12) {
		++count;
	        it = 0;
		average /= 365;
                gcvt(average,6,number);
		syscall(548,'W',number,buffer);
		while(stringCompare((char *)buffer,"Buffer full!")==0) {
		    delayMilliseconds(1000+(1000*rand())/RAND_MAX);
		    syscall(548,'W',number,buffer);
		}
		printf("%dº %.2f\n",count,average);
		delayMilliseconds(1000+(1000*rand())/RAND_MAX);
		average = 0;
	    }
	    currentValue = readNumber(monthlyAverages);
	}
        fclose(monthlyAverages);
    }
    return 0;
}

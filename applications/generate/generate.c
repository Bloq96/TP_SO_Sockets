#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    srand(time(NULL));
    FILE *filePointer;
    filePointer = fopen("monthly-averages.csv","w");
    if(filePointer != NULL) {
        char *months[12] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN",
	"JUL", "AGU", "SEP", "OCT", "NOV", "DEC"};
	fprintf(filePointer, "MON,   AVG  \n");
        for(int it=0;it<120;++it) {
	    fprintf(filePointer,"%s, %.2f\n", months[it%12],
	    7000.0*((float) rand())/((float) RAND_MAX)-2000.0);
	}
        fclose(filePointer);
    }
    return 0;
}

#include <stdio.h>

int stringCompare(char *string1, char *string2) {
    if(string1==NULL||string2==NULL) {
        return 2;
    }
    int it = 0;
    while(1) {
	if(string1[it]=='\0'||string2[it]=='\0') {
	    if(string1[it]=='\0'&&string2[it]=='\0') {
	        return 0;
	    } else {
	        return 1;
	    }
	} else if(string1[it]!=string2[it]) {
	    return 1;
	}
	++it;
    }	 
}

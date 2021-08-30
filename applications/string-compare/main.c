#include <stdio.h>
#include "string_compare.h"

int main(int argc, char *argv[]) {
    if(argc>=3) {
        switch(stringCompare(argv[1],argv[2])) {
	    case 0:
                printf("TRUE\n");
		break;
	    default:
		printf("FALSE\n");
	};
	return 0;
    }
    return 1;
}

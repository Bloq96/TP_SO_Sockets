#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    if(argc<2) {
        return 1;
    }
    char buffer[257];
    syscall(548,'W',argv[1],(char *)buffer);
    return 0;
}

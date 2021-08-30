#include <unistd.h>
#include <stdio.h>

int main(void) {
    char buffer[257];
    syscall(548,'R',NULL,(char *)buffer);
    printf("%s\n",(char *)buffer);
    return 0;
}

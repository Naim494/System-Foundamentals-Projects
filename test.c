#include <stdio.h>

#define OS Linux

#if OS == Linux
	puts(... "Linux!");
#else
	puts(... "Something else");
#endif

int main(void){

	return 0;
}

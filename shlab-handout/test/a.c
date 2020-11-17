#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void sigint_handler(int sig){
	printf("a\n");
	sleep(5);
}

int main(){
	signal(SIGINT,sigint_handler);

	pause();
	printf("b\n");
	sleep(1);
	printf("c\n");

	return 0;
}

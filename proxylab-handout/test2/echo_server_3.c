#include "t.h"
#include "rio.h"
#include <pthread.h>

void echo(int connfd);
void *threads(void *vargp);

int main(int argc,char **argv)
{
	int listenfd,*connfdp;
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;
	pthread_t tid;

	if(argc != 2)
	{
		fprintf(stderr,"usage: %s <port>\n",argv[0]);
		exit(0);
	}

	listenfd = open_listenfd(argv[1]);

	while(1)
	{
		clientlen = sizeof(struct sockaddr_storage);
		connfdp = (int *)malloc(sizeof(int));
		*connfdp = accept(listenfd,(struct sockaddr *)&clientaddr,&clientlen);
		pthread_create(&tid,NULL,threads,connfdp);
	}
}


void echo(int connfd)
{
	int n;
	rio_t rio;
	char buf[MAXLINE];
	
	rio_readinitb(&rio,connfd);
	while((n = rio_readlineb(&rio,buf,MAXLINE)) != 0)
	{
		printf("Server receive %d bytes\n",n);
		rio_writen(connfd,buf,n);
	}
}

void *threads(void *vargp)
{
	int connfd = *((int *)vargp);
	pthread_detach(pthread_self());
	free(vargp);
	echo(connfd);
	close(connfd);
	return NULL;
}


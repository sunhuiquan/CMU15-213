/*
 * In an easy way, the sbuf is a buffer for clients which can be 
 * "comsume" by comsumer, and the threads are the comsumers.
 */
#include "t.h"
#include "rio.h"
#include "sbuf.h"
#include <pthread.h>

#define NTHREADS 4
#define SBUFSIZE 16

void  echo_cnt(int connnfd);
void *thread(void *vargp);
static void init_echo_cnt(void);

sbuf_t sbuf;
static int byte_cnt;
static sem_t mutex;

int main(int argc,char **argv)
{
	int i,listenfd,connfd;
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;
	pthread_t tid;

	if(argc != 2)
	{
		fprintf(stderr,"usage: %s <port>\n",argv[0]);
		exit(0);
	}

	listenfd = open_listenfd(argv[1]);

	sbuf_init(&sbuf,SBUFSIZE);
	for(i = 0;i < NTHREADS;++i)
		pthread_create(&tid,NULL,thread,NULL);
	
	while(1)
	{
		clientlen = sizeof(struct sockaddr_storage);
		connfd = accept(listenfd,(struct sockaddr *)&clientaddr,&clientlen);
		sbuf_insert(&sbuf,connfd);
	}
}

void *thread(void *vargp)
{
	pthread_detach(pthread_self());
	while(1)
	{
		int connfd = sbuf_remove(&sbuf);
		echo_cnt(connfd);
		close(connfd);
	}
}

void echo_cnt(int connfd)
{
	int n;
	char buf[MAXLINE];
	rio_t rio;
	static pthread_once_t once = PTHREAD_ONCE_INIT; /* must be a static life long */

	pthread_once(&once,init_echo_cnt);
	rio_readinitb(&rio,connfd);
	while((n = rio_readlineb(&rio,buf,MAXLINE)) != 0)
	{
		sem_wait(&mutex);
		byte_cnt += n;
		printf("server received %d (%d total) bytes on fd %d\n",n,byte_cnt,connfd);
		sem_post(&mutex);
		rio_writen(connfd,buf,n);
	}
}

static void init_echo_cnt(void)
{
	sem_init(&mutex,0,1);
	byte_cnt = 0;
}


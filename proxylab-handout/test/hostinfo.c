#include <stdio.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>

#define MAXSIZE 1024

int main(int argc,char **argv)
{
	struct addrinfo *p,*listp,hints;
	char buf[MAXSIZE];
	int rc,flags;

	if(argc != 2)
	{
		fprintf(stderr,"usage: %s <domain name>\n",argv[0]);
		exit(0);
	}

	memset(&hints,0,sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if((rc = getaddrinfo(argv[1],NULL,&hints,&listp)) != 0)
	{
		fprintf(stderr,"igetaddrinfo errno: %s",gai_strerror(rc));
		exit(1);
	}

	flags = NI_NUMERICHOST;
	for(p = listp;p;p = p->ai_next)
	{
		if((rc = getnameinfo(p->ai_addr,p->ai_addrlen,buf,MAXSIZE,NULL,0,flags)) != 0)
		{
			fprintf(stderr,"%s",gai_strerror(rc));
			exit(1);
		}
	
		struct sockaddr_in *ps = p->ai_addr;
		printf("socket addr: %x\n",ps->sin_addr.s_addr);
		printf("socket port: %x\n",ps->sin_port);
		printf("%s\n",buf);
		printf("\n");
	}
	
	freeaddrinfo(listp);

	exit(0);
}

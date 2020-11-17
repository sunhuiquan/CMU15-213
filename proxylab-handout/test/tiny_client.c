#include <stdlib.h>
#include "t.h"
#include "rio.h"

int main(int argc,char **argv)
{
	int clientfd,n;
	char *host,*port,buf[MAXLINE];
	rio_t rio;

	if(argc != 3)
	{
		fprintf(stderr,"usage: %s <host> <port>\n",argv[0]);
		exit(0);
	}

	host = argv[1];
	port = argv[2];
	
	clientfd = open_clientfd(host,port);
    rio_readinitb(&rio,clientfd);
	fgets(buf,MAXLINE,stdin);
	buf[strlen(buf)] = '\0'; /* the '\n' is no useful and cause trouble */
	rio_writen(clientfd,buf,strlen(buf));
	rio_writen(clientfd,"\r\n\r\n",4); /* don't forget this !!! */

	/* how to make sure when read from response after server did write ???????????????????????????????????????????? */
	while(1){
		n = rio_readlineb(&rio,buf,MAXLINE);
		if(!n)
			break;
		printf("%s",buf);
		printf("n = %d\n",n);
	}
	close(clientfd);

	exit(0);
}

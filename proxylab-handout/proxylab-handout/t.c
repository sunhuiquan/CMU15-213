#include "t.h"

int open_clientfd(char *hostname,char *port)
{
	/* Hostname is domain name, it's a server,because one domain name can reflect on
	 * one or more host address, and we only conect with one host machine to get serve.
	 * 
	 * And you should know that by using getaddrinfo, we don't need to know the detail
	 * about the IPv4 or IPv6, the getaddrinfo's result members can tell us, just use
	 * them, whether IPv4 or IPv6 don't influen the program.
	 */
	int clientfd;
	struct addrinfo hints,*p,*listp;

	memset(&hints,0,sizeof(struct addrinfo));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_NUMERICSERV; /* using a numeric port */
	hints.ai_flags |= AI_ADDRCONFIG; /* reconmended for connect */
	getaddrinfo(hostname,port,&hints,&listp);

	for(p = listp;p;p = p->ai_next)
	{
		/* if failed try again */		
		if((clientfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) < 0)
			continue;

		if(connect(clientfd,p->ai_addr,p->ai_addrlen) != -1)
			break; /* success and break(we only need to connect one server host) */

		/* socket success but connect failed, try another */
		close(clientfd);
	}

	freeaddrinfo(listp);

	if(!p)
		return -1;
	else
		return clientfd;
}

int open_listenfd(char *port)
{
	struct addrinfo *p,*listp,hints;
	int listenfd,optval = 1;

	memset(&hints,0,sizeof(struct addrinfo));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
	hints.ai_flags |= AI_NUMERICSERV; /* using numeric port */
	getaddrinfo(NULL,port,&hints,&listp); /* NULL host can accept on any IP address */

	for(p = listp;p;p = p->ai_next)
	{
		if((listenfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) < 0)
			continue;
	
		setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,(const void *)&optval,sizeof(int));

		if(bind(listenfd,p->ai_addr,p->ai_addrlen) == 0)
			break;
		
		close(listenfd);
	}

	freeaddrinfo(listp);

	if(!p)
		return -1;

	if(listen(listenfd,LISTENQ) < 0)
	{
		close(listenfd);
		return -1;
	}

	return listenfd;
}


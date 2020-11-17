#include "t.h"
#include "rio.h"
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

void doit(int fd);
void read_requesthdrs(rio_t *rp);
int parse_uri(char *uri,char *filename,char *cgiargs);
void serve_static(int fd,char *filename,int filesize);
void get_filetype(char *filename,char *filetype);
void serve_dynamic(int fd,char *filename,char *cgiargs);
void clienterror(int fd,char *cause,char *errnum,char *shortmsg,char *longmsg);

extern char **environ;

int main(int argc,char *argv[]){
    int listenfd,connfd;
    char hostname[MAXLINE],port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;

    if(argc != 2){
	fprintf(stderr,"usage: %s <port>\n",argv[0]);
	exit(1);
    }

    listenfd = open_listenfd(argv[1]);
    while(1){
	clientlen = sizeof(clientaddr);
	connfd = accept(listenfd,(struct sockaddr *)&clientaddr,&clientlen);
	getnameinfo((struct sockaddr *)&clientaddr,clientlen,hostname,MAXLINE,port,MAXLINE,0);
	printf("Accepted connection from(%s,%s)\n",hostname,port);
	doit(connfd);
	close(connfd);
    }

    return 0;
}

void doit(int fd){ // this fd is connfd
    int is_static;
    struct stat sbuf;
    char buf[MAXLINE],method[MAXLINE],uri[MAXLINE],version[MAXLINE];
    char filename[MAXLINE],cgiargs[MAXLINE];
    rio_t rio;

    rio_readinitb(&rio,fd);
    rio_readlineb(&rio,buf,MAXLINE);

    sscanf(buf,"%s %s %s",method,uri,version);
	/* the spc is for the format,no is also ok */
    if(strcasecmp(method,"GET")){ // if not the GET method
		clienterror(fd,method,"501","Not implemented",
			"Tiny does not implement this method");
		return;
    }
	read_requesthdrs(&rio); // rio have already bound with fd(connfd)
    is_static = parse_uri(uri,filename,cgiargs);
    if(stat(filename,&sbuf) < 0){
		char tbuf[MAXLINE];
		sprintf(tbuf,"-%s-%s-%s-\r\n",method,uri,version);
		rio_writen(fd,tbuf,strlen(tbuf));

		clienterror(fd,filename,"404","Not found",
			"Tiny coudn't find this file");
		return;
    }
    if(is_static){
		if(!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)){
	    	//we need to read
	   		clienterror(fd,filename,"403","Forbidden",
	   	 		"Tiny couldn't read the file");	
	    	return;
		}
		serve_static(fd,filename,sbuf.st_size);
    }else{
		if(!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)){
	    	//we need to excuve
	    	clienterror(fd,filename,"403","Forbidden",
	    	"Tiny couldn't run the CGI program");	
	    	return;
		}
		serve_dynamic(fd,filename,cgiargs);
    }
}

void read_requesthdrs(rio_t *rp){
    //we have already get the ruquest line(by rioread and sscanf)
    //now we need get the request header(s) and abondon the last \r\n
    char buf[MAXLINE];
    rio_readlineb(rp,buf,MAXLINE); 
    while(strcmp(buf,"\r\n")){
		rio_readlineb(rp,buf,MAXLINE);
		printf("%s",buf);
    }
    return;
}

void clienterror(int fd,char *cause,char *errnum,char *shortmsg,char *longmsg){
    char buf[MAXLINE],body[MAXLINE];
    /* respect line
     * respect headers
     * respect body
     * */

    //the part of body
    //the content actually is respect body
    sprintf(body,"<html><title>Tiny ERROR</title>");
    sprintf(body,"%s<body bgcolor=""ffffff"">\r\n",body);
    sprintf(body,"%s%s: %s\r\n",body,errnum,shortmsg);
    sprintf(body,"%s<p>%s: %s\r\n",body,longmsg,cause);
    sprintf(body,"%s<hr><em>The Tiny Web Server</em>\r\n",body);

    sprintf(buf,"HTTP/1.0 %s %s\r\n",errnum,shortmsg); // respect line
    rio_writen(fd,buf,strlen(buf));
    sprintf(buf,"Content-type: text/html\r\n"); // respect headers
    rio_writen(fd,buf,strlen(buf));
    sprintf(buf,"Content-length: %d\r\n\r\n",(int)strlen(body));
    rio_writen(fd,buf,strlen(buf));
    rio_writen(fd,body,strlen(body)); // respect body
}

int parse_uri(char *uri,char *filename,char *cgiargs){
/*for example: GET / HTTP/1.1
 *you'll see only  the / is the uri */

   char *ptr;

    if(strstr(uri,"cgi-bin")){ // if uri contain cgi-bin means it's a dynamic
		ptr = index(uri,'?');
		if(ptr){
	    	strcpy(cgiargs,ptr + 1);
	   		*ptr = '\0';
		}
		else
	    	strcpy(cgiargs,"");

		strcpy(filename,".");
		strcat(filename,uri);    
		return 0;
    }else
	{ // it's a static
		strcpy(cgiargs,"");
		strcpy(filename,".");
		strcat(filename,uri);
		if(uri[strlen(uri) - 1] == '/') // if not '/' the end is a file **.**
	    	strcat(filename,"home.html");    
		return 1;
    }
}

void get_filetype(char *filename,char *filetype){
    if(strstr(filename,".html"))
		strcpy(filetype,"text/html");
    else if(strstr(filename,".gif"))
		strcpy(filetype,"image/gif");
    else if(strstr(filename,".png"))
		strcpy(filetype,"image/png");
    else if(strstr(filename,".jpg"))
		strcpy(filetype,"image/jpeg");
    else
		strcpy(filetype,"text/plain");
}

void serve_static(int fd,char *filename,int filesize){
    int srcfd;
    char *srcp,filetype[MAXLINE],buf[MAXLINE];

    get_filetype(filename,filetype);
    sprintf(buf,"HTTP/1.0 200 OK\r\n");
    sprintf(buf,"%sServer: Tiny Web Server\r\n",buf);
    sprintf(buf,"%sConnection: close\r\n",buf);
    sprintf(buf,"%sContent-length: %d\r\n",buf,filesize);
    sprintf(buf,"%sContent-type: %s\r\n\r\n",buf,filetype);
    rio_writen(fd,buf,strlen(buf));

	for(int i = 0;i < 3000;++i){
		long ii = 0;
		while(ii < 10000000)
		{
			++ii;
		}
	}
	printf("finish write to client\n");

    srcfd = open(filename,O_RDONLY,0);
    srcp = mmap(0,filesize,PROT_READ,MAP_PRIVATE,srcfd,0);
    close(srcfd);
    rio_writen(fd,srcp,filesize);
    munmap(srcp,filesize);
}

void serve_dynamic(int fd,char *filename,char *cgiargs){
    char buf[MAXLINE],*emptylist[] = {NULL}; // like a argv
    
    sprintf(buf,"HTTP/1.0 200 OK\r\n");
    rio_writen(fd,buf,strlen(buf));
    sprintf(buf,"Server: Tiny Web Server\r\n");
    rio_writen(fd,buf,strlen(buf));

    if(fork() == 0){
		setenv("QUERY_STRING",cgiargs,1);
		dup2(fd,STDOUT_FILENO);
		execve(filename,emptylist,environ);    
	}
    waitpid(-1,NULL,0);
}


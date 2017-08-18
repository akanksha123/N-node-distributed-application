#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <time.h>
#include <errno.h>

int portarray[3];
void *processrequests(void *);
void *threadprocess(void *);
pthread_mutex_t mutex;
char buffer[256];
char msgbuffer[256];
pthread_t tid[2];
static int clientcount=0,numrequests=0,servertime=rand() % 25,flag=0;
static int ts=rand() % 10;
int portno;
int portno2;
socklen_t clilen;     
struct sockaddr_in serv_addr, cli_addr;
struct hostent *server;
static int GS=0; 
int argcount;
void callclient(int,int[],int,int);
int main(int argc, char *argv[])
{
     
    int sockfd, newsockfd;
    socklen_t clilen;
    char readbuffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n,k;
    int number;
	 
    argcount = argc;
    /*for(int i=1;i<argc;i++)
    {
        portarray[i] = atoi(argv[i]);
    }*/
    /*portno = atoi(argv[2]);
    printf("Port no. is:%d",portno);*/

    for(int s=1;s<=10;s++)
    msgbuffer[s]=0;

    portarray[1] = atoi(argv[1]);
    portarray[2] = atoi(argv[2]);
    portarray[3] = atoi(argv[3]);
    portarray[4] = atoi(argv[4]);

    printf("Inside sequencer\n");
    printf("Port no. is:%d\n",portarray[1]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)  {
        printf("ERROR opening socket");

    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
        //portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portarray[1]);
    printf("POrt no.is:%d\n",portarray[1]);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        printf("ERROR on binding");

    printf("\nBefore listen" );
    int lis = listen(sockfd,5);
    printf("\nLis:%d",lis);
    printf("\nAfter listen");
    printf("listen socket is:%d\n",sockfd);
    clilen = sizeof(cli_addr);
    while(1)
    {

        newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr,&clilen);
        if (newsockfd < 0) 
          printf("ERROR on accept");
        bzero(buffer,256);
        n = read(newsockfd,buffer,255);
        printf("Sequencer received from clients:%s\n",buffer);
        GS=GS+1;
        number = atoi(buffer);
        callclient(argcount,portarray,number,GS);

    }

      
    
   close(newsockfd);
   close(sockfd); 
  return 0; 
    
     
}
void callclient(int p1,int portarray[],int number,int GS)
{
    int sockfd,n,k;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    
        for(k=2;k<p1;k++)
        {
            sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd < 0) 
           printf("ERROR opening socket");
            server = gethostbyname("127.0.0.1");
            if (server == NULL)
            {
                fprintf(stderr,"ERROR, no such host\n");
                exit(0);
            }
            bzero((char *) &serv_addr, sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;
            bcopy((char *)server->h_addr, 
            (char *)&serv_addr.sin_addr.s_addr,
            server->h_length);
            serv_addr.sin_port = htons(portarray[k]);
            sleep(1);
            if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
           printf("ERROR connecting");

            sprintf(buffer,"%d\t%d\n",GS,number);
            //ts = ts + rand() % 3;
            printf("Sending to port: %d\n", portarray[k]);
            printf("Buffer content seq is sending :%s\n",buffer);
            //printf("Port array of sender is:%d\n",portarray[k]);
            n = write(sockfd,buffer,strlen(buffer));
            //bzero(buffer,256);
            if (n < 0) 
            printf("ERROR writing to socket");
    
        close(sockfd);
        }
    
    
  //  return 0;
}


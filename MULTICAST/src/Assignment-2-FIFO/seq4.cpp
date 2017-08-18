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

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int portarray[3];
int portarray1[3];
void *processrequests(void *);
void *threadprocess(void *);
pthread_t tid[2];
static int ts=10;
int portno;
int portno1;
socklen_t clilen;     
struct sockaddr_in serv_addr, cli_addr;
struct hostent *server;
int argcount=0;
char buffer[256];

int main(int argc, char *argv[])
{
     
    
    FILE *output;
    pthread_t pidsend,pidrecv;
    
    portno = atoi(argv[1]);
    portno1 = atoi(argv[2]);
    argcount=argc;
    /*for(int i=1;i<argc;i++){
        portarray[i] = atoi(argv[i]);
    
    printf("Port no. is:%d",portarray[i]);
    }*/
    portarray[1] = atoi(argv[1]);
            portarray[2] = atoi(argv[2]);
            portarray[3] = atoi(argv[3]);
    //printf("argcount is:%d",argcount);
    
    
    pthread_create(&pidrecv,NULL,processrequests,NULL);
    pthread_create(&pidsend,NULL,threadprocess,NULL);
    pthread_join(pidsend,NULL);
    pthread_join(pidrecv,NULL);
        
}

void *processrequests(void *args){
    //Server first creates socket
    
    
    int sockfd, newsockfd;
    socklen_t clilen;
    char readbuffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n,k;
    int number;
    char *data,*dataport;

     while(1)
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) 
        error("ERROR opening socket");

        bzero((char *) &serv_addr, sizeof(serv_addr));
         //portno = atoi(argv[1]);
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(portarray[1]);
        //printf("Port no. is:%d\n",portno);
        if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR on binding");
        int lis = listen(sockfd,5);
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr,&clilen);
        if (newsockfd < 0) 
          error("ERROR on accept");
     
        n = read(newsockfd,buffer,255);
        
        data = strtok(buffer,"\t");
        //printf("Tokened msg is:%s\n",data);
        dataport = strtok(NULL,"\n");
        //printf("Tokened port is:%s\n",dataport);
        printf("-------------------------------\n");
        printf("Received message %s from %s port\n",data,dataport);
        //printf("Port receiving msg:%d\n",portarray[1]);
        printf("-------------------------------\n");
        if (n < 0) 
        printf("ERROR reading from socket");
        close(newsockfd);
        close(sockfd); 
    }
    
    return 0; 
}


void *threadprocess(void *args)
{
    
    int sockfd,n,k;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    while(1){
        for(k=1;k<argcount;k++)
        {
            sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd < 0) 
            error("ERROR opening socket");
            server = gethostbyname("127.0.0.1");
            if (server == NULL)
            {
                fprintf(stderr,"ERROR, no such host\n");
                exit(0);
            }
            bzero((char *) &serv_addr, sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;
            bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
            serv_addr.sin_port = htons(portarray[k]);
            sleep(10);
            if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
            error("ERROR connecting");
            bzero(buffer,256);
            //sprintf(buffer,"%d\t%d",portarray[1],portarray[k]);
            snprintf(buffer,sizeof(buffer),"%d\t%d\n",ts,portarray[k]);
            printf("Buffer content sending is:%s\n",buffer);
            ts++;
            printf("Port of sender is:%d\n",portarray[k]);
            n = write(sockfd,buffer,strlen(buffer));
            if (n < 0) 
            error("ERROR writing to socket");
            close(sockfd);
        }
    }
    
    return 0;
}
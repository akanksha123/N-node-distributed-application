#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

struct records 
{
    int clocktime;
       
}rec[10];


void *processrequests(void *);

pthread_mutex_t mutex;
char buffer[256];

static int clientcount=0,numrequests=0,servertime=rand() % 25,flag=0;

int main(int argc, char *argv[])
{
     
    int sockfd, newsockfd, portno,n;
    int *newsocket;
    socklen_t clilen;     
    struct sockaddr_in serv_addr, cli_addr;
    pthread_mutex_init(&mutex,NULL);
    int i=0;

    // argument count check
    if (argc < 3) {
       printf("Error: Less no. of arguments");
       exit(1);
    }
        
   
    //Server first creates socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    //Check if error in socket creation
    if (sockfd < 0) 
        printf("Error:Creating socket at server");
    //Zeroying out the bufferaaaaazzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzza
    bzero((char *) &serv_addr, sizeof(serv_addr));
    //Host to network byte updater
    portno = atoi(argv[1]);
    numrequests = atoi(argv[2]);
    printf("----------------------------------\n");
    printf("No. of processes are:%d\n",numrequests);
    printf("Server time is:%d\n",servertime);
    printf("----------------------------------\n");
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    //Bind the socket to port
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        printf("Error:in binding socket\n");

    //Listen to requests from cllient if any
    listen(sockfd,5);

    //accept the request from client if any and provide a new socket for further communication.
    
    clilen = sizeof(cli_addr);
    while(newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr,&clilen))
    {
        clientcount++;
            printf("Distributed Node %d is connected\n",clientcount);
        if (newsockfd < 0) 
            printf("Error: in accept");
        
        newsocket = (int *)malloc(1);  
        *newsocket = newsockfd;
        pthread_t pid;
        
        if(pthread_create(&pid,NULL,processrequests,(void*)newsocket) < 0)   
        {
            printf("Error in creating new thread");
            return 1;
        }
        
        
    }
    close(newsockfd);
    close(sockfd);
    return 0; 
}

void *processrequests(void *sockfd)
{
    int synctime=0;
    int nnsocket = *(int*)sockfd;
    int n=0,t=0,diff=0;
    int clktime,avg;
    static int newtime=0;
    
        //Receiving Hi sent from client processes
        bzero(buffer,256);
         n = read(nnsocket,buffer,255);
        if (n < 0) 
        printf("Error:in reading at client end");
        else
        printf("Time daemon received :%s\n",buffer);

        //Send time daemon's time to all distributed nodes
        bzero(buffer,256);
        sprintf(buffer,"%d",servertime);
        n = write(nnsocket,buffer,strlen(buffer));
        if (n < 0) 
        printf("Error: in writing at client end");  

        //read the timestamp differences sent by the distributed nodes
        bzero(buffer,256);
        n = read(nnsocket,buffer,255);
        if (n < 0) 
        printf("Error:in reading at server end");
        clktime = atoi(buffer);
        rec[clientcount].clocktime = clktime;
        printf("Time daemon received clock time diff:%d from node: %d\n",clktime,clientcount);
        printf("----------------------------------\n");
        while(clientcount!=numrequests)
        {
            sleep(1);
        }
        for(int g=1;g<=numrequests;g++)
        {
            diff = diff+rec[g].clocktime;
         
        }
        avg=diff/(numrequests+1);

        if(flag==0){
        printf("**********************************\n");    
        printf("Avg is:%d\n",avg);
        printf("**********************************\n"); 
        }
        // Processes Synchronization
            printf("Clock time difference for node is:%d\n",clktime);
            synctime = (avg-clktime);
            printf("Synctime for node is:%d\n",synctime);
            printf("----------------------------------\n");
            //send adjustments to client
            bzero(buffer,256);
            sprintf(buffer,"%d",synctime);
            n = write(nnsocket,buffer,strlen(buffer));
            if(n < 0)
            printf("Error:in writing at server end");
            //Server's clock sync
            if(flag==0){
            servertime=servertime+avg;
            flag=1;
            printf("**********************************\n"); 
            printf("Server synchronized time is:%d\n",servertime);
            printf("**********************************\n"); 
            }

}

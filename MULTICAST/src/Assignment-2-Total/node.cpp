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
#include <iostream>

using namespace std;

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
static int ts=10,mindex=0,sendcount=0;
int portno;
int portno1;
socklen_t clilen;     
struct sockaddr_in serv_addr, cli_addr;
struct hostent *server;
int argcount=0;
char buffer[256];
char msgbuffer[256];
// std::map<int, string> msgvector;
//std::vector<string> msgvector;
int message;
int main(int argc, char *argv[])
{
     
    
    FILE *output;
    pthread_t pidsend,pidrecv;
    
    portno = atoi(argv[1]);
    portno1 = atoi(argv[2]);
    argcount=argc;
    for(int i=1;i<argc;i++){
        portarray[i] = atoi(argv[i]);
    
    printf("Port no. is:%d",portarray[i]);
    }
    printf("argcount is:%d",argcount);
    
    pthread_create(&pidrecv,NULL,processrequests,NULL);
    pthread_create(&pidsend,NULL,threadprocess,NULL);
    pthread_join(pidsend,NULL);
    pthread_join(pidrecv,NULL);
        
}

void *processrequests(void *args){
    //Server first creates socket
    printf("Inside processrequests function\n");
    int sockfd, newsockfd;
    char *data,*dataport,*check;
     socklen_t clilen;
     char readbuffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n,k;
     int number,index=0;
     //portno = 9799;
    
     
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     //portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portarray[1]);
     printf("Port no. is:%d\n",portno);
     /*if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1 )
     {
        printf("setsockopt");
     }*/
      
      int yes=1;
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes));
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
          printf("\nBefore listen" );
     int lis = listen(sockfd,5);
     printf("\nLis:%d",lis);
     printf("\nAfter listen");
     printf("listen socket is:%d\n",sockfd);
     while(1)
    {
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
    
     if (newsockfd < 0) 
          error("ERROR on accept");
        bzero(buffer,256);
        n = read(newsockfd,buffer,255);
        //msgvector.push_back(buffer);
        //check = strstr(buffer,"\n");
        if(strstr(buffer,"\n"))
        {

        //printf("Node Received from sequencer:%s\n",buffer);
        data = strtok(buffer,"\t");
        //printf("Tokened msg is:%s\n",data);
        dataport = strtok(NULL,"\n");
        //printf("Tokened port is:%s\n",dataport);
        printf("-------------------------------\n");
                    if(mindex+1==atoi(data))
                    {
                        printf("Received GS %s and %s msg\n",data,dataport);

                        printf("GS and mindex matched\n");
                        mindex++;   
                    }
        }
        else{
        //printf("Node Received from another node:%s\n",buffer);
        msgbuffer[index]=atoi(buffer);
        printf("msgbuffer is:%d\n",msgbuffer[index]);
        }
       /* if(strcmp(buffer,"ready")==0)
        {
            n = read(newsockfd,buffer,255);
            printf("Received from sequencer:%d\n",portarray[k]);
            printf("inside read");
            printf("Here is the message:%s\n",buffer);
            if (n < 0) 
            printf("ERROR reading from socket");
        }
        else*/
        
            // string message(buffer);
       //sleep(5); 
     close(newsockfd);
     
    }
    close(sockfd); 
    //return 0; 
}


void *threadprocess(void *args)
{
    
    printf("\nInside client thread");   
    int sockfd,n,k;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    cout << "Press any key when all servers are up" << endl;
    cin.ignore();
   // sleep(5);
    while(1)
    {
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
            bcopy((char *)server->h_addr, 
            (char *)&serv_addr.sin_addr.s_addr,
            server->h_length);
            // printf("Port array of sender is:%d\n",portarray[k]);
            serv_addr.sin_port = htons(portarray[k]);
            //sleep(2);
            if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
            error("ERROR connecting");
            srand(time(NULL));
            ts = rand()%100;
            bzero(buffer,256);
            sprintf(buffer,"%d",ts);
            //printf("Normal node is sending:%s\n",buffer);
            printf("Node is sending message:%s\n",buffer);
            //printf("Port array of sender is:%d\n",portarray[k]);
             // sleep(2);
            n = write(sockfd,buffer,strlen(buffer));
            sleep(2);
            if (n < 0) 
             error("ERROR writing to socket");
            //sleep(5);
        close(sockfd);
        }
        //ts = ts + 1;
    }
    
    //return 0;
}
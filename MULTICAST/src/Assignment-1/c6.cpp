#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <time.h>


int main(int argc, char *argv[])
{
    int sockfd, portno, n,synctime=0,servertime;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];

    //Client arguments check if not error
    if (argc < 2) {
       printf("Error: Less no. of arguments");
       exit(1);
    }
    //Create a socket and check for error during creation
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        printf("Error:Creating socket at client");

    //Get 2nd argument i.e server to connect with
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        printf("Error:No such host\n");
        exit(1);
    }
    //Zeroying out the buffer
    bzero((char *) &serv_addr, sizeof(serv_addr));

    //Host byte to network byte
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    //Get the port no from command line
    portno = atoi(argv[2]);
    serv_addr.sin_port = htons(portno);

    //Connect to server
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        printf("Error: in connecting to server");

    /*Distributed Node sending Hi to time daemon*/
    n = write(sockfd,"Hi",10);
    if (n < 0) 
    printf("Error: in writing at client end");  

    /*Read the timestamp of the time daemon*/
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
    printf("Error:in reading at client end");
    else
    {
        servertime = atoi(buffer);
        printf("Time daemon's clock time is:%d\n",atoi(buffer));

    }

    //Send the node's timestamp difference with respect to received time daemon's clock time
    bzero(buffer,256);
    srand(time(NULL));
    int clocktime = rand()%10;
    sprintf(buffer,"%d",clocktime-servertime);
    printf("--------------------------------------------\n");
    printf("Node's logical timestamp is:%d\n",clocktime);
    printf("Node sent time difference of %d to the time daemon\n",clocktime-servertime);
    printf("--------------------------------------------\n");
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
        printf("Error: in writing at client end");  

    //Read the adjustments sent from client
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
        printf("Error:in reading at client end");
    
    else
    {
        int synctime = atoi(buffer);
        printf("Time daemon sent sync time :%d\n",(synctime));
        printf("Node's clocktime is:%d\n",clocktime);
        printf("--------------------------------------------\n");
        clocktime = clocktime+synctime;
        printf("Process synchronized its clock to:%d\n",clocktime);
        printf("--------------------------------------------\n");
    }


    //Sending total no. of requests to server and receive response
       close(sockfd);
    return 0;
}


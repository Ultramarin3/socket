/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <poll.h>
#include "func.h"

#define PORTCLIENTA "25515"  // the port client A will be connecting to
#define PORTCLIENTB "26515"  // the port client B will be connecting to
#define PORTSERVERA "21515"  // the port of to server A
#define PORTSERVERB "22515"  // the port of to server B
#define PORTSERVERC "23515"  // the port of to server C

#define PORTUDP "24515"  // the port used to send from UDP
#define HOSTNAMEA "127.0.0.1" //the host name of serverA
#define HOSTNAMEB "127.0.0.1" //the host name of serverB
#define HOSTNAMEC "127.0.0.1" //the host name of serverC
#define HOSTNAMEM "127.0.0.1" //the host name of serverM



#define MAXBUFLEN 100
#define BACKLOG 10	 // how many pending connections queue will hold

//send message to backend server and get reply
char* getInfoSever(char* hostname, char *port, char *message){
    int sockfd;
    int numbytes;
    struct sockaddr_storage their_addr;
    char *buf = (char*)malloc(100*sizeof(char));
    struct sockaddr_in  dst_addr, my_addr;
    socklen_t len = sizeof(my_addr);
    socklen_t addr_len;
    int portno;
    struct hostent *dst_server;
    
    sockfd = init_socket(hostname,PORTUDP, SOCK_DGRAM);

    dst_server = gethostbyname("localhost");
    int getsock_check=getsockname(sockfd,(struct sockaddr*)&my_addr, &len);
    //Error checking
    if (getsock_check== -1) {
        perror("getsockname");
        exit(1);
    }
    printf("Portnumber used to send to server A= %d\n", ntohs(my_addr.sin_port));
    
    dst_server = gethostbyname("localhost");
    portno = atoi(port);
    bzero((char *) &dst_addr, sizeof(dst_addr));
    dst_addr.sin_family = AF_INET;
    bcopy((char *)dst_server->h_addr,
         (char *)&dst_addr.sin_addr.s_addr,
          dst_server->h_length);
    dst_addr.sin_port = htons(portno);
    printf("port number will be connecting to on server A = %d\n", ntohs(dst_addr.sin_port));
    if ((numbytes = sendto(sockfd, message, strlen(message), 0,(struct sockaddr *) &dst_addr,sizeof(dst_addr))) == -1) {
        perror("talker: sendto");
        exit(1);
    }

    printf("serverM sent %d bytes\n", numbytes);
    printf("send message %s\n", message);
    //receive from A
    addr_len = sizeof their_addr;
    if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
                             (struct sockaddr *)&their_addr, &addr_len)) == -1) {
        perror("recvfrom");
        exit(1);
    }
    printf("serverM: receive packet is %d bytes long\n", numbytes);
    buf[numbytes] = '\0';
    printf("serverM: reveive packet contains \"%s\"\n", buf);
    close(sockfd);
    return buf;
}
//check users validation and balance
char* checkUserInfo(char *token)
{
    char *result;
    char message[600];
    strcpy(message, "1");
    strcat(message, token);
    char *replyA = getInfoSever(HOSTNAMEA,PORTSERVERA, message);
    char *replyB = getInfoSever(HOSTNAMEB,PORTSERVERB, message);
    char *replyC = getInfoSever(HOSTNAMEC,PORTSERVERC, message);
    
    int sum = 0;
    int validUser = 0;
    if(strcmp(replyA, "invalidUser")){
    sum += atoi(replyA);
    validUser = 1;
    }
    if(strcmp(replyB, "invalidUser")){
    sum += atoi(replyB);
    validUser = 1;
    }
    if(strcmp(replyC, "invalidUser")){
        sum += atoi(replyC);
        validUser = 1;
    }

    printf("sum %d", sum);
    
    if(validUser){
        int numberCount = 0;
        int temp = abs(sum);
        while(temp != 0){
            temp /= 10;
            ++numberCount;
        }
        result = (char*)malloc(numberCount);
        sprintf(result, "%d", sum);
    }
    else{
        result = (char*)malloc(10);
        strcpy(result,"invalid");
    }
    return result;         

}
int checkSerNum(){
    int res = 0;
    int serA = atoi(getInfoSever(HOSTNAMEA,PORTSERVERA, "2"));
    int serB = atoi(getInfoSever(HOSTNAMEB,PORTSERVERB, "2"));
    int serC = atoi(getInfoSever(HOSTNAMEC,PORTSERVERC, "2")); 
    serA = serA > serB ? serA : serB;
    serA = serA > serC ? serA : serC;
    return ++serA; 
}




void add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size)
{
    // If we don't have room, add more space in the pfds array
    if (*fd_count == *fd_size) {
        *fd_size *= 2; // Double it
        *pfds = realloc(*pfds, sizeof(**pfds) * (*fd_size));
    }
    (*pfds)[*fd_count].fd = newfd;
    (*pfds)[*fd_count].events = POLLIN; // Check ready-to-read
    (*fd_count)++;
}
// Remove an index from the set

void del_from_pfds(struct pollfd pfds[], int i, int *fd_count)
{
    // Copy the one from the end over this one
    pfds[i] = pfds[*fd_count-1];
    (*fd_count)--;
}

int main(void)
{
	int sockfdA, sockfdB, new_fd;  // listen on sock_fd, new connection on new_fd
	struct sockaddr_storage their_addr; // connector's address information
    struct sockaddr_in my_addr;
    socklen_t len = sizeof(my_addr);
	socklen_t sin_size;

    int fd_count = 0, fd_size = 5;
    struct pollfd *pfds = malloc(sizeof *pfds * fd_size);
	char s[INET6_ADDRSTRLEN];
    int numbytes;
    char buf[MAXBUFLEN];
    
    sockfdA = init_socket(HOSTNAMEM,PORTCLIENTA, SOCK_STREAM);
    sockfdB = init_socket(HOSTNAMEM,PORTCLIENTB, SOCK_STREAM);

	printf("The main server is up and running\n");
    
    pfds[0].fd = sockfdA;
    pfds[0].events = POLLIN;
    pfds[1].fd = sockfdB;
    pfds[1].events = POLLIN;
    fd_count = 2;

	while(1) {  // main accept() loop
        int poll_count = poll(pfds, fd_count, -1);
        if (poll_count == -1) {
            perror("poll");
            continue;
        }
        for(int i = 0; i < fd_count; i++) {
            if (pfds[i].revents & POLLIN) {
                sin_size = sizeof their_addr;
                new_fd = accept(pfds[i].fd, (struct sockaddr *)&their_addr, &sin_size);
                if (new_fd == -1) {
                    perror("accept");
                    continue;
                } else{
                    add_to_pfds(&pfds, new_fd, &fd_count, &fd_size);
                }
                int getsock_check=getsockname(pfds[i].fd,(struct sockaddr*)&my_addr, &len);
                //Error checking
                if (getsock_check== -1) {
                    perror("getsockname");
                    exit(1);
                }
                printf("portnumber use to receive client = %d\n", ntohs(my_addr.sin_port));
                
                
                inet_ntop(their_addr.ss_family,
                    get_in_addr((struct sockaddr *)&their_addr),
                    s, sizeof s);
                printf("server: got connection from %s\n", s);

                if (!fork()) { // this is the child process
                    //close(pfds[i].fd); // child doesn't need the listener

                    if ((numbytes = recv(new_fd, buf, MAXBUFLEN-1, 0)) == -1) {
                        perror("recv");
                        exit(1);
                    }
                 

                    buf[numbytes] = '\0';
                    //struct pollfd *pfds = malloc(sizeof *pfds * fd_size);
                    // char *reply;
                    int operationtype = buf[0] - '0';
                    printf("operation type %d\n", operationtype);
                    char *operation = buf;
                    operation+=1;
                    
                    char *tokens[3];
                    
                    char *token = strtok_r(operation, " ", &operation);
                    int i = 0;
                    while(token != NULL){
                        tokens[i++] = token;
                        token = strtok_r(operation, " ", &operation);
                    }

                    for(int k = 0; k < i; k++){
                        printf("token %d = %s\n", k, tokens[k]);
                    }

                        
                    char reply[1500];
                    memset(reply, 0, 1500);
                    switch (operationtype)
                    {
                    case 1:{
                        /* check wallet */
                        //char *s[] = malloc(sizeof)
                        printf("operation 1\n");
                        strcpy(reply, checkUserInfo(tokens[0]));
               
                        break;
                    }
                        
                    case 2:{
                        /* transfer */
                        printf("operation 2\n");
                        
                        char *firstUser = checkUserInfo(tokens[0]);
                        int transamount = atoi(tokens[2]);
                        int remain = 0;
                        if(strcmp(firstUser, "invalid") != 0){
                            int sum = atoi(firstUser);
                            if( transamount > sum){
                                strcpy(reply,"no money");
                                break;
                            }
                            remain = sum - transamount;
                        }
                        else{
                            strcpy(reply,"no user1");
                            break;
                        }
                        //check first username in backend server 
                        char *secondUser = checkUserInfo(tokens[1]);
                        if(strcmp(secondUser, "invalid") == 0){
                            strcpy(reply,"no user2");
                            break;
                        }
                        //record transfer
                        int serNum = checkSerNum();
                        printf("sernum = %d\n", serNum);
                        char message[1500];
                        sprintf(message,"%s%d %s %s %s","3", serNum, tokens[0], tokens[1], tokens[2]);
                        int random = (atoi(tokens[2])*atoi(tokens[2])) %3;
                        printf("random = %d\n", random);
                        if(random == 0){
                            strcpy(reply, getInfoSever(HOSTNAMEA,PORTSERVERA, message)); 
                        }
                        else if(random == 1){
                            strcpy(reply, getInfoSever(HOSTNAMEA,PORTSERVERB, message));
                        }
                        else{
                            strcpy(reply, getInfoSever(HOSTNAMEA,PORTSERVERC, message));
                        }
                        break;
                    }
                        
                    case 3:{
                        /* TXLISTt */

                        char* messageA = getInfoSever(HOSTNAMEA,PORTSERVERA, "4");
                        char* messageB = getInfoSever(HOSTNAMEA,PORTSERVERB, "4");
                        char* messageC = getInfoSever(HOSTNAMEA,PORTSERVERC, "4");
                        
                        sprintf(reply, "%s", "TLIST");
                        printf("operation 3\n");
                        break;

                    }
                    case 4:{
                        /* stats */
                        printf("operation 1\n");
                        break;
                    }
                        
                    default:{
                        printf("unknown operation\n");
                        break;
                    }
                    }
                    printf("reply to client: %s", reply);

                    if (send(new_fd, reply, strlen(reply), 0) == -1)
                        perror("send");
                    //close(new_fd);
                    del_from_pfds(pfds, i, &fd_count);
                    exit(0);       //child exit
                }
                close(new_fd);  // parent doesn't need this
                
            }
        }
        
        
        


	}

	return 0;
}


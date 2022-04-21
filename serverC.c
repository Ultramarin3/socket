/*
** listener.c -- a datagram sockets "server" demo
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "func.h"

#define MYPORT "23515"	// the port users will be connecting to
#define MYHOST "127.0.0.1" //the host name of serverC
#define MAXBUFLEN 100

int main(void)
{
	int sockfd;
	int numbytes;
	struct sockaddr_storage their_addr;
	char buf[MAXBUFLEN];
	socklen_t addr_len;
	char s[INET6_ADDRSTRLEN];
    sockfd = init_socket(MYHOST, MYPORT, SOCK_DGRAM);
	printf("The ServerC is up and running using UDP on port 23515\n");
    while(1){
        addr_len = sizeof their_addr;
        if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
                                 (struct sockaddr *)&their_addr, &addr_len)) == -1) {
            perror("recvfrom");
            exit(1);
        }

        printf("serverC: got packet from %s\n",
               inet_ntop(their_addr.ss_family,
                         get_in_addr((struct sockaddr *)&their_addr),
                         s, sizeof s));
        printf("serverC: packet is %d bytes long\n", numbytes);
        buf[numbytes] = '\0';
        printf("serverC: packet contains \"%s\"\n", buf);
        int operationtype = buf[0] - '0';
        printf("operation type %d\n", operationtype);
        char *user = buf + 1;

        AllRecords *allRecords;
        allRecords = malloc(sizeof(AllRecords));
        allRecords->numline = 0;
        allRecords->record = NULL;
        getBlocktxt(allRecords, "./block3.txt");
        char reply[100];
        if(operationtype == 1){
            if(checkUser(allRecords, user)){
                int value = checkWallet(allRecords, user);
                sprintf(reply,"%d", value);
            }
            else strcpy(reply,"invalidUser");
        }
        else if(operationtype == 2){
            int serNum = checkSerial(allRecords);
            sprintf(reply, "%d", serNum);
        }
        else if(operationtype == 3){
            printf("%s\n", user);
            if(addLog("./block3.txt", user)){
                sprintf(reply, "%s", "success");
            }
            else{
                sprintf(reply, "%s", "fail");
            }
        }
        else if (operationtype == 4){
            for(int i = 0; i < allRecords->numline; i++){
                printf("%d %s %s %d\n", allRecords->record[i].num, allRecords->record[i].sender, 
                                        allRecords->record[i].receiver, allRecords->record[i].amount);
            }
            sortRecords(allRecords, 0, allRecords->numline - 1);

            for(int i = 0; i < allRecords->numline; i++){
                printf("%d %s %s %d\n", allRecords->record[i].num, allRecords->record[i].sender, 
                                        allRecords->record[i].receiver, allRecords->record[i].amount);
            }
            sprintf(reply, "%s", "TLIST");

        } 
        
        printf("reply = %s", reply);

        if ((numbytes = sendto(sockfd, reply, strlen(reply), 0,
                               (const struct sockaddr *)&their_addr, (socklen_t)addr_len)) == -1) {
            perror("serverC: sendto");
            exit(1);
        }
        printf("serverC sent %d bytes\n", numbytes);
    }

    close(sockfd);
	return 0;
}

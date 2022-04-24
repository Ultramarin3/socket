/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "25515" // the port clientA will be connecting to

#define MAXDATASIZE 1024*2 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
    struct sockaddr_in my_addr;
    socklen_t len = sizeof(my_addr);
    


	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo("127.0.0.1", PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
    
    

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("client: connect");
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}
    int getsock_check=getsockname(sockfd,(struct sockaddr*)&my_addr, &len);
    //Error checking
    if (getsock_check== -1) {
        perror("getsockname");
        exit(1);
    }
    //printf("getPortnumber = %d\n", ntohs(my_addr.sin_port));

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	//printf("client: connecting to %s\n", s);
	printf("The clientA is up and running.\n");

	freeaddrinfo(servinfo); // all done with this structure
	char c[MAXDATASIZE];
	if(argc == 2){
		if(!strcmp(argv[1], "TXLIST")){
			strcpy(c,"3"); //operation 3: TXLIST
			char* temp = argv[0];
			temp +=2;
			printf("“%s” sent a sorted list request to the main server.\n", temp);
		}
		else {
			strcpy(c,"1"); //operation 1: check wallet
			printf("“%s” sent a balance enquiry request to the main server.\n", argv[1]);

		}
	}
	else if(argc == 3) strcpy(c,"4"); //operation 4: stats
	else if(argc == 4) {
		strcpy(c,"2"); //operation 2: transfer
		printf("“%s” has requested to transfer %s coins to “%s”.\n", argv[1], argv[3], argv[2]);

	}
	else strcpy(c,"0");    //unknow operation
    for(int i = 1; i< argc;i++){
        strcat(c, argv[i]);
        strcat(c, " ");
    }
	//printf("send message: %s\n", c);	

    if ((numbytes = send(sockfd, c, strlen(c), 0)) == -1) {
        perror("send");
        exit(1);
    }
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	    perror("recv");
	    exit(1);
	}

	buf[numbytes] = '\0';
	if(argc == 2){
		if(strcmp(argv[1], "TXLIST")){
			if(strcmp(buf, "invalid")){
				printf("The current balance of “%s“ is %s.\n", argv[1], buf);
			}
			else{
				printf("”%s“ is not part of the network.\n", argv[1]);
			}

		}
	}
	else if(argc == 4){
		if(buf[0] == 's'){
			char *balance = buf;
			printf("”%s“ successfully transferred %s alicoins to ”%s“.\n", argv[1], argv[3], argv[2]);
			printf("The current balance of ”%s“ is: %s.\n", argv[1], ++balance);
		}
		else{
			if(buf[0] == 'n'){
				char *balance = buf;
				printf("”%s“ was unable to transfer %s alicoins to ”%s“ because of insufficient balance.\n", argv[1], argv[3], argv[2]);
				printf("The current balance of ”%s“ is : %s alicoins.\n", argv[1], ++balance);
			}
			else if(buf[0] == '1'){
				printf("Unable to proceed with the transaction as ”%s“ is not part of the network.\n", argv[1]);
			}
			else if(buf[0] == '2'){
				printf("Unable to proceed with the transaction as ”%s“ is not part of the network.\n", argv[2]);
			}
		}
	}

	

	close(sockfd);

	return 0;
}


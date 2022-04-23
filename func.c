/*
** server.c -- a stream socket server demo
*/


#include "func.h"



#define MAXBUFLEN 100
#define BACKLOG 10	 // how many pending connections queue will hold




void sigchld_handler(int s)
{
	(void)s; // quiet unused variable warning

	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}



// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//
int init_socket(const char *host,const char *port, int socketType){
    int sockfd;
    int rv;
    struct sigaction sa;
    int yes=1;
    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = socketType;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(host, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    else{
        
    }
    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (socketType == SOCK_STREAM && listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    return sockfd;
}
void sortRecords(AllRecords *allRecords, int l, int r){
    if(l >= r) return;
    int i = l - 1, j = r + 1, x = allRecords->record[l + (r - l)/2].num;
    while(i < j){
        while(allRecords->record[++i].num < x);
        while(allRecords->record[--j].num > x);
        if(i<j){
            //swap 
            Records temp;
            temp.num = allRecords->record[i].num;
            temp.amount = allRecords->record[i].amount;
            strcpy(temp.sender, allRecords->record[i].sender);
            strcpy(temp.receiver, allRecords->record[i].receiver);

            allRecords->record[i].num = allRecords->record[j].num;
            allRecords->record[i].amount = allRecords->record[j].amount;
            strcpy(allRecords->record[i].sender, allRecords->record[j].sender);
            strcpy(allRecords->record[i].receiver, allRecords->record[j].receiver);

            allRecords->record[j].num = temp.num;
            allRecords->record[j].amount = temp.amount;
            strcpy(allRecords->record[j].sender, temp.sender);
            strcpy(allRecords->record[j].receiver, temp.receiver);
        }

    }
    sortRecords(allRecords, l, j), sortRecords(allRecords, j + 1, r);

}



void getBlocktxt(AllRecords *allRecords, char *file){

    int initSize = 3;
    allRecords->record = malloc(initSize *sizeof(Records));
    FILE *fp = fopen(file, "r");
    if( fp == NULL){
        printf("%s\n", "Reading file Error");
        return;
    }
    int count = 0;
    while(!feof(fp)){
        fscanf(fp, "%d %s %s %d", &allRecords->record[count].num, 
                    allRecords->record[count].sender,
                    allRecords->record[count].receiver, 
                    &allRecords->record[count].amount);
        count++;
        allRecords->numline++;
        if(count == initSize){
            initSize *= 2;
            allRecords->record = realloc(allRecords->record,initSize *sizeof(Records));
        }
    }
    fclose(fp);

}

void getBlockmessage(AllRecords *allRecords, char *message){

    int initSize = 3;
    allRecords->record = malloc(initSize *sizeof(Records));
    int count = allRecords->numline;

    while(strlen(message) != 0){
        char log[1500];
        memset(log, 0, 1500);
        int i = 0;
        while(message[i++] != '\n');
        message[i-1] = '\0';
        strcpy(log, message);
        message+=i;

        sscanf(log, "%d %s %s %d", &allRecords->record[count].num, 
                    allRecords->record[count].sender,
                    allRecords->record[count].receiver, 
                    &allRecords->record[count].amount);
        count++;
        allRecords->numline++;
        if(count == initSize){
            initSize *= 2;
            allRecords->record = realloc(allRecords->record,initSize *sizeof(Records));
        }
    }
}

int checkWallet(AllRecords *allRecords, char *user){
    int res = 0;
    for(int i = 0; i< allRecords->numline;i++){
        if(!strcmp(allRecords->record[i].sender, user)){
            res-=allRecords->record[i].amount;
            
        }
        if(!strcmp(allRecords->record[i].receiver, user)){
            res+=allRecords->record[i].amount;
        }
    }
    return res;
}

int checkUser(AllRecords *allRecords, char *user){
    int exist = 0;
    for(int i = 0; i< allRecords->numline;i++){
        if(!strcmp(allRecords->record[i].sender, user)){
            exist = 1;
            
        }
        if(!strcmp(allRecords->record[i].receiver, user)){
            exist = 1;
        }
    }
    return exist;
}

int checkSerial(AllRecords *allRecords){
    int ser = 0;
    for(int i = 0; i< allRecords->numline;i++){
        ser = ser > allRecords->record[i].num ? ser : allRecords->record[i].num;
    }
    //printf("max ser %d", ser);
    return ser;
}
int addLog(char *file, char *transLog){
    //printf("add %s", transLog);

    FILE *fp = fopen(file, "a");
    if(fp == NULL){
        return 0;
    }
    //printf("add success\n");
    fprintf(fp, "%s\n",transLog);
    fclose(fp);
    return 1;
}


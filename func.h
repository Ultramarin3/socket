#ifndef __FUNC_H__
#define __FUNC_H__
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
typedef struct {
    int num;
    char sender[512];
    char receiver[512];
    int amount;
}Records;
typedef struct {
    Records *record;
    int numline;
}AllRecords;

void getBlocktxt( AllRecords *, char *);
int checkWallet( AllRecords *, char *);
int checkUser( AllRecords *, char *);
void sigchld_handler(int);
void *get_in_addr(struct sockaddr *);
int init_socket(const char *,const char *, int );
int checkSerial(AllRecords *);
int addLog(char *, char *);
#endif

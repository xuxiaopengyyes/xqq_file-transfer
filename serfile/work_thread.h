#ifndef WORK_PTHREAD_H
#define WORK_PTHREAD_H

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<fcntl.h>

//给一个套接字去新线程执行
void start_thread(int c);


#endif

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fcntl.h>


int connect_ser();
int main()
{
    int c=connect_ser();
    if(c==-1)
    {
        exit(0);
    }

    while(1)
    {
        printf("connect~ >> ");
        fflush(stdout);
        

        char buff[128]={0};
        fgets(buff,128,stdin);

        buff[strlen(buff)-1]=0;
        
        //测试
        if(strcmp(buff,"end")==0)
        {
            break;
        }
        send(c,buff,strlen(buff),0);
        memset(buff,0,128);
        recv(c,buff,127,0);
        printf("buff=%s\n",buff);
    }
    close(c);
    exit(0);



}

int connect_ser()
{
    int sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd==-1)
    {
        printf("socket err\n");
        return -1;
    }

    struct sockaddr_in saddr;
    memset(&saddr,0,sizeof(saddr));
    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(6000);
    saddr.sin_addr.s_addr=inet_addr("127.0.0.1");

    int res =connect(sockfd,(struct sockaddr *)&saddr,sizeof(saddr));
    if(res==-1)
    {
        printf("connect ser failed\n");
        return -1;
    }

    return sockfd;

}


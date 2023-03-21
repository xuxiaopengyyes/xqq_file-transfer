#include"socket.h"

#define IPSTR "ipstr"
#define PORT "port"
#define LISMAX "lismax"

struct sock_info
{
	char ips[32];
	short port;
	short lismax;
};

//读取配置信息
int read_conf(struct sock_info * dt)
{
    if(dt==NULL)
    {
        printf("sock_info NULL\n");
        return -1;
    }
	FILE *fp=fopen("my.conf","r");
	if(fp==NULL)
	{
		printf("open myconf failed\n");
		return -1;
	}

    int index=0;

	char buff[128]={0};
    while(fgets(buff,127,fp)!=NULL)
    {
        index++;
        if(strncmp(buff,"#",1)==0||strncmp(buff,"\n",1)==0)
        {
            continue;
        }

        buff[strlen(buff)-1]='\0';//去除尾部\n

        if(strncmp(buff,IPSTR,5)==0)
        {
            strcpy(dt->ips,buff+strlen(IPSTR)+1);//加一因为IPSTR后面还有一个‘=’
        }
        else if(strncmp(buff,PORT,4)==0)
        {
            dt->port=atoi(buff+strlen(PORT)+1);
        }
        else if(strncmp(buff,LISMAX,6)==0)
        {
            dt->lismax=atoi(buff+strlen(LISMAX)+1);
        }
        else
        {
            printf("无法解析第%d行的数据%s",index,buff);
        }



    }

    fclose(fp);
    return 1;
}

int socket_init()
{
    //读取配置文件
	struct sock_info sock;
	if(read_conf(&sock)==-1)
    {
        printf("read_conf falied\n");
        return -1;
    }

    int sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd==-1)
    {
        return -1;
    }
    struct sockaddr_in saddr;
    memset(&saddr,0,sizeof(saddr));
    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(sock.port);
    saddr.sin_addr.s_addr=inet_addr(sock.ips);

    int res=bind(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
    if(res==-1)
    {

        return -1;
    }

    if(listen(sockfd,sock.lismax)==-1)
    {
        return -1;
    }
    
    return sockfd;
}

#include"socket.h"

#define IPS "ipstr"
#define PORT "port"
#define LMAX "lismax"

struct sock_info
{
	char ips[32];
	short port;
	short lismax;
};

int read_conf(struct sock_info * dt)
{
    if(dt==NULL)
    {
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
    while(fgets(buff,128,fp)!=NULL)
    {
        index++;
        if(strncmp(buff,"#",1)==0||strncmp(buff,"\n",1)==0)
        {
            continue;
        }

        if(strncmp(buff,IPS,5)==0)
        {
            strcpy(dt->ips,buff+strlen(IPS)+1);
        }
        else if(strncmp(buff,PORT,4)==0)
        {
            dt->port=atoi(buff+strlen(PORT)+1);
        }
        else if(strncmp(buff,LMAX,6)==0)
        {
            dt->lismax=atoi(buff+strlen(LMAX)+1);
        }
        else
        {
            printf("无法解析第%d行的数据%s",index,buff);
        }



    }

    return 1;
}

int socket_init()
{
	struct sock_info sock;
	if(read_conf(&sock)==-1)
    {
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








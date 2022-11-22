#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int read_conf(sockaddr_in *s)
{
	if(s==NULL)
	{
		return -1;
	}
	FILE *fp=fopen("my.conf","r");
	if(fp==-1)
	{
		return -1;
	}
	char buff[128]={0};
	while(fgets(buff,127,fp)!=EOF)
	{
		if(buff[0]=='#')
		{
			continue;
		}
		else if(buff[0]=='\n')
		{
			continue;
		}
		else
		{
		
		}

	}

	fclose(fp);
}
int sockfd_init()
{
	int sockfd;
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(socked==-1)
	{
		return -1;
	}
	struct sockaddr_in saddr;
	memset(&saddr,0,sizeof(saddr));

	int r=read_conf(&saddr);
	if(r==-1)
	{
		return -1;
	}
}
int main()
{
	


}

#include"socket.h"


struct socket_info
{
	char ips[32];
	short port;
	short lismax;
};
void read_conf(const socket_info *dt)
{
	FILE *fp=fopen("my.conf","r");
	if(fp==NULL)
	{
		printf("open myconf failed\n");
		return -1;
	}

	char buff[128];


}

int socket_init()
{
	struct sock_info sock;
	read_conf(&sock);

}

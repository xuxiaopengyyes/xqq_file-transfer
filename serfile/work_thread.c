#include"work_thread.h"

void * work_fun(void *arg)
{
	int c=(int) arg;
	while(1)
	{
		char buff[128}={0};
		int num =recv(c,buff,127,0);
		if(num<0)
		{
			break;
		}

		//测试
		printf("%s",buff);
		send("ok",2);	
	
		//测试
	}
}

void start_thread(int c)
{
	pthread_t id;
	if(pthread_create(&id,NULL,work_fun,(void *)c) !=0)
	{
		close(c);
		printf("启动线程失败\n");
		return ;
	}
}

#include"work_thread.h"
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<openssl/md5.h>
#include<mysql/mysql.h>

#define MD5_LEN 16
#define BUFF_SIZE 1024*16

#define ARGC 10

#define ERR "err"
#define PIPE_ERR "err pipe"
#define FORK_ERR "err "
#define NO_FILENAME ""

char* get_cmd(char buff[],char* myargv[])
{
    if(buff==NULL||myargv==NULL)
    {
        return NULL;
    }

    int i=0;
    char* ptr = NULL;
    char* s = strtok_r(buff," ",&ptr);
    while(s != NULL)
    {
        myargv[i++] = s;
        s = strtok_r(NULL," ",&ptr);
    }
    return myargv[0];
}

void recv_file(int c, char* myargv[])
{
    //                   filename             filesize             md5
    if(myargv == NULL || myargv[1] == NULL || myargv[2] == NULL || myargv[3] == NULL)
    {
        send(c, NO_FILENAME, strlen(NOFILENAME), 0);
        return;
    }
    
    printf("%s\n",myargv[3]);

    //秒传，判断文件是否存在
    char srcfilename[128] = {0};
    if(isExist(myargv[3],srcfilename))
    {
        printf("文件传输成功!\n");
        
        //执行文件链接指令 创建链接文件
        if(strncmp(srcfilename, myargv[1], strlen(myargv[1])) != 0)
        {
            pid_t = fork();
            if(pid == -1)
            {
                exit(1);
            }   

            char cmd[256] = {0};
            snprintf(cmd, 256,  "Ln %s %s", srcfilename, myargv[1]);
            char* tmpargv[10] = {0};
            char* ptr = NULL;
            tmpargv[0] = strtok_r(cmd, " ", &ptr);
            int i = 0；
            while(tmpargv[0] != 0)
            {
                tmpargv[++i] = strtok_r(NULL, "", &ptr);
            }
            if(pid == 0)
            {
                execvp(tmpargv[0],tmpargv);
                exit(1);
            }
            
            send(c, FILE_EXIST, strlen(FILE_EXIST), 0);

            return;
        }

        //为文件名拼接  --断点续传
        char filename[128] = {0};
        strcpy(filename,myargv[1]);
        strcat(filename,UNDONE);

        //打开（创建目标文件），获取文件偏移


    }



}

void* work_fun(void *arg)
{
	int c=(int) arg;
	while(1)
	{
        //解析报文
		char buff[128]={0};

		int num =recv(c,buff,127,0);
		if(num<=0)
		{
            printf("close: %d\n",c);
			break;
		}
        if(strncmp("exit",buff,4)==0)
        {
            break;
        }

		//测试
		printf("%s",buff);
	    
        char* myargv[ARGC] = {0};
        char *cmd = get_cmd(buff,myargv);
        if(cmd == NULL)
        {
            send(c,ERR,strlen(ERR),0);
            continue;
        }

        if(strncmp(cmd, "get", 3) == 0)
        {
            //下载
            send_file(c,myargv);
        }
        else if(strncmp(cmd, "up", 2) == 0)
        {
            //上传
            recv_file(c,myargv);
        }
        else
        {
            //其他命令
            run_cmd(c,cmd,myargv);
        }
	}
    close(c);
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

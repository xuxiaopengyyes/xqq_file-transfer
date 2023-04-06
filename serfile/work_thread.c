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
#define FORK_ERR "err fork"
#define NO_FILENAME "err nofilename"
#define FILE_OPEN_ERR "err file open"
#define UNDOWN ".tmp"
#define SQL_ERR "err sql"
#define FILE_EXIST "exist"


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

void run_cmd(int c, char* cmd, char* myargv[])
{
    if(cmd == NULL || myargv == NULL)
    {
        return ;
    }

    int pipefd[2];
    if(pipe(pipefd) == -1)
    {
        send(c, PIPE_ERR, strlen(PIPE_ERR), 0);
        return ;
    }
    pid_t pid = fork();
    if(pid == -1)
    {
        send(c, FORK_ERR, strlen(FORK_ERR), 0);
        return ;
    }
    if(pid == 0)
    {
        //关闭读端
        close(pipefd[0]);
        //将stdout,stderr重定向到管道写端
        dup2(pipefd[1],1);
        dup2(pipefd[1],2);
        execvp(cmd, myargv);
        printf("cmd:%s not found\n", cmd);
        exit(0);
    }
    close(pipe[1]);

    char red_pipe[4096] = {"ok#"};
    wait(NULL);
    read(pipefd[0], read_pipe + 3,4092);
    send(c, read_pipe, strlen(read_pipe), 0);
}

void fun_md5(int fd, unsigned char* md)
{
    MD5_CTX ctx;
    MD5_Init(&ctx);

    unsigned long len = 0;
    char buff[ BUFF_SIZE ];
    while( (len = read(fd, buff, BUFF_SIZE )) > 0)
    {
        MD5_Update(&ctx, buff, len);
    }

    MD5_Final(md,&ctx);
}

void md5_to_char(unsigend char md[], char* md5)
{
    for(int i = 0;i < MD5_LEN; i++)
    {
        sprintf(md5 + i * 2, "%02x", md[i]);
    }

}

void printf_md5(unsigned char md[])
{
    int i = 0;
    for(;i < MD5_LEN; i++)
    {
        printf("%02x",md[i]);
    }
    printf("\n");
}

MYSQL_RES* dosql(const char* sql)
{
    if(sql == NULL)
    {
        return NULL;
    }

    MYSQL mysql_con;
    MYSQL* mysql = mysql_init(&mysql_con);
    if(mysql == NULL)
    {
        printf("mysql init err\n");
        return NULL;
    }

    mysql = mysql_real_connect(mysql, "losthost", "root", "123456", "md5", 3306, NULL, 0);
    if(mysql == NULL)
    {
        printf("connect mysql faild\n");
        return NULL;
    }


    int query_res = mysql_query(mysql, sql);
    if(query_res != 0)
    {
        printf("query err: %s\n",mysql_error(mysql));
        mysql_close(mysql);
        return NULL;
    }

    MYSQL_RES* mysql_res = mysql_store_result(mysql);
    if(mysql_res == NULL)
    {
        mysql_close(mysql);
        return NULL;
    }

    mysql_close(mysql);
    return mysql_res;
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
        int fd = open(filename, O_WRONLY);
        int offset = 0;
        if (fd == -1)
        {
            fd = open(filename, O_WRONLY | O_CREAT, 0600);
            if (fd == -1)
            {
             send(c, "err", 3, 0);
            }
        }
        else
        {
            offset = lseek(fd, 0, SEEK_END);//获取文件大小
        }

        //发送偏移
        char send_status[128] = {"ok#"};
        sprintf(send_status + 3, "%d", offset);//发送段插入偏移
        send(c, send_status, strlen(send_status), 0);

        //文件大小
        long long filesize = atoll(myargv[2]);

        long long curr_file = offset;
        int num = 0;
        char data[1024] = {0};
        bool tag = true;
        printf("%lld\n", filesize);
        while (true)
        {
            num = recv(c, data, 1024, 0);
            if (num <= 0)
            {
                //printf("ser close\n");
                break;
            }
            write(fd, data, num);
            curr_file += num;

            if (curr_file >= filesize)
            {
                break;
            }
        }
        printf("%lld\n", curr_file);

        //校验：重新打开文件，获取md5值
        close(fd);
        fd = open(filename, O_RDONLY);
        if (fd == -1)
        {
            send(c, FILE_OPEN_ERR, strlen(FILE_OPEN_ERR), 0);
        }
        lseek(fd, 0, SEEK_SET);

        //获取md5值
        unsigned char md[MD5_LEN + 1] = {0};
        fun_md5(fd, md);
        print_md5(md);
        char md5[MD5_LEN * 2 + 1] = {0};
        md5_to_char(md, md5);
        printf("%s\n", md5);
        if (strcmp(md5, myargv[3]) != 0)
        {
            send(c, ERR, strlen(ERR), 0);
            return;
        }

        //校验完成，返回
        send(c, "ok#", strlen("ok#"), 0);

        //对文件名进行处理
        pid_t pid = fork();
        if (pid == -1)
        {
            exit(1);
        }

        char cmd[128] = {"mv "};
        strcat(cmd, filename);
        strcat(cmd, " ");
        strcat(cmd, myargv[1]);
        char* tmpargv[10] = {0};
        char* index = NULL;
        tmpargv[0] = strtok_r(cmd, " ", &index);
        int i = 0;
        while (tmpargv[i] != 0)
        {
            tmpargv[++i] = strtok_r(NULL, " ", &index);
        }
        if (pid == 0)
        {
            execvp(tmpargv[0], tmpargv);
            exit(1);
        }
        saveMD5_in_Sql(myargv[1], md5);
        close(fd);

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

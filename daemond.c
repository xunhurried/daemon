#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <time.h>

//查找进程中程序名字
#define NAME_FIND "Mask"
//输出目录
#define DIR_OUT_FILE "/home/ubuntu/damon/out.ini"
//日志目录
#define LOG_OUT_FILE "/home/ubuntu/damon/out.txt"
//要运行的程序
#define RUN_NAME "./home/ubuntu/damon/Mask &"

//获取时间
void gettime(char *cur_time) {
        char Year[6] = {0};
        char Month[4] = {0};
        char Day[4] = {0};
        char Hour[4] = {0};
        char Min[4] = {0};
        char Sec[4] = {0};

        time_t current_time;
        struct tm* now_time;
        time(&current_time);
        now_time = localtime(&current_time);

        strftime(Year, sizeof(Year), "%Y-", now_time);
        strftime(Month, sizeof(Month), "%m-", now_time);
        strftime(Day, sizeof(Day), "%d ", now_time);
        strftime(Hour, sizeof(Hour), "%H:", now_time);
        strftime(Min, sizeof(Min), "%M:", now_time);
        strftime(Sec, sizeof(Sec), "%S", now_time);

        strncat(cur_time, Year, 5);
        strncat(cur_time, Month, 3);
        strncat(cur_time, Day, 3);
        strncat(cur_time, Hour, 3);
        strncat(cur_time, Min, 3);
        strncat(cur_time, Sec, 3);
}

int daemon(int nochdir,int noclose)
{
	pid_t pid;
 
	//让init进程成为新产生进程的父进程
	pid = fork();
	//如果创建进程失败
	if (pid < 0)
	{
		perror("fork");
		return -1;
	}
	//父进程退出运行
	if (pid != 0)
	{
		exit(0);
	}
	//创建新的会话
	pid = setsid();
	if (pid < -1)
	{
		perror("set sid");
		return -1;
	}
	//更改当前工作目录,将工作目录修改成根目录
	if (!nochdir)
	{
		chdir("/");
	}
	//关闭文件描述符，并重定向标准输入，输出合错误输出
	//将标准输入输出重定向到空设备
	if (!noclose)
	{
		int fd;
		fd = open("/dev/null",O_RDWR,0);
		if (fd != -1)
		{
			dup2(fd,STDIN_FILENO);
			//dup2(fd,STDOUT_FILENO);
			dup2(fd,STDERR_FILENO);

			if (fd > 2)
			{
				close(fd);
			}
		}
	}
	//设置守护进程的文件权限创建掩码
	umask(0027);
 
	return 0;
}
 
//是否有匹配的字符，有则返回1，没有返回0
//src:源字符串
//dst：目标字符串
//len:源字符串被比较的长度
int match(char *src,char *dst,int len)
{
	int i = 0;
	int j = 0;
	int size_dst = 0;
 
	//获得目标字符串的长度
	size_dst = strlen(dst);
	//如果目标字符串的长度大于len，返回失败
	if (size_dst > len)
	{
		return 0;
	}	
	//开始比较
	for (i = 0;i < len;i++)
	{
		for (j = 0;j < size_dst;j++)
		{
			if (src[i + j] != dst[j])
			{
				break;
			}
		}
		if (j == size_dst)
		{
			return 1;
		}
	}
 
	return 0;
}
 
int main(int argc,char *argv[])
{
	int fd = 0;
	char buf[100];
	//开启守护进程
	daemon(0,0);
        FILE *fplog = NULL;
        fplog = fopen(LOG_OUT_FILE, "a+");

        time_t now;
	struct tm *t;
	
	while (1)
	{
		//查看程序是否运行
		//新建输出文件
		system("touch "DIR_OUT_FILE);
		//获得程序ID
		system("ps -e|grep "NAME_FIND" >> "DIR_OUT_FILE);
		//打开输出文件
		fd = open(DIR_OUT_FILE,O_CREAT|O_RDONLY,0777);
		//清空缓存
		memset(buf,0,100);
		//读取全部
		read(fd,buf,100);
		//判断是否有程序文件运行

		if (match(buf,NAME_FIND,90))
		{
			time(&now);
			t = localtime(&now);
			fprintf(fplog, "This  mask is running %d-%d-%d %d:%d:%d\n", t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
			fflush(fplog);
		}
		else
		{
			time(&now);
			t = localtime(&now);
			fprintf(fplog, "This  mask is filed %d-%d-%d %d:%d:%d\n", t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
			fflush(fplog);			
			// 启动程序
			system(RUN_NAME);
		}
		//休眠
		sleep(5);
		//删除输出文件
		system("rm "DIR_OUT_FILE);
		//休眠
		sleep(5);
		
	}
 
	fclose(fplog);
	return 0;
}
 

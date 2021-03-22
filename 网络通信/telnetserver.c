# 摘自https://love.junzimu.com/archives/1116

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/vfs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#define BUFFERSIZE 64			//缓冲区大小
char buffer[BUFFERSIZE];		//缓冲区

int sockfd, new_fd;				//socket连接

char login_name[] = "admin";	//登录用户名
char login_pwd[] = "123456";	//登录密码

char name[20];					//保存用户输入的用户名
char pwd[20];					//保存用户输入的密码

int usr_flag = 0;				//判断用户名标志位
int pwd_flag = 0;				//判断用户密码标志位

int flag = 0;					//判断是否为无效命令

char str_info[64];				//print_write函数的字符串

/*将printf转换为telnet的远程输出*/
int print_write(char str[])
{
	write(new_fd, str, strlen(str));
	memset(str, 0, sizeof(str));
	return 0;
}

/*将recv转为本地的scanf*/
int scanf_recv(char str[])
{
	recv(new_fd, buffer, BUFFERSIZE, 0);
	int i = 0;
	while (buffer[i] != '\r') {
		str[i] = buffer[i];
		i++;
	}
	str[i] = '\0';
	printf("%s\n", str);
	memset(buffer, 0, sizeof(buffer));
	return 0;
}

/*******************
 * 匹配用户的输入命令
 *******************/
int match_cmd(char cmd[])		//判断命令输入
{
	if (strcmp(cmd, "help") == 0) {
		flag = 1;
		sprintf(str_info, " 	帮助命令：                   #help\n");
		print_write(str_info);

		sprintf(str_info, "  	显示xxx系统CPU信息：         #cpuinfo\n");
		print_write(str_info);

		sprintf(str_info, "  	显示xxx系统内存使用状态信息：#meminfo\n");
		print_write(str_info);

		sprintf(str_info, "  	显示xxx系统磁盘使用状态信息：#diskinfo\n");
		print_write(str_info);

		sprintf(str_info, "        xxx*Plus退出：               #quit\n");
		print_write(str_info);
	}

	if (strcmp(cmd, "quit") == 0)	//客户端退出
	{
		flag = 1;
		sprintf(str_info, "确定要退出[yes]/no:\n");
		print_write(str_info);

		char quit_str[4];
		scanf_recv(quit_str);

		if (strcmp(quit_str, "yes") == 0) {
			shutdown(new_fd, 2);
			//close(new_fd); 
			exit(1);
		}
		if (strcmp(quit_str, "no") == 0) {
			sprintf(str_info, "\n");
			print_write(str_info);
		}
	}

	/********************
    本机CPU使用信息
    ********************/
	if (strcmp(cmd, "cpuinfo") == 0) {
		flag = 1;
		const char *path = "/proc/stat";
		FILE *fp;
		char buf[8192];
		char *s;
		memset(buf, 0x00, 8192);
		fp = fopen(path, "rw");
		if (fp == NULL) {
			printf("没有找到相关信息。\n");
			fclose(fp);
		}
		while (fgets(buf, 8192, fp)) {
			write(new_fd, buf, strlen(buf));
		}
	}

	/************************
    //本机内存使用信息
    ************************/
	if (strcmp(cmd, "meminfo") == 0) {
		flag = 1;
		const char *path = "/proc/meminfo";
		FILE *fp;
		char buf[8192];
		char *s;
		memset(buf, 0x00, 8192);
		fp = fopen(path, "rw");
		if (fp == NULL) {
			printf("没有找到相关信息。\n");
			fclose(fp);
		}
		while (fgets(buf, 8192, fp)) {
			write(new_fd, buf, strlen(buf));
		}
	}

	/*********************    
    //磁盘使用信息 
    ********************/
	if (strcmp(cmd, "diskinfo") == 0) {
		flag = 1;
		struct statfs sfs;
		int i = statfs("/home", &sfs);
		int percent = (sfs.f_blocks - sfs.f_bfree) * 100 / (sfs.f_blocks - sfs.f_bfree + sfs.f_bavail) + 1;
		sprintf(str_info, "/dev/sda11            %ld    %ld  %ld   %d%% /home\n", 4 * sfs.f_blocks, 4 * (sfs.f_blocks - sfs.f_bfree), 4 * sfs.f_bavail, percent);
		print_write(str_info);
		system("df /home ");
	}

	/**********************
      错误命令
    **********************/
	if (flag == 0) {
		sprintf(str_info, "没有找到命令，请输入help查询帮助。\n");
		print_write(str_info);
	}
	return 0;
}

int main(int argc, char *argv[])
{
	struct sockaddr_in server_addr;	//服务器网络地址结构
	struct sockaddr_in client_addr;	//客户端网络地址结构
	socklen_t sin_size;
	int portnumber;				//协议号

	if (argc != 2)				//main参数
	{
		fprintf(stderr, "Usage:%s portnumber\a\n", argv[0]);
		exit(1);
	}

	if ((portnumber = atoi(argv[1])) < 0) {
		fprintf(stderr, "Usage:%s portnumber\a\n", argv[0]);
		exit(1);
	}

	/*
	 * 以下为SOCKET过程
	 */

	/* 服务器端开始建立socket描述符 */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)	//建立SOCKET通信,-1为失败
	{
		fprintf(stderr, "Socket error:%s\n\a", strerror(errno));
		exit(1);
	}

	/* 服务器端填充 sockaddr结构  */
	bzero(&server_addr, sizeof(struct sockaddr_in));	//置字节字符串server_addr的前n个字节为零且包括‘\0
	server_addr.sin_family = AF_INET;	//设置为IPv4通信
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);	//服务器IP地址--允许连接到所有本地地址上
	server_addr.sin_port = htons(portnumber);	//服务器端口号 

	int n = 1;
	/* 如果服务器终止后,服务器可以第二次快速启动而不用等待一段时间  */
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &n, sizeof(int));

	/* 绑定sockfd描述符  */
	if (bind(sockfd, (struct sockaddr *) (&server_addr), sizeof(struct sockaddr)) == -1) {
		fprintf(stderr, "Bind error:%s\n\a", strerror(errno));
		exit(1);
	}

	/* 监听sockfd描述符  */
	if (listen(sockfd, 5) == -1) {
		fprintf(stderr, "Listen error:%s\n\a", strerror(errno));
		exit(1);
	}

	while (1) {
		sin_size = sizeof(struct sockaddr_in);
		new_fd = accept(sockfd, (struct sockaddr *) (&client_addr), &sin_size);

		if ((new_fd < 0) && (errno == EINTR))
			continue;

		else if (new_fd < 0) {
			fprintf(stderr, "Accept error:%s\n\a", strerror(errno));
			continue;
		}

		if ((n = fork()) == 0) {
			fprintf(stderr, "Server get connection from %s\n", inet_ntoa(client_addr.sin_addr));	//在服务端输出连接信息

			char str[] = "用户名：";

			write(new_fd, str, strlen(str));	//将str发送到客户端
			recv(new_fd, buffer, BUFFERSIZE, 0);	//从客户端接收buffer数据

			int i = 0;
			while (buffer[i] != '\r') {
				name[i] = buffer[i];
				i++;
			}
			name[i] = '\0';
			memset(buffer, 0, sizeof(buffer));	//清空缓冲区

			char str1[] = "密码：";
			write(new_fd, str1, strlen(str1));	//将str1发送到客户端
			recv(new_fd, buffer, BUFFERSIZE, 0);	//从客户端接收buffer数据

			i = 0;
			while (buffer[i] != '\r') {
				pwd[i] = buffer[i];
				i++;
			}
			pwd[i] = '\0';
			memset(buffer, 0, sizeof(buffer));

			if (strcmp(name, login_name) == 0) {
				usr_flag = 1;
			}
			if (strcmp(pwd, login_pwd) == 0) {
				pwd_flag = 1;
			}

			if (usr_flag == 1 && pwd_flag == 1) {
				sprintf(str_info, "登录成功！");

				print_write(str_info);

				/*获取系统当前时间 */
				time_t timep;
				time(&timep);
				system("reset");

				char command[64];	//保存用户的输入操作

				sprintf(str_info, "\r\n\r\n");	//换行
				write(new_fd, str_info, strlen(str_info));

				memset(str_info, 0, sizeof(str_info));
				sprintf(str_info, "xxx System *Plus : Release 1.1.0.0 Production on %s", ctime(&timep));	//显示当前系统时间
				write(new_fd, str_info, strlen(str_info));

				sprintf(str_info, "Copyright (c) 2014, 2015, xxx Co.ltd.  All rights reserved.\r\n");	//显示版权信息
				write(new_fd, str_info, strlen(str_info));

				sprintf(str_info, "\r\n");
				write(new_fd, str_info, strlen(str_info));

				memset(str_info, 0, sizeof(str_info));
				sprintf(str_info, "帮助命令：help\r\n\n");
				write(new_fd, str_info, strlen(str_info));
				memset(str_info, 0, sizeof(str_info));

				while (1)		//等待用户输入操作命令
				{
					sprintf(str_info, "xxx_SYS> ");
					write(new_fd, str_info, strlen(str_info));

					flag = 0;	//标志位复位

					recv(new_fd, buffer, BUFFERSIZE, 0);
					i = 0;
					while (buffer[i] != '\r') {
						command[i] = buffer[i];
						i++;
					}
					command[i] = '\0';

					printf("%s\n", command);	//主机端显示

					match_cmd(command);	//匹配用户的输入
				}
			}
			else {
				sprintf(str_info, "用户名或密码错误！");
				print_write(str_info);
				close(new_fd);	//关闭SOCKET连接
			}

		}
		else if (n < 0) {
			printf("Fork Error:%s\n\a", strerror(errno));
		}

	}

}

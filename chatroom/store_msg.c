#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "server.h"
#include "debug.h"

/****************************************************************************
函    数:    init_msg
功    能:    对user信息进行初始化
传入参数:    无
传出参数:    stu: 经初始化后的结构体
返    回:    stu: 经过初始化的结构体地址
****************************************************************************/


struct user *init_msg()
{
	struct user *stu = (struct user *)malloc(sizeof(struct user));

	memset(stu,0,sizeof(struct user));
	
	fprintf(stdout,"Please input your name: ");
	fgets(stu->name, sizeof(stu->name),stdin);
	fprintf(stdout, "Please input password: ");
	fgets(stu->password,sizeof(stu->password),stdin);
	
	return stu;	
}



/*******************************************************************************
函    数:    write_in
功    能:    将一个结构体中的信息写入文件
传入参数:    stu:需要写入文件的user结构地址
传出参数:    无
返    回:    0:检测程序执行结果
*******************************************************************************/
int write_in(struct user *stu)
{
	int fd_open = open("user_msg.txt", O_RDWR | O_APPEND); /*open user message file*/
	
	if(fd_open == -1)
	{
		perror("read");
		return -1;
	}

	int fd_write = write(fd_open, stu, sizeof(struct stu));
	
        if(fd_wirte == -1)
	{
		perror("write");
		return -1;
	}

 	close(fd_open);
	return 0;
}






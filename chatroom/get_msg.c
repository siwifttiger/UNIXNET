#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "server.h"
#include "debug.h"


struct user_link *user_head = NULL;
/********************************
function: insert_link

make  user infomation to a link

param: temp
return : null
*********************************/
int insert_link(struct user_link * temp)
{

	if(user_head == NULL)
	{
		user_head = temp;
		return 0;
	}	

	//head insertion
	temp->next = user_head;
	user_head = temp;

	return 0;
}


/********************************************************************
函    数:    traverse
功    能:    对用户链表遍历
传入参数:    temp:用来存放链表头节点
传出参数：   无
返    回:    无
********************************************************************/

int traverse(struct user_link *head)
{
	struct user_link *stu = head;
	fputs("用户信息如下\n",stderr);
	for(stu; stu != NULL; stu = stu->next)
	{
		fprintf(stderr, "name: %s, password: %s\n",stu->user_msg.name,stu->user_msg.password);
	}

	fputs("查看用户信息完毕\n",stderr);
	return 0;
}



/*********************************************************************
函    数:    insert_online
功能描述:    将登录的用户插入到用户链表中
传入参数:    load_user:要插入到用户链表中的用户
传出参数:    无
返    回:    0:程序执行结果
*********************************************************************/

int insert_online(struct user_link *load_user)
{
	if(online_head == NULL)
	{
		online_head = load_user;
		return 0;
	}

	load_user->next = online_head;
	online_head = load_user;
	return 0;
}



/********************************************************************
函    数:    get_msg
功    能:    得到用户文件里的用户信息并做成链表
传入参数：   无
传出参数：   无
返    回:    0: 程序执行完毕结果
********************************************************************/

int get_msg()
{
	struct user_link *temp = (struct user_link *)malloc(sizeof(struct user_link));

	int fd_open = open("user_msg.txt",O_RDONLY);
	int fd_read = 0;
	if(fd_open == -1)
	{
		perror("open");
		return -1;
	}
	while( (fd_read = read(fd_open,&temp->user_msg,sizeof(struct user))) > 0 )
	{
		temp->next = NULL;
		temp->confd = -1;
		insert_link(temp);
		temp = (struct user_link*)malloc(sizeof(struct user_link));
	}

	if(fd_read == -1)
	{
		perror("read");
		return -1;
	}
	
	close(fd_open);
	return 0;
}


/***************************************************************************
函    数:    check_user
功    能:    查找用户
传入参数:    name: 需要查找的用户的名字
传出参数:    无
返    回:    1:在链表中找到用户
             0:在链表中没有找到用户
****************************************************************************/

int check_user(char *name)
{
	if(user_head == NULL) return 0;

	struct user_link *temp = user_head;
	for(temp; temp != NULL; temp = temp->next)
	{
		if(strcmp(temp->user_msg.name, name) == 0)
			return 1;
	}	
	return 0;
}



int check_online(char *name)
{
	if(online_head == NULL) return 0;
		
	struct user_link *temp = online_head;
	
	for(temp; temp != NULL; temp = temp->next)
	{
		if(strcmp(name, temp->user_msg.name) == 0)
			return 1;
	}
	return 0;
}


/*************************************************************************
函    数:    free_all
功    能:    释放所有节点
传入参数:    head:需要释放的链表的头节点
传出参数:    无
返    回:    0：程序执行结果
************************************************************************/

int free_all()
{
	if(user_head == NULL)
		return 0;

	struct user_link *temp = NULL;
	while(user_head != NULL)
	{
		temp = user_head;
		user_head = user_head->next;
		free(temp);
	}

	return 0;
}


/**************************************************************************
函    数:    find_online
功    能:    在用户链表中找到用户
传入参数:    user_name:需要查找的用户的头指针
传出参数:    无
返    回:    temp:找到用户
             0:在链表中没有找到用户
**************************************************************************/
struct user_link *find_online(char *name)
{
	struct user_link *temp = NULL;
	for(temp = online_head; temp != NULL; temp = temp->next)
	{
		if(strcmp(name, temp->user_msg.name) == 0)
		{
			debug_msg("find online: found\n");
			return temp;
		}
	}
	return NULL;
}



/**************************************************************************
函    数:    check_password
功    能:    检测用户和密码是否匹配
传入参数:    temp_user:需要被检测的用户信息
传出参数:    无
返    回:    1: 用户名和密码匹配
             0: 用户名和密码不匹配
***************************************************************************/

int check_password(struct user *temp_user)
{
	
	struct user_link *temp = user_head;
	for(temp; temp != NULL; temp = temp->next)
	{	
		if( (strncmp(temp->user_msg.name, temp_user->name,strlen(temp->user_msg.name)) || strncmp(temp->user_msg.password,temp_user->password,strlen(temp->user_msg.password)) ) == 0 )
		return 1;
		
	}

	return 0;

}

/*************************************************************************
函    数:    user_node
功    能:    做一个用户节点
传入参数:    usr_msg:需要做成节点的用户信息
             confd  :用户与服务器通信的端口号
传出参数:    无
返    回:    temp_user做好的节点首地址
*************************************************************************/

struct user_link *user_node(struct user *user_msg, int confd)
{
	struct user_link *temp = (struct user_link *)malloc(sizeof(struct user_link));
	
	strcpy(temp->user_msg.name,user_msg->name);
	strcpy(temp->user_msg.name,user_msg->password);
	temp->confd = confd;
	temp->speak_flag = 1;
	temp->next = NULL;
	return temp;
}


/***************************************************************************
函    数:    delete_online
功    能:    当用户下线的时候删除在线节点
传入参数:    user_node:需要被删除的节点
传出参数:    无
返    回:    0:程序成功执行结果
***************************************************************************/

int delete_online(struct user_link *user_node)
{
	if(online_head == NULL) return 1;
	struct user_link *dummy = (struct user_link *)malloc(sizeof(struct user_link));
	struct user_link *current = (struct user_link *)malloc(sizeof(struct user_link));

	dummy->next = online_head;
	current = dummy;
	while(current && current->next)
	{
		struct user_link *q = current->next;
		if(q == user_node)
		{
			debug_msg("delete_online temp:name %s",current->user_msg.name);
			debug_msg("delete_online name %s",user_node->user_msg.name);
			current->next = q->next;
			delete q;
			debug_msg("testing delete node");
			return 0;
		}else current = current->next;
		
	}
	return 1;
}


/***********************************************************
函    数:    find_by_confd
功    能:    通过通信端口找到用户
传入参数:    confd: 通信端口
传出参数:    无
返    回:    
***********************************************************/
struct user_link *find_by_confd(int confd)
{
	struct user_link *temp = (struct user_link *)malloc(sizeof(struct user_link));

	for(temp = online_head; temp != NULL; temp = temp->next)
	{
		if(temp->confd == confd)
			return temp;

	}
	return NULL;
}


/***********************************************************************
函    数:    kick_all
功    能:    踢出所有在线用户
传入参数:    
传出参数:
返    回:    0:程序争取执行结果
***********************************************************************/

int kick_all()
{
	struct user_link *temp = (struct user_link *)malloc(sizeof(struct user_link));

	for(temp = online_head; temp != NULL; temp = temp->next)
	{
		delete_online(temp);
	}

	return 0;
}

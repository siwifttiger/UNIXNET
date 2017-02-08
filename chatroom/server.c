/***********************************************************************
文    件:    server.c
文件描述:    提供聊天服务功能
***********************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "server.h"
#include "wrap.h"
#include "debug.h"

struct user_link *online_head = NULL;

pthread_mutex_t reg_lock = PTHREAD_MUTEX_INITIALIZER;  // register mutex lock
pthread_mutex_t log_lock = PTHREAD_MUTEX_INITIALIZER; // login mutex lock

/***********************************************************
函    数:    server
功    能:    提供聊天服务
传入参数:    无
传出参数:    无
返    回:    无
***********************************************************/

int server()
{
	struct sockaddr_in servaddr;		//server ip addr
	struct sockaddr_in cliaddr;		//client ip addr
	
	socklen_t cliaddr_len;			//client addr length

	int sockfd;				//listened socket
	int confd;				//connected socket

	pthread_t thread;

	get_msg();				//load user configer file

	
	sockfd = Socket(AF_INET,SOCK_STREAM,0);
	/*initialize server structer*/
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERVERPORT);

	Bind(sockfd,(struct sockaddr *)servaddr,sizeof(servaddr));
	int opt = 1;
	setsockopt(sockfd,IPPROTO_TCP,SO_REUSEADDR,&opt,sizeof(opt));
	
	Listen(sockfd,20);

	while(1)
	{
		fput("Ready to accept connection...\n",stderr);
		cliaddr_len = sizeof(cliaddr);
		confd = Accept(sockfd,(struct sockaddr *)&cliaddr, &cliaddr_len);
		
		/*create client manager thread*/
		pthread_create(&thread,0,manage_client,(void*)confd);
	}
		
}

/************************************************************************
函    数:    manage_client
功    能:    处理客户端的线程函数
传入参数:    服务器与客户端通信的端口
传出参数:    无
返    回:    0:程序执行结果
***********************************************************************/

void *manage_client(void *arg)
{
	int confd = (int)arg;
	
	/*call the funtion destruct client requierment*/
	get_choice(confd);
	
	pthread_exit((void*)0);
}




/************************************************************************
函    数:    get_choice
功    能:    得到用户请求
传入参数:    confd: 用来与客户端通信的端口 
传出参数:    无
返    回:    0: 程序正确执行
             -1:程序错误执行
************************************************************************/

int get_choice(int confd)
{
	/*temporary message buffer*/
	char buf[MAXSIZE];
	memset(buf,0,MAXSIZE);
	
	/*user and server connect buffer*/
	struct menu_choice tran_msg;
	memset(&tran_msg,0,sizeof(tran_msg));
	
	int n_write;
	int n_read;
	
	while(1)
	{
		/*读取客户请求*/
		n_read = Read(confd,&tran_msg, sizeof(tran_msg)); 
		if(n_read <= 0)
		{
			fputs("get_choice: 进入时服务器读取错误\n");
			close(fd);
			return -1;
		}
		
		switch(tran_msg.mode[0])
		{
			case REGISTER:
			{
				my_register(confd,&tran_msg.user_msg);
				break;	
			} 
		
			case  LOGIN:
			{
				load(confd,&tran_msg.user_msg);
				break;
			}
			case QUIT:
			{
				strcpy(buf,"You have quit successfully.\n");
				n_write = Write(confd,buf,strlen(buf));
				if(n_wirte <= 0)
				{
					fputs("Write error in get_choice\n",stderr);
					close(confd);
					return -1;

				}
				close(confd);
				return 0;
			}
		}
		memset(&tran_msg,0,sizeof(tran_msg));
		memset(buf,0,MAXSIZE);
	}
	 	
}

/******************************************************************************
函    数:    my_register
功    能:    完成用户的注册工作
传入参数:    confd : 服务器和客户端的通信接口
传出参数:    无
返    回:    -1:程序错误返回
              0:程序正确执行
******************************************************************************/
int my_register(int confd, struct user *temp_user)
{
	debug_msg("d: my_reg- %s",temp_user->name);
	char buf[MAXSIZE];
	memset(buf,0,MAXSIZE);
	int n_write;
	
	/*判断用户名是否已经存在*/
	if(check_user(temp_user->name) == 1)
	{
		strcpy(buf,"Sorry!Register failed, user has existed\n");
		n_write = Write(confd,buf,strlen(buf));
		if(n_write <= 0)
		{
			fputs("Write error in my_register\n",stderr);
			close(confd);
			return -1;
		}
	}	
	else
	{
		pthread_mutex_lock(&reg_lock);  //链表和文件操作地方上锁
		
		write_in(temp_user);
		struct user_link *temp = user_node(temp_user,-1);
		insert_link(temp);
		
		pthread_mutex_unlock(&reg_lock);
		
		strcpy(buf,"Register success!\n");
		n_write = Write(confd,buf,strlen(buf));
		if(n_wirte <= 0)
		{
			fputs("Write error in my_register\n",stderr);
			close(confd);
			return -1;
		}

	}
	return 0;
}





/************************************************************************
函    数:    load
功    能:    处理客户登录的函数
传入参数:    confd:服务器与客户端通信的端口
传出参数:    无
返    回:    0:程序正确执行
             -1：程序错误执行
************************************************************************/

int load(int confd, struct user *temp_user)
{
	char buf[MAXSIZE];
	memset(buf,0,MAXSIZE);
	int n_write;
	
	debug_msg("查看注册用户\n");
	traverse(user_head);
	debug_msg(查看注册用户完毕\n);
	
	/*查看该用户已经登录*/
	if(check_online(temp_user->name) == 1)
	{
		strcpy(buf,"User has already logged in\n");
		n_write = Write(confd,buf,strlen(buf));
		if(n_write <= 0){
			fputs("load: write error",stderr);
			close(confd);	
			return -1;	
		}
		return 0;
	}
	
	if(check_password(temp_user) == 1)
	{
		strcpy(buf,"Log in success.Welcom to chatroom\n");
		n_write = Write(confd,buf,strlen(buf));
		if(n_write <= 0)
		{
			fputs("load failure\n",stderr);
			close(confd);
			return -1;
		}
		
		pthread_mutex_lock(&log_lock); /*涉及到用户在线链表的操作的地方上锁*/

		struct user_link *temp = user_node(temp_user,confd);
		notice(temp_user->name,ONLINE);
		insert_online(temp);
		debug_msg("登录处\n");
		traverse(online_head);
		debug_msg("插入在线列表成功\n");
		pthread_mutex_lock(&log_lock);		
	}
	else
	{	
		strcpy(buf,"log failure, user name and password can not match\n");
		
		n_write = Write(confd, buf, strlen(buf));
		if(n_write <= 0)
		{
			fputs("load: write failure\n",stderr);
			close(confd);
			return -1;
		}
		return 0;
	}
	
	memset(buf,0,MAXSIZE);
	manage_chat(confd);
	return 0;
}

/*******************************************************************
函    数:    manage_chat
功    能:    管理聊天模式
传入参数:    confd:聊天用的端口号
传出参数:    无
返    回:    1:程序错误返回
*******************************************************************/

int manage_chat(int confd)
{
	struct chat tran_msg;
	
	memset(&tran_msg,0,sizeof(tran_msg));
	
	int n_read;
	int n_write;
	
	struct user_link *temp_online = NULL;
	
	while(1)
	{
		n_read = Read(confd, &tran_msg,sizeof(tran_msg));
		if(n_read <= 0)
		{
			fputs("manage_chat: Read failure\n",stderr);
			temp_online = find_by_confd(confd);
			debug_msg("manage_chat: m_name %s", temp_online->user_msg.name	);
			delete_online(temp_online);
			notice(temp_online->user_msg.name,OFFLINE);  // notice OFFLINE	
			close(confd);
			return -1;
		}
	
		/*analyse user's requierments*/
		debug_msg("manage_chat:n_read = %d, buf[0] = %c\n",n_read,tran_msg.mode[0]);
		
		switch(tran_msg.mode[0])
		{
			case PRIVATE:
			{
				my_private(confd,&tran_msg);
				break;
			}	
			
			case PUBLIC:
			{	
				my_public(confd,&tran_msg);
				break;	
			}

			case QUIT_CHAT:       //log out
			{
				struct user_link *temp_link = find_online(tran_msg.m_name);
				debug_msg("要删除的名字:%s", temp_link->user_msg.name);
				debug_msg("要删除的通信端口号:%d\n", temp_link->confd);
				pthread_mutex_lock(&log_lock);
				delete_online(temp_link);
				notice(temp_link->user_msg.name,OFFLINE);
				pthread_mutex_unlock(&log_lock);
			
				debug_msg("查看删除后的在线用户链表\n");
                		traverse(online_head);
				debug_msg("查看删除后的在线用户链表完毕\n:");
				strcpy(tran_msg.m_name,"Sever\n");
				strcpy(tran_msg.msg,"您已经成功退出聊天模式\n");
				n_write = Write(confd,&tran_msg,sizeof(tran_msg));
				if(n_write <= 0)
				{
					temp_online = find_by_confd(confd);
					delte_online(temp_online);					      close(confd);
					notice(temp_online->user_msg.name,OFFLINE);
					fputs("服务器读取错误\n",stderr);
					return -1;
				}
				
				return 0;
						
			}
			
			case SCAN_ONLINE:
			{
				scan_online(confd);
				debug_msg("scanf_online: %s", tran_msg.mode);
				break;
			}
			
			case KICK_ALL:    //super root
			{
				struct user_link *temp = NULL;
				notice_close_serv(confd);
				
				sleep(10);
				
				strcpy(tran_msg.m_name, "Server\n");
				strcpy(tran_msg.msg,"Sever has been closed, you are forced to log out\n");
				
				for(temp = online_head; temp != NULL; temp=temp->next)
				{
					n_write = Write(temp->confd,&tran_msg, sizeof(tran_msg));
					if(n_write <= 0)
					{
						fputs("The user is not online\n",stderr);
						temp_online = find_by_confd(temp->confd);
						delete_online(temp_online);
						notice(temp_online->user_msg.name,OFFLINE);
						close(confd);
						return -1;
					}
					temp_online = find_by_confd(temp->confd);
					delete_online(temp_online);
					
				}
				exit(0);
			}

			case MANAGE_SPEAK:
			{	
				debug_msg("Man-sp:进入服务\n");
		 		debug_msg("Man-sp:f_name :%s confd:%d\n", tran_msg.f_name, confd);
	         		manage_speak(&tran_msg, confd);
				break;		
			}

			case KICK_ONE:
			{
				debug_msg("kick one\n");
				kick_one(&tran_msg,confd);
				break;
			}

			default:
			{
				strcpy(tran_msg.msg,"您输入的选项有误\n");
				n_write = Write(confd, &tran_msg, sizeof(tran_msg));   /*写回给客户端*/
               			 if (n_write <= 0)
				{
		    			temp_online = find_by_confd(confd);
		    			delete_online(temp_online);
                    			notice(temp_online->user_msg.name, OFFLINE);
		    			close(confd);
		    			fputs("服务器读取错误\n", stderr);
		    			return -1;
				}

				break;
			}

		}
		memset(&tran_msg,0,sizeof(tran_msg));
		
	}
		
}


/************************************************************************
函    数:    my_private
功    能:    处理用户私聊的函数
传入参数:    confd:用来和客户通信的网络端口
传出参数:    无
返    回:    
************************************************************************/

int my_private(int confd, struct chat *temp)
{
	debug_msg("private: confd = %d\n",confd);
	struct user_link *temp_user = NULL;

	int n_read;
	int n_write;
	
	int temp_confd;		
	struct user_link *temp_online = NULL;
	
	debug_msg("private NAME = %s", temp->f_name);
	debug_msg("private NAME = %s", temp->m_name);
		
	temp_user = find_online(temp->m_name);
	
	//judge if can speak
	if(temp_user->speak_flag == 0)
	{
		strcpy(temp->m_name,"Sever\n");
		strcpy(temp->msg,"Sorry,you can't speak,plead contact administrator!");
		
		n_write = Write(confd,temp,sizeof(struct chat));
		if(n_write <= 0)
		{
			fputs("private : 读取错误\n",stderr);
			temp_online = find_by_confd(confd);
			delete_online(temp_online);
			notice(temp_online->user_msg.name,OFFLINE);
			close(confd);
			return -1;
		}
		return 0;
	}

	//judge if the other one is online?
	temp_user = find_online(temp->f_name);
	
	if(temp_user == NULL)    			//offline
	{
		strcpy(temp->m_name,"Server\n");
		strcpy(temp->msg, "The people you want to talk is offline\n");
		
		n_write = Write(confd,temp,sizeof(struct chat));                   //feedback to client
		if(n_write <= 0)
		{
			fputs("private: 读取错误\n",stderr);
			temp_online = find_by_confd(confd);
			delete_online(temp_online);
			notice(temp_online->user_msg.name,OFFLINE);
			close(confd);
			return -1;
		}
		return 0;
	}

	//begin connection
	debug_msg("private: name = %s",temp->f_name);
	temp_confd = temp_user->confd;
	debug_msg("private: confd = %d",temp_confd);

	debug_msg("开始通信\n");
	n_write = Write(temp_confd,temp,sizeof(struct chat));   		// to  the other client
	if(n_write <= 0)
	{
		fputs("private: Write failure\n",stderr);
		temp_online = find_by_confd(temp_confd);
		delete_online(temp_online);
		notice(temp_online->user_msg.name,OFFLINE);
		close(temp_confd);
		return -1;
		return -1;
	} 

	debug_msg("结束通信\n");
	return 0;
}



/***************************************************************************
函    数:    my_public
功    能:    处理客户端的群聊模式
传入参数:    confd
传出参数:    无
返    回:    无
***************************************************************************/


int my_public(int confd, struct chat *tran_msg)
{
	int n_read;
	int n_write;
	
	struct user_link *temp_user = NULL;
	struct user_link *temp_online  = NULL;
	
	    /*判断是否被禁言*/
   	 if (temp->speak_flag == 0)
    	{
        	strcpy(tran_msg->m_name, "Server\n");
		strcpy(tran_msg->msg, "Sorry ,you can't speak! Plead conect to administrator!");
	
		n_write = Write(confd, tran_msg, sizeof(struct chat));  /*发送到指定的聊天对象*/
		if (n_write <= 0)
		{   
	   	 	fputs("private : 读取错误\n", stderr);
            		temp_online = find_by_confd(confd);
	    		delete_online(temp_online);
            		notice(temp_online->user_msg.name, OFFLINE);
	    		close(confd);
	    		return -1;
		}

		return 0;
	} 
	
	debug_msg("my_public:tran_msg->msg: %s\n", tran_msg->msg);

	//send to ever user
	for(temp_user = online_head;temp != NULL; temp = temp->next)
	{
		n_write = Write(temp->confd,tran_msg,sizeof(struct chat));
		if(n_write <= 0)
		{
			fputs("public: 写入错误\n",stderr);
			temp_online = find_by_confd(temp_user->confd);
			delete_online(temp_online);
			notice(temp_online->user_msg.name,OFFLINE);
			close(temp_user->confd);
			
		}
		debug_msg("my_public:temp_user->confd %d\n",temp_user->confd);
	}
	return 0;

}


/************************************************************************
函    数:    scan_online
功    能:    查看在线用户
传出参数:    服务器与客户端通信的端口
传出参数:    无
返    回:    0: 程序成功执行结果
*************************************************************************/

int scan_online(int confd)
{
	struct user_link *temp_online = NULL;
	struct user_link *temp_user   = NULL;	

	struct chat tran_msg;
	memset(&tran_msg,0,sizeof(tran_msg));
	
	int n_write;
	int debug_flag = 0;

	strcpy(tran_msg.m_name,"Server\n");
	strcpy(tran_msg.msg,"Begin to browser the online user\n");
	
	n_write = Write(confd,&tran_msg,sizeof(tran_msg));
	if(n_write <= 0)
	{
		temp_online = find_by_confd(confd);
		delete_online(temp_online);
		fputs("scan_online: 读取错误\n",stderr);
		notice(temp_online->user_msg.name,OFFLINE);
		close(confd);
		return -1;
	}

	memset(&tran_msg,0,sizeof(tran_msg));
	
    /*循环将在线用户发送给需要查看在线用户的客户*/
    for (temp = online_head; temp != NULL; temp = temp->next)
    {
        /*将在线用户的名字存进需要传送的缓存区*/
        strcpy(tran_msg.m_name, "Server\n");
	
	strcpy(tran_msg.msg, temp->user_msg.name);
	
	/*将用户姓名写入到通信端口*/
	n_write = Write(confd, &tran_msg, sizeof(tran_msg));  
        if (n_write <= 0) 
	{
            temp_online = find_by_confd(confd);
	    delete_online(temp_online);
	    notice(temp_online->user_msg.name, OFFLINE);
            fputs("scan_online：读取错误\n", stderr); 
            close(confd);
	    return -1;
	}
        
	debug_msg("debug_flag = %d\n", debug_flag++);
        memset(&tran_msg, 0, sizeof(tran_msg));
    }
    
    	strcpy(tran_msg.m_name, "Server\n");     
	strcpy(tran_msg.msg, "Browser the online user over\n");

    /*将用户姓名写入到通信端口*/
    n_write = Write(confd, &tran_msg, sizeof(tran_msg));  
    if (n_write <= 0) 
    {   
        close(confd);
        temp_online = find_by_confd(confd);
	delete_online(temp_online);
	notice(temp_online->user_msg.name, OFFLINE);
        fputs("scan_online：读取错误\n", stderr); 
        return -1;
    }

	return 0;
}

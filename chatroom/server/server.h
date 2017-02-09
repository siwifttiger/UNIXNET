/****************************************************
文件：server.h
功能：聊天室服务器端的函数声明和用户链表结构体的定义
*****************************************************/

#ifndef SERVER_H
#define SERVER_H

#define SERVERPORT   8000   	        /*服务器绑定的端口号*/
#define MAXSIZE      1024               /*缓存的最大字节数*/
#define MAX_MSG      256		/*聊天缓存区的最大值*/
#define REGISTER     '0'		/*注册*/
#define LOGIN	     '1'		/*登录*/
#define QUIT	     '2' 		/*退出*/
#define PRIVATE      '3'	        /*私聊*/
#define PUBLIC       '4'		/*公聊*/
#define SCAN_HIS    '5'		/*查看聊天记录*/
#define	SCAN_ONLINE '6'		/*查看在线人员*/
#define QUIT_CHAT    '7'                /*退出聊天*/
#define KICK_ALL     '8'                /*超级用户剔除所有用户*/
#define MANAGE_SPEAK '9'                /*管理用户能否发言*/
#define KICK_ONE     'k'                /*踢出一个人*/
#define ONLINE        1
#define OFFLINE       0



/*用户结构体的定义*/
struct user{
	char name[20];
	char password[20];

};

/*用户链表节点*/
struct user_link
{
	struct user user_msg;
	int confd;              //用户通信端口
	int speak_flag;        //发言标志，1为正，0为否
	struct user_link *next;
};


/*菜单选择的结构体*/
struct menu_choice
{
	char mode[10];
	struct user user_msg;
};


/*得到用户聊天模式*/
struct chat
{
	char mode[10];
	char f_name[20];
	char m_name[20];
	char msg[MAX_MSG];
};


extern struct user_link *user_head;   //用户链表头的声明
extern struct user_link *online_head; //在线用户链表头指针声明

/*************函数声明*****************/

struct user* init_msg(); 				//用户信息初始化
int write_in(struct user*);				//将用户信息写入文件
int insert_link(struct user_link*);			//将初始化后的用户信息插入链表
int traverse(struct user_link*);			//遍历用户链表
int get_msg();						//get msg from file and make linklist
int server();						//聊天室服务器
void *manage_client(void*);				//用来处理与客户端联系的线程函数
int insert_online(struct user_link*);			//将登陆用户插入到在线用户链表、
int check_user(char*);					//检测注册链表中用户是否存在
int check_online(char *);                               //check an online user by name
int my_register(int , struct user*);			//处理用户注册的函数
int load(int, struct user *);				//处理用户登录的函数
int free_all();						//释放链表内存
struct user_link *find_online(char *);			//查找在线用户
int check_password(struct user *);				//检测用户名和密码是否匹配的函数
struct user_link *user_node(struct user *, int);        //制作在线链表的结构体的地址
int manage_chat(int );					//管理用户聊天的函数
int delete_online(struct user_link *);			//删除在线用户链表节点
int my_private(int , struct chat *);			//处理私聊
int my_public(int , struct chat *);			//处理群聊
struct user_link *find_by_confd(int);			//通过通信端口号来查找用户
int kick_all();						//管理员踢出所有用户
int scan_online(int );					//查看在线用户
int manage_speak(struct chat *, int ); 			//管理用户发言
int kick_one(struct chat *, int ); 			//踢出某个用户
int notice(char *, int );				//通知用户
int notice_close_serv(int);				//通知服务器关闭的函数

#endif

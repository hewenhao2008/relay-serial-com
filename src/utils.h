#ifndef _UTILS_H
#define _UTILS_H


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>  
#include <string.h>
#include <sys/stat.h>  
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>

/* 数据流方向 */
#define DIR_TO_PHONE 0
#define DIR_TO_SERIAL 1
#define DIR_TO_SERVER 2
#define DIR_TO_BLUETOOTH 3

/* 申请内存 */
void *my_malloc(size_t size);
/* 释放内存 */
#define my_free(ptr)							\
    if(ptr)                                     \
    do {										\
        free(ptr);								\
        ptr = NULL;								\
    } while(0)

#define load_timer(tv)                            \
	tv.tv_sec = 0;                                \
	tv.tv_usec = 100*000;

/* 封包用户输入的数据，仿照shadowsocks-libev的封装包格式 */
typedef struct user_content {
	int index;		  /* 已发送字节索引 */
	int data_size;				/* data包的大小 */
	int direction;				/* 包发送方向 */
	int fd;						/* 发送到目的 的 文件描述符 */
	char *ip;
	char *port;
	char *data;
	char mac[18];				/* 蓝牙物理地址 */
	char *device;				/* 串口 设备名称 */
	struct sp_port *com_port;		/* 串口port结构体 */
	struct sp_port_config *com_conf; /* 串口配置 */
} user_content_t;

/* 输入数据包 返回一个user_content的指针，将其打包发送者header，使用后记得释放内存 */
/* 
   格式：目的地 数据
   可以接收的data举例：
   192.168.444.1:3333:xxxxdataxxxx
   /dev/ttyUSB0:xxxdataxxx
   [00:11:22:33:44:55]:hellofrombluetooth
 */
user_content_t *new_user_content_from_str(char *in, char *header,int direction);

// TCP bind, lack of listen
int create_server_socket(const char *host,const char *port);

/* 将ip和port封包 */
char *get_header_ipv4(char *ip,char *port);

int get_direction(char *in);

/* 重定向流量 */
int redirect_from_user_content(user_content_t *in);

#endif

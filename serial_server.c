//Time-stamp: < serial_server.c 2016-06-23 00:44:44 >
/*说明：串口端的接收数据，模拟串口
 */
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
#include "serial_server.h"


com_port_t *open_com(char *devicename){
	com_port_t *tmp=my_malloc(sizeof(com_port_t));
	/* 打开设备 */
    if(sp_get_port_by_name(devicename, &(tmp->port)) != SP_OK){
        fprintf(stderr, "Cannot find the serial port %s\n",devicename);
		sp_free_port(tmp->port);
		my_free(tmp);
		return NULL;
    }

	/* 打开串口 */
    if(sp_open(tmp->port, SP_MODE_READ_WRITE) != SP_OK) {
        fprintf(stderr, "Cannot open the serial port %s\n",devicename);
		sp_free_port(tmp->port);
		my_free(tmp);
		return NULL;
    }
	
	/* 文件描述符 */
	if(sp_get_port_handle(tmp->port,&(tmp->fd))!=SP_OK){
		fprintf(stderr, "Cannot get the serial port fd\n");
		sp_free_port(tmp->port);
		my_free(tmp);
		return NULL;
	}else{
		printf("open %s ok, the fd is %d\n",devicename,tmp->fd);
	}
	/* 配置结构体 */
    sp_new_config(&(tmp->conf));
    sp_set_config_baudrate(tmp->conf, 115200);
    sp_set_config_parity(tmp->conf, SP_PARITY_NONE);
    sp_set_config_bits(tmp->conf, 8);
    sp_set_config_stopbits(tmp->conf, 1);
    sp_set_config_flowcontrol(tmp->conf, SP_FLOWCONTROL_NONE);

	/* 设置串口 */
	if(sp_set_config(tmp->port, tmp->conf) != SP_OK){
        fprintf(stderr, "Cannot configure the serial port\n");
		sp_free_port(tmp->port);
		sp_free_config(tmp->conf);
		my_free(tmp);
		return NULL;
    }

	return tmp;
}

void close_com(struct sp_port *port_blue,struct sp_port_config *port_blue_config){
	sp_free_port(port_blue);
	sp_free_config(port_blue_config);
}

// 过时的宏定义,仅用与测试模拟一个串口设备调试
#ifdef SERIAL_MAIN_OLD
int main(int argc,char *argv[]){
	char in[MAX_BUFFER_LENTH];
	char *IP,*PORT;
	/* 用户态的数据包格式ASCII */
	user_content_t *my_content=NULL;
	int sockfd_to_remote,serialfd,byte_readed;

	int status;
	struct addrinfo hints, *res; /* 连接到remote */

	struct sp_port *port_blue;
	struct sp_port_config *port_blue_config;

	if(open_com("/dev/ttyUSB0",&serialfd,port_blue,port_blue_config)==0)
		printf("ok! %d\n",serialfd);
	else
		printf("fail!\n");

	/* 暂时从这里退出 */
	return 0;
	if (argc != 3) {
		fprintf(stderr,"usage: %s ip port\n",argv[0]);
		return 1;
	}
	
	IP=argv[1];
	PORT=argv[2];
	
	printf("hello\n");

	if(read_input(in));
	my_content=new_user_content_from_str(in,DIR_TO_SERVER);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // AF_INET 或 AF_INET6 可以指定版本
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // fill in my IP for me

	/* if ((status = getaddrinfo(my_content->ip, my_content->port, &hints, &res)) != 0) { */
	if ((status = getaddrinfo(IP, PORT, &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		return 1;
	}
	
	/* 建立一個 socket： */
	sockfd_to_remote = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	/* connect! */
	if(status=(connect(sockfd_to_remote, res->ai_addr, res->ai_addrlen))<0){
		perror("connect error");
		return 1;
	}

	if(-1==sendall(sockfd_to_remote,my_content))printf("sendall fail.\n");

	my_free(my_content->data);
	my_free(my_content);
	close_com(port_blue,port_blue_config);
	return 0;
}
#endif

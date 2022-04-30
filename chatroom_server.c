#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>


//定义一个全局变量，记录有多少个连接
int t = 0;
//存储连接的通信描述符结构体
struct Sockinfo
{
	int fd;
	pthread_t pth;
	struct sockaddr_in addr;
	
};

struct Sockinfo sockinfo[128];

void * pthreadfunc(void * arg)
{
	struct Sockinfo *pinfo = (struct Sockinfo *)arg;

	pinfo->pth = pthread_self();
	printf("线程ID：%ld\n", pinfo->pth);	
	
	char addrinfo[16];
	int ret;
		
	memset(addrinfo, 0, sizeof(addrinfo));

	printf("IP：%s, 端口：%d\n", inet_ntop(AF_INET, &pinfo->addr.sin_addr.s_addr, addrinfo, sizeof(addrinfo)), ntohs(pinfo->addr.sin_port));

	char buf[1024];
	while(1)
	{
		memset(buf, 0, sizeof(buf));
		ret = read(pinfo->fd, buf, sizeof(buf));
		if( ret <= 0)
		{
			printf("no data or client closed\n");
			close(pinfo->fd);
			pinfo->fd =-1;

			//减少一个连接
			t--;

			pthread_exit(NULL);
		}

		printf("len == %d, buf == %s\n",ret, buf);
		
		int a;
		if(strstr(buf, "用户second:"))
		{
			char *tmpword1 = strstr(buf, ":");
			int tmplen = strlen(buf) - strlen(tmpword1);

			char tmpword2[1024] = {0};
			memcpy(tmpword2,tmpword1 + 1, sizeof(buf));
			printf("进入:%s\n",tmpword2);

			char user[64] = "online_usr:";
			strcat(user,tmpword2);

			for(a = 0; a < 128; a++)
			{
				if(sockinfo[a].fd != -1)
				{
					write(sockinfo[a].fd, user, sizeof(user));
				}
			}
			
		}
		else
		{
			int i;
			for(i =0; i<t; i++)
			{
				if(sockinfo[i].fd != -1)
				{
					write(sockinfo[i].fd, buf, ret);
				}
			}
		}
	}
	
	return NULL;	
}

int main()
{	
	
	//创建socket
	int lfd = socket(AF_INET, SOCK_STREAM, 0);	
	if( lfd < 0)
	{
		printf("socket error:%s\n", strerror(lfd));
		return -1;
	}	

	//端口复用
	int opt = 1;
	setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
	//绑定IP，端口
	struct sockaddr_in server_addr;

	server_addr.sin_port = htons(5656);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	int bind_res = bind(lfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if( bind_res < 0)
	{
		printf("bind error:%s\n", strerror(bind_res));
		return -1;
	}
	//监听
	int listen_res = listen(lfd, 128);
	if( listen_res < 0)
	{
		printf("listen error%s\n", strerror(listen_res));
		return -1;
	}
	//初始化信息结构体
	int max = sizeof(sockinfo) / sizeof(sockinfo[0]);
	int j;	
	for(j = 0; j < max; j++)
	{
		bzero(&sockinfo[j], sizeof(sockinfo[j]));
		sockinfo[j].fd = -1;
		sockinfo[j].pth = -1;	
	}
	//等待连接
	int cfd;
	int k;
	pthread_t pth;

	while(1)
	{
		//找出空置的位置存储信息
		struct Sockinfo * pinfo;
		socklen_t len = sizeof(pinfo->addr);
		for(k = 0; k < max; k++)
		{
			if(sockinfo[k].fd == -1)
			{
				pinfo = &sockinfo[k];
				//增加一个连接
		//		t++;
				break;
			}
			if( k == max -1)
			{
				sleep(1);
				k--;
			}
		}
		t++;
		cfd = accept(lfd, (struct sockaddr *)&pinfo->addr, &len);
		if( cfd < 0)
		{
			printf("accept error\n");
			exit(0);
		}	
		
		pinfo->fd = cfd;
		
		int res = pthread_create(&pinfo->pth, NULL, pthreadfunc, pinfo);
		if( res != 0)
		{
			printf("pthread create error:%s\n", strerror(res));
			exit(1);
		}
		
		pthread_detach(pinfo->pth);			

	}

	
	close(lfd);

	return 0;
}

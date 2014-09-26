/*
 * =====================================================================================
 *
 *       Filename:  server.c
 *
 *    Description:  code of server
 *
 *        Version:  1.0
 *        Created:  2014年08月02日 12时25分04秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  pupengfei, 792570367@qq.com
 *        Company:  NONE
 *
 * =====================================================================================
 */

#include "functions.h"

int main(int argc, char *argv[])
{
	int server_fd, client_addr_len;
	int r_len;//接收数据的长度
	int optval = 1;
	int clients = 0;
	int t;
	char r_msg[MSG_MAX_L] , s_msg[MSG_MAX_L];
	char result = 'n';
	char log1[50];
	char real_time[64];
	time_t record_t;
	struct tm *tm_p;
	
	pthread_t pid[LIST_MAX];
	pthread_t pid_show_client;
	struct sockaddr_in server_addr, client_addr;
	USER user[LIST_MAX];
	FILE *fp,*log_fp;
	
	head.next = NULL;
	
	pthread_create(&pid_show_client,NULL,show_clients_server,(void *)&head);
	strcpy(log1,"检测到新的IP:");
	fp = fopen("Clients","a");
	fclose(fp);
	
	client_addr_len = sizeof(client_addr);
	
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		my_err("socket", __LINE__);
	}

	if ((setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&optval, sizeof(int))) < 0)
	{
		my_err("setsockopt", __LINE__);
	}

	memset(&server_addr, 0, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) < 0)
	{
		my_err("bind", __LINE__);
	}
	
	if (listen(server_fd,LIST_MAX) < 0)
	{
		my_err("listen", __LINE__);
	}

	while(clients < LIST_MAX)
	{
	  if ((user[clients].fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len)) < 0)
		{
			my_err("accept", __LINE__);
		}
		log_fp = fopen("SYS_LOG.txt","a");
	  record_t = time(NULL);
	  tm_p = localtime(&record_t);
	  strftime(real_time,sizeof(real_time),"%Y/%m/%d %X %A 本年第%j天",tm_p);
	  printf("Detected New Client,IP:%s\n",inet_ntoa(client_addr.sin_addr));
	  fprintf(log_fp,"%s,%s,%s\n",log1,inet_ntoa(client_addr.sin_addr),real_time);
	  fclose(log_fp);
	  t = user[clients].fd;
	  printf("clients: %d,id:--%d,%s\n",clients,user[clients].fd,real_time);
	 
	  user[clients].fd = t;
	  user[clients].order = &clients;
	  user[clients].result = 'n';
	  pthread_create(&pid[clients],NULL,serve_chat,(void *)&user[clients]);
	  clients++;
	}
	  
	return 0;
}


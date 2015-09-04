#include "defines.h"
#include "module.h"
#include "threadpool.h"
#include "eventmgr.h"
#include "server.h"

int listenfd;

void PoolTask::Run()
{
	if (m_callback){
		m_callback(m_callback_data);
	}
}

void sigchld_handler(int signal_number)
{
	int status;
	wait(&status);
}
/* Handle HTTP GET Command*/
void handle_get(int conn_fd, const char* page)
{
	if (page == NULL)
		return ;
	if (*page == '/' && strchr (page + 1,'/') == NULL) 
	{
		write(conn_fd, ok_response, strlen(ok_response));
		char module_file_name[64];
		snprintf(module_file_name, sizeof(module_file_name),"%s.so", page + 1);
		CModule module(module_file_name,conn_fd);
		module.Execute();
	}
	else
	{
		char response[1024];
		snprintf(response, sizeof(response),not_found_response_template, page);
		write(conn_fd, response, strlen(response));
	}
}
/* Handle client connection */
void handle_connection(int conn_fd)
{
	char buffer[MAX_BUF];
	int  bytes_read;

	bytes_read = read(conn_fd, buffer, sizeof(buffer) - 1);
	if (bytes_read == -1){
		perror("read data error!\n");
		return;
	}
	else if(bytes_read > 0)
	{
		char method[MAX_BUF];
		char url[MAX_BUF];
		char protocol[MAX_BUF];

		buffer[bytes_read] ='\0';
		sscanf (buffer, "%s %s %s", method, url, protocol);
		/* Read data from client until find the end of the header */
		while(strstr(buffer, "\r\n\r\n") == NULL)
			bytes_read = read(conn_fd, buffer, sizeof(buffer));

		if (bytes_read == -1) {
			close(conn_fd);
			return;
		}

		if (strcmp(protocol, "HTTP/1.0") && strcmp(protocol, "HTTP/1.1")) {
			write (conn_fd, bad_request_response,sizeof(bad_request_response));
		}
		else if(strcmp(method, "GET")){
			//Only support GET method
			char response[1024];
			snprintf (response, sizeof(response),bad_method_response_template, method);
			write(conn_fd, response, strlen (response));
		}
		else{
			handle_get(conn_fd, url);
			close(conn_fd);
		} 
	}
}

void start_server(struct in_addr local_addr, short port)
{
	struct sigaction sigchld_action;
	memset(&sigchld_action, 0, sizeof(sigchld_action));
	sigchld_action.sa_handler = &sigchld_handler;
	sigaction(SIGCHLD, &sigchld_action, NULL);

	//create listen socket
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1){
		perror("create listen socket error!\n");
		return;
	}

	int opt = 1;
	if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt))==-1){
		perror("setsockopt error!\n");
		return;
	}
	//init serveraddr
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = port;
	serveraddr.sin_addr = local_addr;

	/* Bind listen socket to the serveraddr*/
	int rval = bind(listenfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (rval != 0){
		perror("bind listenfd error!\n");
		return;
	}

	rval = listen (listenfd, 5);
	if (rval != 0){
		perror("listen error!\n");
		return;
	}

	socklen_t addrlen;
	addrlen = sizeof(serveraddr);
	rval = getsockname(listenfd, (struct sockaddr*)&serveraddr, &addrlen);
	assert(rval == 0);
	printf("server listening on %s:%d\n",inet_ntoa(serveraddr.sin_addr),(int)ntohs(serveraddr.sin_port));

	EventMgr::Instance()->AddEvent(listenfd,EPOLLIN);
	EventMgr::Instance()->SetNonBlock(listenfd);
	EventMgr::Instance()->EventLoop();
}
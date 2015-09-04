#include "defines.h"
#include "eventmgr.h"
#include "threadpool.h"
#include "server.h"

#define MAX_EVENTS 10240

extern int listenfd;
EventMgr* EventMgr::m_Instance = NULL;
int EventMgr::m_epfd = 0;

void EventMgr::SetNonBlock(int fd)
{
	if (fcntl(fd,F_SETFL,fcntl(fd,F_GETFL) | O_NONBLOCK)){
		perror("fcntl error");
	}
}

EventMgr* EventMgr::Instance()
{
	if (!m_Instance){
		m_epfd = epoll_create(16);
		m_Instance = new EventMgr;
	}
	return m_Instance;
}

void EventMgr::AddEvent(int fd, int flags)
{
	epoll_event ev;
	ev.data.fd = fd;
	ev.events = flags;
	if (epoll_ctl(m_epfd,EPOLL_CTL_ADD,fd,&ev) == -1){
		perror("epoll ctl error\n");
		exit(0);
	}
}

void EventMgr::ModifyEvent(int fd, int flags)
{
	epoll_event ev;
	ev.data.fd = fd;
	ev.events = flags;
	if (epoll_ctl(m_epfd,EPOLL_CTL_MOD,fd,&ev) == -1){
		perror("epoll ctl mod error\n");
		exit(0);
	}
}

void EventMgr::RemoveEvent(int fd)
{
	if (epoll_ctl(m_epfd,EPOLL_CTL_DEL,fd,0) == -1){
		perror("epoll ctl remove error\n");
		exit(0);
	}
}

void EventMgr::EventLoop()
{
	ThreadPool pool(5,true);
	pool.PoolStart();

	while(true)
	{
		struct epoll_event events[MAX_EVENTS];
		int nfds = epoll_wait(m_epfd,events,MAX_EVENTS,-1);
		for (int i = 0; i < nfds; ++i)
		{
			if (events[i].data.fd == listenfd){
				struct sockaddr_in peer_addr;
				socklen_t addr_len = 0;
				memset(&peer_addr,0,sizeof(peer_addr));
				int connfd;
				if((connfd = accept(listenfd,(struct sockaddr*)&peer_addr,&addr_len)) == -1){
					perror("accept error\n");
				}
				fprintf(stdout,"get a connection from %s:%d\n",inet_ntoa(peer_addr.sin_addr),ntohs(peer_addr.sin_port));
				AddEvent(connfd,EPOLLIN);
			}else if(events[i].events & EPOLLIN){
				int connfd = events[i].data.fd;
				PoolTask *pTask = new PoolTask();
				pTask->SetCallbackFun((callback_t)handle_connection);
				pTask->SetCallbackData((void*)connfd);
				pool.PushTask(pTask);
				RemoveEvent(connfd);
			}else{
				continue;
			}
		}
	}
}
#ifndef EVENT_MGR_H
#define EVENT_MGR_H

class EventMgr
{
public:
	static void SetNonBlock(int fd);
    static void AddEvent(int fd, int flags);
	static void RemoveEvent(int fd);
	static void ModifyEvent(int fd, int flags);
	static void EventLoop();
	static EventMgr* Instance();
private:
	EventMgr(){}
	~EventMgr(){}
	static int m_epfd;
	static EventMgr* m_Instance;
};

#endif // EVENT_MGR_H

#ifndef SERVER_H
#define SERVER_H

#include "threadpool.h"

static char* ok_response =
"HTTP/1.0 200 OK\n"
"Content-type: text/html\n"
"\n";

static char* bad_request_response = 
"HTTP/1.0 400 Bad Request\n"
"Content-type: text/html\n"
"\n"
"<html>\n"
" <body>\n"
"  <h1>Bad Request</h1>\n"
"  <p>This server did not understand your request.</p>\n"
" </body>\n"
"</html>\n";

static char* not_found_response_template = 
"HTTP/1.0 404 Not Found\n"
"Content-type: text/html\n"
"\n"
"<html>\n"
" <body>\n"
"  <h1>Not Found</h1>\n"
"  <p>The requested URL %s was not found on this server.</p>\n"
" </body>\n"
"</html>\n";

static char* bad_method_response_template = 
"HTTP/1.0 501 Method Not Implemented\n"
"Content-type: text/html\n"
"\n"
"<html>\n"
" <body>\n"
"  <h1>Method Not Implemented</h1>\n"
"  <p>The method %s is not implemented by this server.</p>\n"
" </body>\n"
"</html>\n";

extern int listenfd;
void handle_connection(int conn_fd);

typedef void (*callback_t)(void*);
class PoolTask : public Task
{
public:
    PoolTask(){}
    virtual ~PoolTask(){}
    void SetCallbackFun(callback_t callback){m_callback = callback;}
    void SetCallbackData(void* data){m_callback_data = data;}
    void Run();
private:
	callback_t m_callback;
	void* 	   m_callback_data;
};

#endif // SERVER_H

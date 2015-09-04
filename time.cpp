#include "defines.h"
#include "server.h"

static char* page_time =
"<html>\n"
" <head>\n"
"  <meta http-equiv=\"refresh\" content=\"5\">\n"
" </head>\n"
" <body>\n"
"  <p>The current time is %s.</p>\n"
" </body>\n"
"</html>\n";

extern "C"
{
	void module_generate_page(int fd)
	{
		struct timeval tv;
		gettimeofday(&tv, NULL);
		struct tm* ptm = localtime(&tv.tv_sec);

		char time_str[32];
		strftime(time_str, sizeof(time_str),"%H:%M:%S", ptm);

		FILE* fp = fdopen(fd, "w");
			assert(fp != NULL);
		fprintf(fp, page_time, time_str);
		fflush(fp);
	}	
}

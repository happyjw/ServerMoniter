#include "defines.h"
#include "server.h"

static char* page_start = "<html>\n <body>\n <pre>\n";
static char* page_end = "</pre>\n </body>\n </html>\n";

extern "C"
{
void module_generate_page(int fd)
{
	write(fd, page_start, strlen(page_start));

	int ret;
	pid_t child_pid = fork ();

	if (child_pid == 0) 
	{
		char* argv[] = {"/bin/df","-h",NULL};
		ret = dup2(fd, STDOUT_FILENO);
		if (ret == -1){
			perror("dup2");
			return;
		}

		ret = dup2(fd, STDERR_FILENO);
		if (ret == -1){
			perror("dup2");
			return;
		}
		if(execv(argv[0], argv) == -1){
			perror("execv");
			return;
		}
	}
	else if (child_pid > 0) {
		ret = waitpid(child_pid, NULL, 0);
		if (ret == -1){
			perror("waitpid");
			return;
		}
	}
	else{
		perror("fork error!\n");
		return;
	}
	write(fd, page_end, strlen (page_end));
}
}

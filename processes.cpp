#include "defines.h"
#include "server.h"

static int get_uid_gid(pid_t pid, uid_t* uid, gid_t* gid)
{
	char dir_name[64];
	struct stat dir_info;
	snprintf(dir_name, sizeof(dir_name), "/proc/%d", (int)pid);
	int rval = stat(dir_name, &dir_info);
	if (rval != 0) 
		return 1;
	assert(S_ISDIR(dir_info.st_mode));
	*uid = dir_info.st_uid;
	*gid = dir_info.st_gid;
	return 0;
}

static char* get_user_name(uid_t uid)
{
	struct passwd* entry;
	entry = getpwuid(uid);
	if (entry == NULL)
		perror("getpwuid");
	return strdup(entry->pw_name);
}

static char* get_group_name(gid_t gid)
{
	struct group* entry = getgrgid (gid);
	if (entry == NULL)
		perror("getgrgid");
	return strdup(entry->gr_name);
}

static char* get_program_name(pid_t pid)
{
	char file_name[64];
	snprintf(file_name, sizeof(file_name), "/proc/%d/stat", (int)pid);
	int fd = open (file_name, O_RDONLY);
	if (fd == -1)
		return NULL;

	char status_info[256];
	int rval = read(fd, status_info, sizeof(status_info) - 1);
	close(fd);
	if (rval <= 0)
		return NULL;
	status_info[rval] = '\0';

	char* front = strchr(status_info,'(');
	char* back  = strchr(status_info,')');
	if (front == NULL||back == NULL||back < front)
		return NULL;

	char* result = (char*)malloc(back - front);
	strncpy(result, front + 1, back - front - 1);
	result[back - front - 1] ='\0';
	return result;
}

static int get_rss(pid_t pid)
{
	char file_name[64];
	snprintf(file_name, sizeof(file_name), "/proc/%d/statm", (int)pid);

	int fd = open(file_name, O_RDONLY);
	if (fd == -1)
		return -1;

	char mem_info[128];
	int rval = read(fd, mem_info, sizeof(mem_info) - 1);
	close(fd);
	if (rval <= 0)
		return -1;
	mem_info[rval] ='\0';

	int rss;
	rval = sscanf(mem_info, "%*d %d", &rss);
	if (rval != 1)
		return -1;

	return rss * getpagesize() / 1024;
}

static char* format_process_info(pid_t pid)
{
	uid_t uid;
	gid_t gid;	
	int rval = get_uid_gid(pid, &uid, &gid);
	if (rval != 0)
		return NULL;

	int rss = get_rss(pid);
	if (rss == -1)
		return NULL;

	char* program_name = get_program_name(pid);
	if (program_name == NULL)
		return NULL;

	char* user_name  = get_user_name(uid);
	char* group_name = get_group_name(gid);

	int   res_len = strlen(program_name) + strlen(user_name) + strlen(group_name) + 128;
	char* result = (char*)malloc(res_len);

	snprintf(result, res_len,
	"<tr><td align=\"right\">%d</td><td><tt>%s</tt></td><td>%s</td>"
	"<td>%s</td><td align=\"right\">%d</td></tr>\n",
	(int)pid, program_name, user_name, group_name, rss);

	free(program_name);
	free(user_name);
	free(group_name);
	return result;
}

static char* page_start = 
"<html>\n"
" <body>\n"
"  <table cellpadding=\"4\" cellspacing=\"0\" border=\"1\">\n"
"   <thead>\n"
"    <tr>\n"
"     <th>PID</th>\n"
"     <th>Program</th>\n"
"     <th>User</th>\n"
"     <th>Group</th>\n"
"     <th>RSS&nbsp;(KB)</th>\n"
"    </tr>\n"
"   </thead>\n"
"   <tbody>\n";
/* HTML source for the end of the process listing page.  */
static char* page_end =
"   </tbody>\n"
"  </table>\n"
" </body>\n"
"</html>\n";

extern "C"
{
void module_generate_page (int fd)
{
	size_t vec_size = 16;
	struct iovec* vec = (struct iovec*)malloc(vec_size * sizeof(struct iovec));

	int vec_length = 0;
	vec[vec_length].iov_base = page_start;
	vec[vec_length].iov_len = strlen (page_start);
	++vec_length;

	DIR* proc_listing = opendir("/proc");
	if(proc_listing == NULL){
		perror("opendir");
		return;
	}

	while (1) 
	{
		char* process_info;
		struct dirent* proc_entry  = readdir (proc_listing);
		if (proc_entry == NULL)
			break;

		const char* name = proc_entry->d_name;
		if (strspn(name, "0123456789") != strlen (name))
			continue;

		pid_t pid = (pid_t)atoi(name);

		process_info = format_process_info(pid);
		if (process_info == NULL)

		process_info = "<tr><td colspan=\"5\">ERROR</td></tr>";

		if (vec_length == vec_size - 1) {
			vec_size *= 2;
			vec = (struct iovec*)realloc(vec, vec_size * sizeof(struct iovec));
		}

		vec[vec_length].iov_base = process_info;
		vec[vec_length].iov_len = strlen(process_info);
		++vec_length;
	}

	closedir(proc_listing);

	vec[vec_length].iov_base = page_end;
	vec[vec_length].iov_len = strlen (page_end);
	++vec_length;

	writev (fd, vec, vec_length);
	for (int i = 1; i < vec_length - 1; ++i)
		free(vec[i].iov_base);

	free(vec);
}
}
#include "defines.h"
#include "eventmgr.h"
#include "module.h"

#define  DEFAULT_PORT 10240
static const struct option long_options[] = {
{ "address",    1, NULL, 'a'},
{ "help",       0, NULL, 'h'},
{ "port",       1, NULL, 'p'},
};

static const char* const short_options = "a:h:p";

static const char* const usage_template = 
"Usage: %s [ options ]\n"
"  -a, --address ADDR        Bind to local address (by default, bind\n"
"                              to all local addresses).\n"
"  -h, --help                Print this information.\n"
"                              (by default, use executable directory).\n"
"  -p, --port PORT           Bind to specified port.\n";

char *program_name;

static void print_usage(int is_error)
{
	fprintf(is_error ? stderr : stdout, usage_template, program_name);
	exit(is_error ? 1 : 0);
}

int main(int argc, char* const argv[])
{
	struct in_addr local_addr;
	unsigned short port;
	int next_opt;

	program_name = argv[0];
	local_addr.s_addr = INADDR_ANY;
	port = DEFAULT_PORT;

	do 
	{
		next_opt = getopt_long(argc, argv, short_options, long_options, NULL);
		switch (next_opt) 
		{
			case 'a':
			{
				struct hostent* local_host_name;
				local_host_name = gethostbyname (optarg);
				if (local_host_name == NULL  ||local_host_name->h_length == 0){
					fprintf(stderr, "%s\n", "Invalid host name!\n");
					return;
				}
				else
					local_addr.s_addr = *((int*)(local_host_name->h_addr_list[0]));
			}
			break;      
			case 'h':  
				print_usage(0);
			break;
			case 'p':
			{
				long value;
				char* end;
				value = strtol(optarg, &end, 10);
				if (*end != '\0')
					print_usage(1);
				port = (unsigned short)htons(value);
			}
			break;
			case '?':  
				print_usage (1);
		}
	} while (next_opt != -1);

	if(optind != argc)
		print_usage(1);
	start_server(local_addr, port);
	return 0;
}
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <mqueue.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>

#define BUF_SIZE 8192
char buf[BUF_SIZE];

void check_it(int result, int line)
{
	if(result == -1)
	{
		fprintf(stderr, "(%s, %d): ", __FILE__, line-1);
		exit(errno);
	}
}

void sig_handler(int signo)
{
	if (signo == SIGINT)
		{
			printf("\nServer going down...\n");
			int rc;
			mqd_t server_queue;

			server_queue = mq_open("/server_queue", O_CREAT | O_RDWR, 0666, NULL);
			check_it(server_queue, __LINE__);
			rc = mq_close(server_queue);
			check_it(rc, __LINE__);
			rc = mq_unlink("/server_queue");
			check_it(rc, __LINE__);
			exit(0);
		}
}

int main(int argc, char **argv)
{
	if (argc == 2)
	{
		if (strcmp(argv[1], "server") == 0)
		{
			if (signal(SIGINT, sig_handler) == SIG_ERR)
				printf("Nope\n");

			mqd_t server_queue;
			mqd_t client_queue;
			int rc;

			printf("Server side\n");

			server_queue = mq_open("/server_queue", O_CREAT | O_RDWR, 0666, NULL);
			check_it(server_queue, __LINE__);

			while(1)
			{
				rc = mq_receive(server_queue, buf, BUF_SIZE, NULL);
				check_it(rc, __LINE__);
				int token = rand();
				char token_message[20];
				sprintf(token_message, "%d", token);
				client_queue = mq_open(buf, O_CREAT | O_RDWR, 0666, NULL);
				check_it(client_queue, __LINE__);
				rc = mq_send(client_queue, token_message, strlen(token_message), 0);
				check_it(rc, __LINE__);
			}

			rc = mq_close(server_queue);
			check_it(rc, __LINE__);
			rc = mq_unlink("/server_queue");
			check_it(rc, __LINE__);
			return 0;
		}
		if (strcmp(argv[1], "client") == 0)
		{
			mqd_t client_queue;
			mqd_t server_queue;
			int rc;
			char message_queue_name[50];
			sprintf(message_queue_name, "%s_%d", "/client_queue", getpid()); // format pid of current client into message (string)

			printf("For client with pid %d", getpid());

			server_queue = mq_open("/server_queue", O_CREAT | O_RDWR, 0666, NULL);
			check_it(server_queue, __LINE__);
			client_queue = mq_open((const char *)message_queue_name, O_CREAT | O_RDWR, 0666, NULL);
			check_it(client_queue, __LINE__);

			rc = mq_send(server_queue, message_queue_name, strlen(message_queue_name), 0);
			check_it(rc, __LINE__);
			//wait back for token receiving
			rc = mq_receive(client_queue, buf, BUF_SIZE, NULL);
			check_it(rc, __LINE__);
			printf(" token is: %s\n", buf);

			rc = mq_close(client_queue);
			check_it(rc, __LINE__);
			rc = mq_unlink((const char *)message_queue_name);
			check_it(rc, __LINE__);

			return 0;
		}
	}
	printf("No client or server specification!\n");
	return 0;
}

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/wait.h>
#include <stdarg.h>

#define THREAD_COUNT	100

#define IS_DEBUG		0
#define DEBUG(fmt, ...)	if (IS_DEBUG) { fprintf(stdout, fmt, ## __VA_ARGS__); fflush(stdout); }

void* jail(void*);
int my_func(const char *format, ...);

int main()
{
	pthread_t	tid[THREAD_COUNT];
	uint8_t		i;
	void*		result;

	DEBUG("Main Method\n");

	for (i = 0; i < THREAD_COUNT; i++) {
		uint8_t *arg = malloc(sizeof(*arg));
		*arg = i;
		pthread_create(&tid[i], NULL, (void*)&jail, arg);
	}

	for (i = 0; i < THREAD_COUNT; i++) {
		pthread_join(tid[i], &result);
		printf("Thread %d returning: %s\n", i, (char*)result);
	}

	return EXIT_SUCCESS;
}

void* jail(void *arg)
{
	uint8_t	id = *((uint8_t*) arg);
	pid_t	fpid;
	int		pfd[2];

	ssize_t nbytes;
	uint16_t len = 8192;
	unsigned char *result = (unsigned char*) malloc(sizeof(*result) * 2);
	*result = '\0';
	unsigned char *buffer = (unsigned char*) malloc(sizeof(*buffer) * len);

	DEBUG("Thread %d\n", id);

	pipe(pfd);
	fpid = fork();

	DEBUG("Fork PID: %d\n", fpid);

	if (fpid >= 0) {
		if (0 == fpid) {
			close(pfd[0]);
			int moutfd = dup(STDOUT_FILENO);
			// @TODO: Have file descriptor in memory region
			FILE *outfh = tmpfile();
			dup2(fileno(outfh), STDOUT_FILENO);

			// Call the blind function
			my_func("Hello this is jail #%d written on %d instead of %d-%d-%d\n", id, fileno(outfh), moutfd, STDOUT_FILENO, fileno(stdout));

			dup2(moutfd, STDOUT_FILENO);
			close(moutfd);

			fseek(outfh, 0, SEEK_SET);
			while (!feof(outfh)) {
				nbytes = fread(buffer, 1, len, outfh);

				if (read > 0) {
					result = realloc(result, (sizeof(*result) * (strlen(result) + 1)) + nbytes);
					strncat(result, buffer, nbytes);
				}
			}
			fclose(outfh);

			write(pfd[1], result, sizeof(*result) * (strlen(result) + 1));
			close(pfd[1]);

			exit (EXIT_SUCCESS);
		} else {
			close(pfd[1]);

			DEBUG("Parent Process (%d)\n", id);

			while (nbytes = read(pfd[0], buffer, len)) {
				result = realloc(result, (sizeof(*result) * (strlen(result) + 1)) + nbytes);
				strncat(result, buffer, nbytes);
			}

			// wait(NULL);

			return (void*)result;
		}
	} else {
		perror("Unable to fork");
	}

	return NULL;
}

/**
 *	Poor function which is printing on stdout
 */
int my_func(const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	int result = vfprintf(stdout, format, ap);
	va_end(ap);

	fflush(stdout);
	sleep(1);

	return result;
}


// gcc ./daemon1.c -o ./kb_daemon.out
// sudo ./kb_daemon.out
// sudo killall kb_daemon.out

// cat /var/log/keyboard.stats

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <time.h>

#define STATISTICS_PATH 		"/var/log/keyboard.stats"
#define KEYBOARD_EVENTS_PATH	"/dev/input/by-path/platform-i8042-serio-0-event-kbd"
#define REFRESH_TIME 10

struct Event 
{
	struct timeval time;
	unsigned short type;
	unsigned short code;
	unsigned int value;
};

#define Event struct Event
#define EVENT_SIZE sizeof(Event)

int main(void) 
{
	pid_t pid, sid;
	pid = fork();
	if (pid < 0) 
	{
	    exit(EXIT_FAILURE);
	}
	if (pid > 0) 
	{
	    exit(EXIT_SUCCESS);
	}

	umask(0);
	
	sid = setsid();
	if (sid < 0) 
	{
		exit(EXIT_FAILURE);
	}

	if ((chdir("/")) < 0) 
	{
		exit(EXIT_FAILURE);
	}

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	long key_count = 0;
	size_t readEventSize;
	FILE *eventsFile = fopen(KEYBOARD_EVENTS_PATH, "r");
	if (eventsFile == NULL) 
		exit(EXIT_FAILURE);

	FILE *statsFile = NULL;
	Event *Kb_Event = (Event *) malloc(EVENT_SIZE);
	time_t Start_Time = time(NULL);
	time_t currentTime;
	while (1) 
	{		
		readEventSize = fread(Kb_Event, EVENT_SIZE, 1, eventsFile);
		if ((Kb_Event->type == 1) && (Kb_Event->value == 1)) 
			key_count += 1;
		currentTime = time(NULL);
		if (currentTime - Start_Time > REFRESH_TIME) 
		{
			statsFile = fopen(STATISTICS_PATH, "w");

			fprintf(statsFile, "KEY COUNT: %ld\n", key_count);

			Start_Time = currentTime;
			key_count = 0;
			fclose(statsFile);
		}
	}
	fclose(eventsFile);
	free(Kb_Event);
	exit(EXIT_SUCCESS);
}

/* See LICENSE for license details */
#define _POSIX_C_SOURCE 199309L

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define USAGE \
    "Usage: %s TIME REPEAT COMMAND\n\n" \
    "TIME: the intended time to run COMMAND\n" \
    "Format (24h): HOUR:MINUTE:SECOND or\n" \
    "              HOUR:MINUTE        or\n" \
    "              HOUR\n\n" \
    "COMMAND: the shell command to run at TIME\n" \
    "Example: aplay alarm-tune.wav\n\n" \
    "REPEAT: o[nce]|l[oop]\n"

int
main(int argc, char *argv[])
{
	struct tm *currtime;
	struct timespec ts;
	time_t ctime;
	int targettime[3], /* [0]: hour, [1]: minute, [2]: second */
        idx;
	int hinterval = 0, minterval = 0, sinterval = 0;
	char *token;

	if (argc != 4) {
		fprintf(stderr, USAGE, argv[0]);

		return 1 << 0;
	}

	memset(targettime, 0, sizeof(targettime));

	idx = 0;
	token = strtok(argv[1], ":");

	while (token != NULL) {
		if (idx > 2)
			break;

		targettime[idx] = strtol(token, NULL, 10);

		if (idx == 0) {
			if (targettime[idx] > 23 || targettime[idx] < 0) {
				fprintf(stderr, "ERROR: Invalid value for HOUR!\n\n");
				fprintf(stderr, USAGE, argv[0]);

				return 1 << 1;
			}
		} else {
			if (targettime[idx] > 59 || targettime[idx] < 0) {
				if (idx == 1) {
					fprintf(stderr, "ERROR: Invalid value for MINUTE!\n\n");
					fprintf(stderr, USAGE, argv[0]);
					return 1 << 2;
				} else {
					fprintf(stderr, "ERROR: Invalid value for SECOND!\n\n");
					fprintf(stderr, USAGE, argv[0]);
					return 1 << 3;
				}
			}
		}

		idx++;
		token = strtok(NULL, ":");
	}

	time(&ctime);
	currtime = localtime(&ctime);

	if ((sinterval = targettime[2] - currtime->tm_sec) < 0) {
		sinterval += 60;
		minterval--;
	}

	if ((minterval += targettime[1] - currtime->tm_min) < 0) {
		minterval += 60;
		hinterval--;
	}

	if ((hinterval += targettime[0] - currtime->tm_hour) < 0) {
		hinterval += 24;
	}

	ts.tv_sec = hinterval * 3600 + minterval * 60 + sinterval;
	ts.tv_nsec = 0;

	if (tolower(argv[2][0]) == 'o') {
		if (nanosleep(&ts, NULL) == 0)
			system(argv[3]);
	} else if (tolower(argv[2][0]) == 'l') {
		while (1) {
			if (nanosleep(&ts, NULL) == 0)
				system(argv[3]);

			ts.tv_sec = 86400;
			ts.tv_nsec = 0;
		}
	} else {
		fprintf(stderr, "ERROR: Invalid REPEAT argument!\n\n");
		fprintf(stderr, USAGE, argv[0]);

		return 1 << 4;
	}

	return 0;
}

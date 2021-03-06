/* Gpio.cpp
 *
 * This file modified according to :
 * Raspberry Pi GPIO example using sysfs interface.
 * Guillermo A. Amaral B. <g@maral.me>
 *
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include "Gpio.h"

#define IN  0
#define OUT 1

#define LOW  0
#define HIGH 1

Gpio::Gpio(int pin) : mPin(pin), mIsExported(-1), mDirection(-1), mEdge(-1), mIsActiveLow(-1), mError(0) {
}

Gpio::~Gpio() {
    unexportGpio();
}

Gpio::Gpio() {}
Gpio::Gpio(const Gpio&) {}

#define BUFFER_MAX 3
int Gpio::exportGpio() {
    if (mIsExported == 1) {
        fprintf(stderr, "Already exported\n"); 
        return 0;
    }
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;

	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open export for writing!\n");
		return(-1);
	}

	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", mPin);
	::write(fd, buffer, bytes_written);
	close(fd);
	mIsExported = 1;
	return(0);
}

int Gpio::unexportGpio() {
    if (mIsExported != 1) {
        fprintf(stderr, "Need export first\n"); 
        return -1;
    }

	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open unexport for writing!\n");
		return(-1);
	}

	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", mPin);
	::write(fd, buffer, bytes_written);
	close(fd);
	mIsExported = 0;
	return(0);
}

#define DIRECTION_MAX 35
int Gpio::getDirection() {
    if (mIsExported != 1) {
        fprintf(stderr, "Need export first\n"); 
        return -1;
    }
	static char buffer[4] = "";
	char path[DIRECTION_MAX];
	int fd;

	snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", mPin);
	fd = open(path, O_RDONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio direction for writing!\n");
		return(-1);
	}

    int len = ::read(fd, buffer, 4);
	if (-1 == len) {
		fprintf(stderr, "Failed to set direction!\n");
		return(-1);
	}
	close(fd);

	buffer[len] = '\0';

    if (!strncmp("in", buffer, len))
        mDirection = IN;
    else if (!strncmp("out", buffer, len))
        mDirection = OUT;
    else
        mDirection = -1;

	return mDirection;
}

int Gpio::setDirection(int dir) {
    if (mIsExported != 1) {
        fprintf(stderr, "Need export first\n"); 
        return -1;
    }
    mDirection = -1;
	static const char s_directions_str[]  = "in\0out";
	char path[DIRECTION_MAX];
	int fd;

	snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", mPin);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio direction for writing!\n");
		return(-1);
	}

	if (-1 == ::write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3)) {
		fprintf(stderr, "Failed to set direction!\n");
		return(-1);
	}

	close(fd);
	mDirection = dir;
	return(0);
}

int Gpio::getEdgeTrigger() {
fprintf(stderr, "Not implemented\n"); 
return 0;
}

int Gpio::setEdgeTrigger(int trigger) {
fprintf(stderr, "Not implemented\n"); 
return -1;
}

int Gpio::getActiveLow() {
fprintf(stderr, "Not implemented\n"); 
return 0;
}

int Gpio::setActiveLow(int low) {
fprintf(stderr, "Not implemented\n"); 
return -1;
}

int Gpio::read() {
#define VALUE_MAX 30
    if (mIsExported != 1) {
        fprintf(stderr, "Need export first\n"); 
        return -1;
    }
	char path[VALUE_MAX];
	char value_str[3];
	int fd;

	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", mPin);
	fd = open(path, O_RDONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio value for reading!\n");
		return(-1);
	}

	if (-1 == ::read(fd, value_str, 3)) {
		fprintf(stderr, "Failed to read value!\n");
		return(-1);
	}

	close(fd);

	return(atoi(value_str));
}

int Gpio::write(int value) {
    if (mIsExported != 1) {
        fprintf(stderr, "Need export first\n"); 
        return -1;
    }
	static const char s_values_str[] = "01";

	char path[VALUE_MAX];
	int fd;

	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", mPin);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio value for writing!\n");
		return(-1);
	}

	if (1 != ::write(fd, &s_values_str[LOW == value ? 0 : 1], 1)) {
		fprintf(stderr, "Failed to write value!\n");
		return(-1);
	}

	close(fd);
	return(0);
}

int gGpios = 0;
int gBase = 0;
Gpio ** gGpioClasses = NULL;

int readStringFromN(char * in, size_t size, const char * path) {
	int fd;
    if (in == NULL || path == NULL)
        return -1;

	fd = open(path, O_RDONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open path %s. (%d) %s\n", path, errno, strerror(errno));
		return(errno);
	}
    
    int len = ::read(fd, in, size);
	if (-1 == len) {
		fprintf(stderr, "Failed to read. (%d) %s\n", errno, strerror(errno));
		return(errno);
	}
	close(fd);
    return 0;
}

void finishGpios() {
	printf("Reset all gpios\n");
    if (gGpios != 0) {
        if (gGpioClasses == NULL)
            return;
        for (size_t i = 0; i < gGpios; i++) {
            if (gGpioClasses[i] != NULL) {
                delete gGpioClasses[i];
                gGpioClasses[i] = 0;
            }
        }
        delete [] gGpioClasses;
        gGpioClasses = NULL;
    }
}

static void sigproc(int dunno) {
	switch (dunno) {
	case SIGHUP:
		printf("Get a signal -- SIGHUP\n");
		break;
	case SIGINT:
		printf("Get a signal -- SIGINT\n");
		break;
	case SIGQUIT:
		printf("Get a signal -- SIGQUIT\n");
		break;
	case SIGKILL:
		printf("Get a signal -- SIGKILL\n");
		break;
	case SIGTERM:
		printf("Get a signal -- SIGTERM\n");
		break;
	case SIGPIPE:
		printf("Get a signal -- SIGPIPE\n");
		break;
	}
	finishGpios();
	exit(0);
	return;
}

int initChip() {
	char buffer[5];
    size_t bSize = sizeof(buffer);
	const char ngpioPath[] = "/sys/class/gpio/gpiochip0/ngpio";
	const char basePath[] = "/sys/class/gpio/gpiochip0/base";
	int ret;
    if (ret = readStringFromN(buffer, bSize, ngpioPath))
        return ret;
    errno = 0;
    gGpios = atoi(buffer);
    if (errno) goto error;

    if (ret = readStringFromN(buffer, bSize, basePath))
        exit(ret);

    errno = 0;
    gBase = atoi(buffer);
    if (errno) goto error;

    printf("Chip 0 has %d gpios. It's base is %d\n", gGpios, gBase);

    if (gGpios != 0) {
        gGpioClasses = new Gpio*[gGpios];
        for (size_t i = 0; i < gGpios; i++) {
            gGpioClasses[i] = NULL;
        }
    }

	signal(SIGHUP, sigproc);
	signal(SIGINT, sigproc);
	signal(SIGQUIT, sigproc);
	signal(SIGKILL, sigproc);
	signal(SIGTERM, sigproc);
	signal(SIGPIPE, sigproc);
    return 0;

error:
    fprintf(stderr, "Failed to parse number\n");
    gGpios = 0;
    gBase = 0;
    return errno;
}

Gpio* Gpio::getInstance(int pin) {
    if (pin < 0 || pin >= gGpios) {
        fprintf(stderr, "Invalide gpio pin.\n");
        return NULL;
    }
    if (gGpioClasses == NULL) {
        fprintf(stderr, "No any gpio.\n");
        exit(-1);
    }
    if (gGpioClasses[pin] == NULL)
        gGpioClasses[pin] = new Gpio(pin);
    return gGpioClasses[pin];
}



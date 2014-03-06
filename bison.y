%{
#include <stdio.h>
#include <vector>
#include <string.h>
#include <errno.h>
#include "ATS.h"
#include "bison.hpp"
#include "Gpio.h"

#define DD(args...) printf(args)

extern int yylex(void);
extern void yyerror(char *);

char * gText;
double gFloat;
int gInteger;
bool gBool;
bool gEnableNewline = false;
extern int yylineno;
extern char * yytext;
const char * gcondition;
void yyerror(const char *message) {
    fprintf(stderr, "%d: error: '%s' at '%s'\n", yylineno, message, yytext);
}
void usage();

int yylex(void);

%}

%token GPIO GPIO_PIN EXPORT UNEXPORT READ WRITE BOOLEAN DIRECTION EDGE TRIGGER ACTIVE_LOW HELP

%verbose

%%

commands:
    commands command
    | command
    ;

command:
    gpio
    | gpio_command
    | runCmd
    ;

runCmd:
    '\n'
    | ';'
    ;

gpio:
    GPIO runCmd {usage();}
    ;

gpio_command:
    GPIO_PIN runCmd {
        Gpio* gpio = Gpio::getInstance($1.pin);
        bool isExported = gpio->getIsExported();
        int dir = gpio->getDir();
        int edge = gpio->getEdge();
        printf("gpio%d status\n", $1.pin);
        printf("  Is exported? %d\n", isExported);
        if (isExported) {
            if (dir != Gpio::UNKNOWN)
                printf("  Direction: %s\n", dir == Gpio::IN ? "IN" : "OUT");
            else
                printf("  Direction: UNKNOWN\n");
            if (dir == Gpio::IN)
                printf("  Edge: %s\n", gpio->getEdgeName(edge));
        }
    }
    | GPIO_PIN EXPORT runCmd {
        printf("gpio%d exported\n", $1.pin);
        Gpio* gpio = Gpio::getInstance($1.pin);
        int ret = gpio->exportGpio();
        printf("export return %d\n", ret);
    }
    | GPIO_PIN UNEXPORT runCmd {
        printf("gpio%d unexported\n", $1.pin);
        Gpio* gpio = Gpio::getInstance($1.pin);
        int ret = gpio->unexportGpio();
        printf("unexport return %d\n", ret);
    }
    | GPIO_PIN READ runCmd {
        Gpio* gpio = Gpio::getInstance($1.pin);
        int ret = gpio->read();
        printf("read return %d\n", ret);
    }
    | GPIO_PIN WRITE BOOLEAN runCmd {
        Gpio* gpio = Gpio::getInstance($1.pin);
        int ret = gpio->write($3.boolean);
        printf("write return %d\n", ret);
    }
    | GPIO_PIN DIRECTION runCmd {
        Gpio* gpio = Gpio::getInstance($1.pin);
        int dir = gpio->getDirection();
        if (dir != Gpio::UNKNOWN)
            printf("gpio%d direction is %s\n", $1.pin, dir == Gpio::IN ? "in" : "out");
    }
    | GPIO_PIN DIRECTION BOOLEAN runCmd {
        Gpio* gpio = Gpio::getInstance($1.pin);
        int ret = gpio->setDirection($3.boolean);
        printf("setDirection return %d\n", ret);
    }
    | GPIO_PIN EDGE runCmd {printf("gpio%d edge is\n", $1.pin);}
    | GPIO_PIN EDGE TRIGGER runCmd {printf("gpio%d edge %s\n", $1.pin, $3.str);}
    | GPIO_PIN ACTIVE_LOW runCmd {printf("gpio%d active_low is \n", $1.pin);}
    | GPIO_PIN ACTIVE_LOW BOOLEAN runCmd {printf("gpio%d active_low %d\n", $1.pin, $3.boolean);}
    ;

%%

void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}

void usage() {
    printf("\n");
}

int main(void) {
    int ret;
    usage();
    if (ret = initChip() < 0)
        return ret;
    yyparse();
    finishGpios();
    return 0;
}


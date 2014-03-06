%{
#include <stdio.h>
#include <vector>
#include <string.h>
#include "ATS.h"
#include "bison.hpp"

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
    GPIO_PIN runCmd {printf("gpio%d status\n", $1.pin);}
    | GPIO_PIN EXPORT runCmd {printf("gpio%d exported\n", $1.pin);}
    | GPIO_PIN UNEXPORT runCmd {printf("gpio%d unexported\n", $1.pin);}
    | GPIO_PIN READ runCmd {printf("gpio%d read is\n", $1.pin);}
    | GPIO_PIN WRITE BOOLEAN runCmd {printf("gpio%d write %d\n", $1.pin, $3.boolean);}
    | GPIO_PIN DIRECTION runCmd {printf("gpio%d direction is\n", $1.pin);}
    | GPIO_PIN DIRECTION BOOLEAN runCmd {printf("gpio%d direction %d\n", $1.pin, $3.boolean);}
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

void init() {
}

int main(void) {
    usage();
    init();
    yyparse();
    return 0;
}


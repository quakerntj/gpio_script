%{
#include <stdlib.h>
#include "ATS.h"
#include "bison.hpp"
void yyerror(char *);

extern char * gText;
extern double gFloat;
extern int gInteger;
extern bool gBool;
extern bool gEnableSpace;
extern const char * gcondition;

%}

whitespace  ([ \t\n\r]+)

%%

gpio[0-9]+ {
    char * ptr = yytext + 4; // skip gpio
    yylval.pin = atoi(ptr);
    return GPIO_PIN;
}

gpio {
    return GPIO;
}

export {
    return EXPORT;
}

unexport {
    return UNEXPORT;
}

read {
    return READ;
}

write {
    return WRITE;
}

false {
    yylval.boolean = false;
    return BOOLEAN;
}

true {
    yylval.boolean = true;
    return BOOLEAN;
}

0 {
    yylval.boolean = false;
    return BOOLEAN;
}

1 {
    yylval.boolean = true;
    return BOOLEAN;
}

dir {
    return DIRECTION;
}

in {
    yylval.boolean = false;
    return BOOLEAN;
}

out {
    yylval.boolean = true;
    return BOOLEAN;
}

edge {
    return EDGE;
}

none {
    yylval.str = yytext;
    return TRIGGER;
}

rising {
    yylval.str = yytext;
    return TRIGGER;
}

falling {
    yylval.str = yytext;
    return TRIGGER;
}

both {
    yylval.str = yytext;
    return TRIGGER;
}

active_low {
    return ACTIVE_LOW;
}

\n {
    return '\n';
}

; {
    return ';';
}

help {
    return HELP;
}

\? {
    return HELP;
}

    /* anything else is ignored */
. {}

%%

int yywrap(void) {
    return 1;
}

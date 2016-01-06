#ifndef COMINTERN_H
#define COMINTERN_H
#include "Shell.h"

void internal_cmd_exit();
void internal_cmd_host();
void internal_cmd_echo(Expression *e);
void internal_cmd_kill(Expression *e);
void internal_cmd_date(Expression *e);

void internal_cmd_remote(Expression *e);

#endif

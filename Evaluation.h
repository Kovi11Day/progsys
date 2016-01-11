#ifndef _EVALUATION_H
#define _EVALUATION_H

#include "Shell.h"

extern int evaluer_expr(Expression *e);
void execute_background(Expression *e);
void execute_sequence_and_command (Expression *e);
void execute_sequence_ou_command(Expression *e);
void execute_pipe_command(Expression *e);

#endif

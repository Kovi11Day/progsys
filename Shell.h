#ifndef ANALYSE
#define ANALYSE

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>

#define NB_ARGS 50
#define TAILLE_ID 500
#define MAX_MACHINE 50

int num_elements (char** array);

char* machines[MAX_MACHINE];
int tube1[MAX_MACHINE][2];
int tube2[2]; int tube3[2];
int tube_output[2];


typedef enum expr_t {
  VIDE,	         		// Commande vide 
  SIMPLE,        		// Commande simple 
  SEQUENCE,      		// Séquence (;) 
  SEQUENCE_ET,   		// Séquence conditionnelle (&&) 
  SEQUENCE_OU,   		// Séquence conditionnelle (||) 
  BG,	         		// Tache en arriere plan 
  PIPE,	         		// Pipe 
  REDIRECTION_I, 		// Redirection entree 
  REDIRECTION_O, 		// Redirection sortie standard 
  REDIRECTION_A, 		// Redirection sortie standard, mode append 
  REDIRECTION_E, 		// Redirection sortie erreur 
  REDIRECTION_EO,		// Redirection sorties erreur et standard
  SOUS_SHELL,                   // ( shell ) 
} expr_t;

typedef struct Expression {
  expr_t type;
  struct Expression *gauche;
  struct Expression *droite;
  char   **arguments;
} Expression;

extern int yyparse(void);
Expression *ConstruireNoeud (expr_t, Expression *, Expression *, char **);
char **AjouterArg (char **, char *);
char **InitialiserListeArguments (void);
int LongueurListe(char **);
void EndOfFile(void);

void yyerror (char *s);
Expression *ExpressionAnalysee;
extern int status;
#endif /* ANALYSE */

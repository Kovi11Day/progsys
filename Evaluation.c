#include "Shell.h"
#include "Evaluation.h"
#include "Commandes_Internes.h"


#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>


 /*
  * Execute a sequence command 
  * */
void execute_sequence_command(Expression *e){
  //-- --//
  int status;
  pid_t pid = fork();

  if (pid){
    wait(&status);
    evaluer_expr(e->droite);
  }else{
    evaluer_expr(e->gauche);
    exit(0);
  }
  
}
  
    
  
 /*
  * Execute a simple command 
  * */
void execute_simple_command(Expression *e){

  if (!strcmp (e->arguments[0], "exit"))
    internal_cmd_exit();
  else if (!strcmp (e->arguments[0], "remote")){
    internal_cmd_remote(e);
    return;
  }
  else if (!strcmp (e->arguments[0], "hostname")){
    internal_cmd_host();
    return;
  }
  else if (!strcmp (e->arguments[0], "echo")){
    internal_cmd_echo(e);
    return;
  }
  else if (!strcmp (e->arguments[0], "kill")){
    internal_cmd_kill(e);
    return;
  }
   else if (!strcmp (e->arguments[0], "date")){
    internal_cmd_date(e);
    return;
   }  
   else if (!strcmp (e->arguments[0], "cd")){
    internal_cmd_cd(e);
    return;
   }
   else if (!strcmp (e->arguments[0], "pwd")){
    internal_cmd_pwd(e);
    return;
   }
   else if (!strcmp (e->arguments[0], "history")){
    internal_cmd_history(e);
    return;
   }
  
  
  int tmp;
  int pid = fork();
  if(pid<0)// Send an error if the fork fails
    printf("Fork failed");
		
  else if(pid==0){ //The son execute the command with the exec function
    execvp(e->arguments[0],&e->arguments[0]);
    perror("exec Faild");
    exit(0);
  }
  else{ // The father wait for his son
    wait(&tmp);
  }
}


/*
 * Execute or sequence ( || )
 * */	
 void execute_sequence_ou_command(Expression *e){
	 int pid = fork();
	 int tmp;
	 if(pid<0)
		perror("Fork failed");
	 else if(pid>0) //the father wait his son
			 wait(&tmp);
	 else{
		 execvp(e->gauche->arguments[0],e->gauche->arguments);//First exec
		 /*if the first exec failed => execute the secend */
		 execvp(e->droite->arguments[0],e->droite->arguments);//secend exec
		 perror("Exec failed");
		 exit(0);
		 
		 }
}

/*
 * execute and sequence ( && )
 * */			
 void execute_sequence_and_command (Expression *e){
	 int pid= fork();
	 int tmp;
	 if(pid<0)
	 perror("Fork failed");
	 
	 else if(pid==0){ //the son excutes the first command
		 execvp(e->gauche->arguments[0],e->gauche->arguments);
		 
		 return;
	 }
	 else{//the father excutes the second
		 wait(&tmp);
		 execvp(e->droite->arguments[0],e->droite->arguments);
		exit(0);
		 }
}
 /*
  * Execute a command in background (&)
 * */
 void execute_background(Expression *e){
	 int pid = fork();
	 int tmp;
	 if(pid<0)
		perror("fork failed ");
	else if(pid==0){
		execvp(e->gauche->arguments[0],e->gauche->arguments);
		perror("exec failed");
		exit(0);
		}
		else{
			waitpid(pid,&tmp,WNOHANG);
		}
}
/*
 * execute a pipe command.
*/
void execute_pipe_command(Expression *e){
		int tmp;
		int fd[2];
		pipe(fd);
		
		int first = fork();
		
		if(first==-1)
			perror("Fork failed");
		else if(first>0){
			
			int second = fork();
			if (second <-1)
			perror("second fork failed");
			else if (second ==0){
				close (fd[0]);
				dup2(fd[1],1);
				execvp (e->gauche->arguments[0],&e->gauche->arguments[0]);
				perror("first exec failed");
				exit(0);
				}
			else{
				close (fd[1]);
				dup2(fd[0],0);
				execvp (e->droite->arguments[0],&e->droite->arguments[0]);
				perror("second exec failed");
				exit(0);
				
				}
		         
		}
		else
				waitpid(first,&tmp,0);
	
}


int evaluer_expr(Expression *e)
{
    
  
  switch(e->type){
  case VIDE:
    return 0;
  case SIMPLE:
    execute_simple_command(e);
    break;	
  case SEQUENCE_OU:
    execute_sequence_ou_command(e);
    break;
  case SEQUENCE_ET:
    execute_sequence_and_command(e);
    break;
  case BG:
    execute_background(e);
    break;
  case SEQUENCE:
    execute_sequence_command(e);
    break;
  case PIPE:
	execute_pipe_command(e);
	break;
  default:
	break;
	
  }
  
  return 1;
}

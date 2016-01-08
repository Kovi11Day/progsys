#include "Commandes_Internes.h"

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <fcntl.h>

void internal_cmd_exit(){
  raise(9);//send signal to calling process
  //execlp("kill", "kill", "-9", getpid(), NULL);
}

void internal_cmd_host(){
  char hostname[1024];
  hostname[1023] = '\0';
  gethostname(hostname, 1023);
  printf("Hostname: %s\n", hostname);
}

void internal_cmd_echo(Expression *e)
{
  size_t i;
  if (num_elements(e->arguments) > 1)
    printf("%s",(e->arguments[1]));
  for (i = 2; i < num_elements(e->arguments); i++)
    printf(" %s",(e->arguments[i]));
  printf("\n");
}

void internal_cmd_kill(Expression *e)
{ int  pid=atoi(e->arguments[2]);
  int sig=atoi(e->arguments[1]);
   kill(pid,sig); 
 }

void internal_cmd_date(Expression *e){
  time_t current_time;
  char* c_time_string;
  
  current_time = time(NULL);
  c_time_string = ctime (&current_time);
  
  printf("%s\n", c_time_string);
}
void internal_cmd_cd (Expression *e){
	if(e->arguments[1] != NULL)
		chdir(e->arguments[1]);
	else
		chdir(getenv("HOME"));
}
void internal_cmd_pwd (Expression *e){
int i;
	for(i=0;list_of_directories[i]!=NULL;i++)
		printf("%s\n",list_of_directories[i]);	
}


//////////////////////////remote_shell////////////////////////////////////////

/////redirection dynamique pipe-shell
void setpath_exec(int i){
  //default path when listeners created
  dup2 (tube1[i][0], STDIN_FILENO);
  dup2 (tube2[1], STDOUT_FILENO);
}
			 

void setpath_output(){
  dup2 (tube3[0], STDIN_FILENO);
  dup2 (tube_output[1], STDOUT_FILENO);
  }

void remote_add (Expression *e){
 
  int num_args = num_elements(e->arguments);
  int num_remote_shell = num_args - 2;

  //enregistrer liste de machines
  //create list in Shell.h or where they call this function
  //then simply add names to list here
  
  int start = num_elements(machines);
  
  for (int i = start; i < num_remote_shell; ++i)
    machines[i] = e->arguments[i + 2];
  
  
  pid_t myShell = getpid();

  //tubes btw myShell and listeners

  for (int i = start; i < num_remote_shell; ++i)
    pipe (tube1[i]);
  
  pid_t pid_listeners[num_remote_shell]; //myShell: pid of each listener


  for (int i = 0; i < num_remote_shell; ++i){

    if ((pid_listeners[i] = fork()) == 0){
      //ACTION FILS LISTENER
      //i am a listener_i
      //i will create a son to execute  >>ssh name_i

      close (tube1[i][1]);

      
      pid_t pid_remoteShell_i;

      pipe (tube2);
      pipe (tube3);
      pipe (tube_output);

      close (tube2[0]);
      close (tube3[1]);
      close (tube_output[0]);

      //default path
      setpath_exec(i);

      
      if ((pid_remoteShell_i = fork()) == 0){
	//OUTPUT WINDOW
	close (tube_output[1]);

	//stdin = tube_output[0]
	dup2 (tube_output[0], STDIN_FILENO);

	execlp("./xcat.sh", "xcat.sh", NULL);
      }

      if ((pid_remoteShell_i = fork()) == 0){
	//ACTION REMOTE SHELL
	//i am remoteShell_i
	close (tube2[1]);
	close (tube3[0]);

	//redirection stdin=tube2[0]
	dup2 (tube2[0], STDIN_FILENO);
	//redirection stdout=tube3[1]
	dup2 (tube3[1], STDOUT_FILENO);
	
	//execlp("ssh", "ssh", e->arguments[2], NULL);
	execlp("./Shell", "./Shell", NULL); 
	//ssh-cd-./Shell
	
      }else{
	//ACTION LISTENER AFTER CREATING REMOTE SHELL
	//i am listener_i
	wait(NULL);
      }
      

    }else{
      //myShell
      if (i == num_remote_shell - 2){
	//REMOTE REMOVE DOES WAIT ON LISTENERS AND REMOTE SHELLS
	for (int i = 0; i < num_remote_shell; ++i)
	wait(NULL);
	}
    }
    
  }

      //myShell 
    for (int i = 0; i < num_remote_shell; ++i){
      close (tube1[i][0]);
    }
}

void remote_all (Expression *e){
  ////
}

void remote_name_cmd(Expression *e){
  //redirection stdout -> listener number #name
  int i = 0;
  //retrieve
  while (!strcmp (e->arguments[1], machines[i]))
    i++;

  
  
  //redirection stdout = tube1[i][1]
  //dup2 (tube1[i][1], STDOUT_FILENO);

  //send sigusr1 
  printf ("%s\n", e->arguments[2]);
  
  
  

      
}

void internal_cmd_remote(Expression *e){

  if (!strcmp (e->arguments[1], "add"))
    remote_add(e);
  else if (!strcmp (e->arguments[1], "all"))
    remote_all(e);
  else //remote nom-machine commande
    remote_name_cmd(e);
}

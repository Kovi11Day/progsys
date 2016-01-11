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

void remote_add(Expression *e){
  setvbuf (stdin, NULL, _IONBF, 0);
  int tube_display_results[2]; pipe (tube_display_results);
  int tube_send_cmd[2]; pipe (tube_send_cmd);
  

  f_send_cmd = fdopen (tube_send_cmd[1], "w");
  setvbuf (f_send_cmd, NULL, _IOLBF, 0);
  
  pid_t pid_remoteShell;

  if ((pid_remoteShell = fork()) == 0){
    //remote shell
    setsid (); //remote process in bg
    int tube_send_results[2]; pipe (tube_send_results);
    pid_t pid_tee;
    if ((pid_tee = fork()) == 0){
      //TEE_i
      close (tube_send_results[1]);
      dup2 (tube_send_results[0], STDIN_FILENO);
      close (tube_send_results[0]);

      close (tube_display_results[0]);
      dup2 (tube_display_results[1], STDOUT_FILENO);
      close (tube_display_results[1]);

      execlp ("tee", "tee", NULL);
      printf ("MY_ERROR: execlp TEE failed\n");
    }else{
      
      //REMOTE_SHELL_i
      close (tube_display_results[0]); close (tube_display_results[1]);

      setvbuf (stdin, NULL, _IOLBF, 0);
      close (tube_send_cmd[1]); fclose (f_send_cmd);
      dup2 (tube_send_cmd[0], STDIN_FILENO);
      close (tube_send_cmd[0]);
      
      setvbuf (stdout, NULL, _IONBF, 0);
      close (tube_send_results[0]);
      dup2 (tube_send_results[1], STDOUT_FILENO);
      close (tube_send_results[1]);


      execlp ("ssh", "ssh", "infini1", NULL);
      printf ("MY_ERROR: execlp SSH failed\n");
    }
  }else{
    //myShell
    pid_t window;
    if ((window = fork()) == 0){
      //OUTPUT WINDOW_i
      setvbuf (stdin, NULL, _IONBF, 0);      
      close (tube_display_results[1]);
      dup2 (tube_display_results[0], STDIN_FILENO);
      close (tube_display_results[0]);

      execlp ("./xcat.sh", "xcat.sh", NULL);
      printf ("MY_ERROR: execlp XCAT.SH failed\n");
    }else{
      //myShell
      close (tube_display_results[0]); close (tube_display_results[1]);
      //cpy_myShell_stdout = STDOUT_FILENO;

      //dup2 (tube_send_cmd[1], STDOUT_FILENO);
      //close (tube_send_cmd[1]);

      close (tube_send_cmd[0]);
      sleep(0.01);
      fprintf(f_send_cmd, "%s\n", "cd progsys/projet/mybranch2/progsys");
      fprintf(f_send_cmd, "%s\n", "./Shell");
      /*Expression* init1 = e;
      Expression* init2 = e;
      init1->arguments[2] = "cd progsys/projet/mybranch2/progsys";
      init2->arguments[2] = "pwd";
      sleep (0.01); remote_name_cmd (init1);
      sleep (0.01); remote_name_cmd (init2);*/
      
      /*sleep (0.01);
      printf ("cd progsys/projet/mybranch2/progsys\n");
      sleep (0.01);
      printf ("./Shell\n"); */
    }
  }
}

void remote_all (Expression *e){
  ////
}

void remote_name_cmd(Expression *e){
  fprintf(f_send_cmd, "%s\n", e->arguments[2]);
  //redirection to tube_i
  //dup2 (tube_send_cmd[1], STDOUT_FILENO);
  
  //fprintf("%s\n", e->arguments[2]); sleep(0.01);

  //restore stdout
  //dup2 (cpy_myShell_stdout, STDOUT_FILENO);
      
}

void internal_cmd_remote(Expression *e){

  if (!strcmp (e->arguments[1], "add"))
    remote_add(e);
  else if (!strcmp (e->arguments[1], "all"))
    remote_all(e);
  else //remote nom-machine commande
    remote_name_cmd(e);
}

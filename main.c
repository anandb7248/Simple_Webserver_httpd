#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include "simple_net.h"
#include "getPort.h"
#include "checked.h"
#include "handle_request.h"
#include "signal_handler.h"
#include "limit_fork.h"

#define QUEUE_SIZE 15

int main(int argc, char* argv[])
{
   /* Get port number from command line */
   unsigned short port = getPort(argc, argv);
   /* Create a socket out of the port number */
   int socketFD = create_service(port, QUEUE_SIZE);
   int newSocketFD;
   pid_t pid;

   limit_fork(101);

   /* Setup a signal handler that will wait for terminated child processes */
   signal_setup(SIGCHLD);

   while(1)
   {
      /* Accept new connection */
      if((newSocketFD = accept_connection(socketFD)) < 0)
      {
         fprintf(stderr, "accept_connection failure\n");
         exit(-1);
      }

      pid = checked_fork();

      if(pid == 0)
      { /* Child - Client */
         close(socketFD);
         handle_request(newSocketFD);
      }
      else
      { /* Parent - Server */  
         close(newSocketFD);
      }
   }

   return 0;
}

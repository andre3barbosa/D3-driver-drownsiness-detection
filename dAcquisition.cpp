//#include <sys/types.h>  //pid_t
#include <unistd.h>     //fork setsid getsid
#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <sys/stat.h> // umask


#include "CDaemon.h"

int main(int argc, char *argv[])
{
    //daemon creation
    /*pid_t pid, sid;


	pid = fork(); // create a new process

	if (pid < 0) { // on error exit
		perror("fork");
	    exit(EXIT_FAILURE);
	}

	if (pid > 0){  
		syslog(LOG_INFO, "Deamon PID: %d\n", pid);	
		exit(EXIT_SUCCESS); // parent process (exit)
	}
	sid = setsid(); // create a new session

	if (sid < 0) { // on error exit
		perror("setsid");
		exit(EXIT_FAILURE);
	}
	// make '/' the root directory
	if (chdir("/") < 0) { // on error exit
		perror("chdir");
		exit(EXIT_FAILURE);
	}
	umask(0);
	close(STDIN_FILENO);  // close standard input file descriptor
	close(STDOUT_FILENO); // close standard output file descriptor
	close(STDERR_FILENO); // close standard error file descriptor

    syslog(LOG_INFO, "Daemon creation finished!\n");
    */

    //service implementation
    //instantiation of CDaemon object
    
    CDaemon daemon;

    daemon.init();
    daemon.run();
    //while(1);
    return 0;
}